#include "driver_button.h"

/**
 * @brief 内部中断回调函数
 * @note  这个函数是传递给BSP层的，当硬件中断发生时，它被调用。
 * 它的职责非常简单：只记录中断发生的时间。
 * 这保证了中断服务程序(ISR)的执行时间极短。
 */
static void internal_irq_handler(void* handle) {
    button_t* button = (button_t*)handle;
    if (button && button->api && button->api->get_tick) {
        uint32_t current_time = button->api->get_tick();
        
        // 软件消抖核心：只处理在消抖间隔之外的第一次中断
        if (current_time - button->last_irq_time > button->debounce_time_ms) {
            // 这是一个有效的按下事件，调用上层注册的回调
            if (button->on_event_callback) {
                button->on_event_callback(button, button->user_data);
            }
        }
        // 无论如何，都更新最后一次的中断时间，以屏蔽后续的抖动
        button->last_irq_time = current_time;
    }
}

led_status_t button_init(button_t* button, const button_api_t* api, void* handle) {
    if (button == NULL || api == NULL || handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    if (api->init == NULL || api->get_tick == NULL) {
        return LED_STATUS_INV_ARG;
    }

    button->api = api;
    button->handle = handle;
    button->on_event_callback = NULL;
    button->user_data = NULL;
    button->last_irq_time = 0;
    
    // 设置一个默认的消抖时间
    button_set_debounce_time(button, 50);

    // 调用底层API初始化硬件，并将内部中断处理函数作为回调注册进去
    // 注意：这里传递的handle是button_t对象自身，而不是硬件句柄
    return button->api->init(button->handle, internal_irq_handler);
}

led_status_t button_register_event_callback(button_t* button, button_event_callback_t callback, void* user_data) {
    if (button == NULL) {
        return LED_STATUS_INV_ARG;
    }
    button->on_event_callback = callback;
    button->user_data = user_data;
    return LED_STATUS_OK;
}

led_status_t button_set_debounce_time(button_t* button, uint32_t ms) {
    if (button == NULL) {
        return LED_STATUS_INV_ARG;
    }
    button->debounce_time_ms = ms;
    return LED_STATUS_OK;
}

led_status_t button_process(button_t* button) {
    // 在我们当前的设计中，所有逻辑都在中断回调中处理了，
    // 所以process函数是空的。
    // 这种设计适用于简单的“按下即触发”事件。
    // 如果要实现更复杂的长按、双击等，就需要在这里添加状态机逻辑。
    (void)button; // 避免编译器警告
    return LED_STATUS_OK;
}