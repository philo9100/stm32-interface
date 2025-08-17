#include "driver_button_test.h"

// 1. 定义驱动对象
extern led_t g_led1;
extern led_t g_led2;
button_t g_key0;
button_t g_key1;

// 2. 定义应用逻辑相关的变量
led_t* g_leds[] = { &g_led1, &g_led2 }; // 一个指向LED对象的指针数组，方便切换
const uint8_t g_led_count = sizeof(g_leds) / sizeof(g_leds[0]);
volatile uint8_t g_selected_led_index = 0; // 当前被选中的LED索引 (volatile很重要)

// 3. 定义按键事件的回调函数
void key0_pressed_callback(button_t* button, void* user_data);
void key1_pressed_callback(button_t* button, void* user_data);



void driver_button_test(void) {

    /* 驱动初始化 -------------------------------------------------------------*/
    // a. 从BSP层获取为本平台实现的API实例
    const led_api_t* led_api = bsp_led_get_api();
    const button_api_t* button_api = bsp_button_get_api();

    // b. 初始化LED对象
    led_init(&g_led1, led_api, (void*) &g_bsp_led1);
    led_init(&g_led2, led_api, (void*) &g_bsp_led2);

    // c. 初始化按键对象
    button_init(&g_key0, button_api, (void*) &g_bsp_key0);
    button_init(&g_key1, button_api, (void*) &g_bsp_key1);

    // d. 为按键注册事件回调函数
    button_register_event_callback(&g_key0, key0_pressed_callback, NULL);
    button_register_event_callback(&g_key1, key1_pressed_callback, NULL);

    /* 应用逻辑初始化 ----------------------------------------------------------*/
    // 初始状态下，让被选中的LED1闪烁一下作为提示
    led_trigger_pulse_once(g_leds[g_selected_led_index], 100);

    while (1) {
        /* 周期性处理驱动状态机 ---------------------------------------------------*/
    // 这个循环非常干净，只负责调用Process函数驱动状态变化
        led_process(&g_led1);
        led_process(&g_led2);

        // // button_process目前是空的，但保留是好习惯
        // button_process(&g_key0);
        // button_process(&g_key1);
    }
}

/**
 * @brief 按键1 (选择键) 按下时的回调函数
 */
void key0_pressed_callback(button_t* button, void* user_data)
{
    // 切换到下一个LED
    g_selected_led_index = (g_selected_led_index + 1) % g_led_count;
    
    // 让新选中的LED闪烁一下作为提示
    led_trigger_pulse_once(g_leds[g_selected_led_index], 100);
}

/**
 * @brief 按键2 (控制键) 按下时的回调函数
 */
void key1_pressed_callback(button_t* button, void* user_data)
{
    // 获取当前被选中的LED对象
    led_t* selected_led = g_leds[g_selected_led_index];

    // 判断当前LED的状态并翻转它
    // 注意：这里我们只关心ON和OFF两种稳定状态
    if (selected_led->mode == LED_MODE_ON) {
        led_set_mode_off(selected_led);
    } else {
        led_set_mode_on(selected_led);
    }
}


