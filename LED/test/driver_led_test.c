#include "driver_led_test.h"

// 定义两个LED驱动对象
 led_t g_led1;
 led_t g_led2;


void driver_led_test(void) {
    // --- 驱动初始化 ---
    // 1. 从BSP层获取为本平台实现的统一API实例
    const led_api_t* stm32_led_api = bsp_led_get_api();

    // 2. 使用获取到的API实例和具体的硬件句柄，来初始化通用的驱动对象
    led_init(&g_led1, stm32_led_api, (void*) &g_bsp_led1);
    led_init(&g_led2, stm32_led_api, (void*) &g_bsp_led2);

    // --- 驱动使用 ---
    // 让系统LED (PF9) 以1秒的频率闪烁
    led_set_mode_blink(&g_led1, 500, 500);

    // 用户LED (PF10) 初始为关闭状态
    led_set_mode_off(&g_led2);

    uint32_t last_pulse_time = 0;

    while (1) {
        // 在主循环中周期性地调用Process函数，以驱动闪烁等需要时间变化的状态
        led_process(&g_led1);
        led_process(&g_led2);

        // 示例: 每隔5秒，让用户LED触发一次200ms的脉冲
        if (HAL_GetTick() - last_pulse_time > 5000) {
            led_trigger_pulse_once(&g_led2, 200);
            last_pulse_time = HAL_GetTick();
        }
    }
}

