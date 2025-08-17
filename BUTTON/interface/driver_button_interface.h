#ifndef __DRIVER_BUTTON_INTERFACE_H
#define __DRIVER_BUTTON_INTERFACE_H

#include <stdint.h>
#include "driver_led_interface.h" // 复用led_status_t

#ifdef __cplusplus
extern "C" {
#endif

// 定义一个回调函数指针类型，当中断发生时，BSP层将调用这个函数
typedef void (*button_irq_callback_t)(void* handle);

/**
 * @brief 定义了按键驱动所需的所有平台依赖项的API函数指针结构体。
 */
typedef struct button_api_s {
    /**
     * @brief 初始化按键的底层硬件 (GPIO, EXTI)。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @param[in] callback - 当此按键的中断发生时，需要被调用的回调函数。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*init)(void* handle, button_irq_callback_t callback);

    /**
     * @brief 反初始化按键的底层硬件。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*deinit)(void* handle);

    /**
     * @brief 获取系统时间戳 (单位: 毫秒)。
     * @return 当前系统时间戳。
     */
    uint32_t (*get_tick)(void);

} button_api_t;


#ifdef __cplusplus
}
#endif

#endif // __DRIVER_BUTTON_INTERFACE_H
