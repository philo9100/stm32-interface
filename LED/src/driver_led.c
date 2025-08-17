#include "driver_led.h"

// 内部辅助函数，用于安全地调用底层硬件API来设置状态
static led_status_t set_led_state(led_t* led, uint8_t state) {
    if (led->api && led->api->set_state) {
        led->is_on = state; // 更新逻辑状态
        return led->api->set_state(led->handle, state);
    }
    return LED_STATUS_ERROR;
}

led_status_t led_init(led_t* led, const led_api_t* api, void* handle) {
    // 防御性编程: 检查所有指针是否有效
    if (led == NULL || api == NULL || handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 检查必要的API函数是否已实现
    if (api->init == NULL || api->set_state == NULL || api->get_tick == NULL) {
        return LED_STATUS_INV_ARG;
    }

    led->api = api;
    led->handle = handle;

    // 调用底层API初始化硬件
    led_status_t res = led->api->init(led->handle);
    if (res != LED_STATUS_OK) {
        return res;
    }

    // 设置驱动的初始状态为OFF
    return led_set_mode_off(led);
}

led_status_t led_deinit(led_t* led) {
    if (led == NULL || led->api == NULL || led->api->deinit == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 先将LED关闭
    (void)led_set_mode_off(led);
    // 再调用底层API反初始化硬件
    return led->api->deinit(led->handle);
}

led_status_t led_set_mode_on(led_t* led) {
    if (led == NULL) return LED_STATUS_INV_ARG;
    led->mode = LED_MODE_ON;
    return set_led_state(led, 1); // 1 for ON
}

led_status_t led_set_mode_off(led_t* led) {
    if (led == NULL) return LED_STATUS_INV_ARG;
    led->mode = LED_MODE_OFF;
    return set_led_state(led, 0); // 0 for OFF
}

led_status_t led_set_mode_blink(led_t* led, uint32_t on_time_ms, uint32_t off_time_ms) {
    if (led == NULL) return LED_STATUS_INV_ARG;

    led->mode = LED_MODE_BLINK;
    led->params.blink.on_time_ms = on_time_ms;
    led->params.blink.off_time_ms = off_time_ms;

    // 进入闪烁模式时，立即点亮LED并重置计时器
    led->last_event_time = led->api->get_tick();
    return set_led_state(led, 1);
}

led_status_t led_trigger_pulse_once(led_t* led, uint32_t duration_ms) {
    if (led == NULL) return LED_STATUS_INV_ARG;

    led->mode = LED_MODE_PULSE_ONCE;
    led->params.pulse.duration_ms = duration_ms;

    // 立即触发脉冲：点亮LED并重置计时器
    led->last_event_time = led->api->get_tick();
    return set_led_state(led, 1);
}

led_status_t led_process(led_t* led) {
    if (led == NULL) return LED_STATUS_INV_ARG;

    // 状态机核心: 只有需要时间驱动的模式才需要处理
    switch (led->mode) {
        case LED_MODE_BLINK:
        {
            uint32_t current_time = led->api->get_tick();
            uint32_t interval = led->is_on ? 
                                led->params.blink.on_time_ms : 
                                led->params.blink.off_time_ms;

            if (current_time - led->last_event_time >= interval) {
                // 时间到了，翻转状态
                if (led->api->toggle) {
                    led->is_on = !led->is_on; // 更新逻辑状态
                    led->last_event_time = current_time;
                    return led->api->toggle(led->handle);
                }
            }
            break;
        }

        case LED_MODE_PULSE_ONCE:
        {
            uint32_t current_time = led->api->get_tick();
            if (current_time - led->last_event_time >= led->params.pulse.duration_ms) {
                // 脉冲结束，自动切换到OFF模式
                return led_set_mode_off(led);
            }
            break;
        }

        case LED_MODE_ON:
        case LED_MODE_OFF:
        default:
            // 在这些静态模式下，无需任何周期性处理
            break;
    }
    return LED_STATUS_OK;
}