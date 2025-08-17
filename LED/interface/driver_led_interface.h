#ifndef __DRIVER_LED_INTERFACE_H
#define __DRIVER_LED_INTERFACE_H

#include <stdint.h>
#include <stddef.h> // For NULL

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 定义了标准化的状态/错误码
 * @note  使用枚举替代简单的0/1返回，可以极大地增强代码的可读性和可维护性。
 */
typedef enum {
    LED_STATUS_OK           = 0,    /**< 操作成功 */
    LED_STATUS_ERROR        = 1,    /**< 通用/未知错误 */
    LED_STATUS_INV_ARG      = 2,    /**< 无效参数 */
    LED_STATUS_NOT_SUPPORTED = 3,    /**< 功能不被支持 (例如，对普通LED调用调光) */
} led_status_t;

/**
 * @brief 定义了驱动所需的所有平台依赖项的API函数指针结构体。
 * @note  这个结构体被重新命名为 led_api_t，以更准确地反映其“接口”的本质，
 * 而非一个具体的“句柄”。
 */
typedef struct led_api_s {
    /**
     * @brief 初始化LED的底层硬件。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*init)(void* handle);

    /**
     * @brief 反初始化LED的底层硬件，释放资源。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*deinit)(void* handle);

    /**
     * @brief 设置LED的开关状态。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @param[in] state  - 期望的状态 (1 代表 ON, 0 代表 OFF)。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*set_state)(void* handle, uint8_t state);

    /**
     * @brief 翻转LED的当前状态。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*toggle)(void* handle);

    /**
     * @brief (可扩展功能) 设置LED的亮度。
     * @note  这是一个为未来功能预留的接口。如果底层硬件(BSP)不支持PWM调光，
     * 可以将此函数指针设置为NULL。上层驱动在调用前必须检查其有效性。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @param[in] brightness_percent - 亮度百分比 (0-100)。
     * @return led_status_t - 操作的状态码。如果不支持，应返回 LED_STATUS_NOT_SUPPORTED。
     */
    led_status_t (*set_brightness)(void* handle, uint8_t brightness_percent);

    /**
     * @brief 获取系统时间戳 (单位: 毫秒)。
     * @return 当前系统时间戳。
     */
    uint32_t (*get_tick)(void);

} led_api_t;


#ifdef __cplusplus
}
#endif

#endif // __DRIVER_LED_INTERFACE_H
