#ifndef __DRIVER_BUTTON_H
#define __DRIVER_BUTTON_H

#include "driver_button_interface.h"

// 定义按键事件类型
typedef enum {
    BUTTON_EVENT_PRESSED,  /**< 按键被按下事件 */
    BUTTON_EVENT_RELEASED, /**< 按键被释放事件 (当前驱动暂不实现，为未来预留) */
    BUTTON_EVENT_LONG_PRESS, /**< 按键长按事件 (为未来预留) */
} button_event_t;

// 前向声明 button_t 结构体
struct button_s;

// 定义事件回调函数指针类型
// 参数: button_t* - 指向触发事件的按键对象; void* - 用户自定义数据
typedef void (*button_event_callback_t)(struct button_s* button, void* user_data);

/**
 * @brief 按键驱动的 "对象" 或 "类" 定义
 */
typedef struct button_s {
    const button_api_t* api;    /**< 指向平台依赖API函数表的指针 */
    void* handle;               /**< 指向具体硬件实例句柄的void指针 */

    // --- 消抖状态机 ---
    uint32_t debounce_time_ms;  /**< 消抖时间 */
    uint32_t last_irq_time;     /**< 上次中断发生的时间戳 */

    // --- 事件回调 ---
    button_event_callback_t on_event_callback; /**< 事件回调函数指针 */
    void* user_data;                           /**< 传递给回调函数的用户自定义数据 */

} button_t;


// ===================================================================================
// 公共API函数
// ===================================================================================

/**
 * @brief  初始化一个按键对象
 * @param[in] button - 指向要初始化的button_t对象的指针
 * @param[in] api    - 指向底层硬件API函数表的指针
 * @param[in] handle - 指向具体硬件实例句柄的void指针
 * @return led_status_t - 操作的状态码
 */
led_status_t button_init(button_t* button, const button_api_t* api, void* handle);

/**
 * @brief  为按键注册事件回调函数
 * @param[in] button    - 指向button_t对象的指针
 * @param[in] callback  - 当事件发生时要调用的回调函数
 * @param[in] user_data - 需要传递给回调函数的自定义数据指针
 * @return led_status_t - 操作的状态码
 */
led_status_t button_register_event_callback(button_t* button, button_event_callback_t callback, void* user_data);

/**
 * @brief  设置按键的消抖时间
 * @param[in] button - 指向button_t对象的指针
 * @param[in] ms     - 消抖时间 (毫秒)，典型值为20-50ms
 * @return led_status_t - 操作的状态码
 */
led_status_t button_set_debounce_time(button_t* button, uint32_t ms);

/**
 * @brief  按键状态处理函数 (消抖的核心)
 * @note   此函数必须在主循环中被周期性地调用。它本身不处理事件，
 * 而是由BSP层的中断回调来触发。
 * @param[in] button - 指向button_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t button_process(button_t* button);

#endif // __DRIVER_BUTTON_H