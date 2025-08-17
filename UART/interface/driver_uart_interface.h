#ifndef __DRIVER_UART_INTERFACE_H
#define __DRIVER_UART_INTERFACE_H

#include "driver_led_interface.h" // 复用led_status_t等定义
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明，避免循环包含
struct uart_api_s;

/**
 * @brief 定义串口接收回调函数指针类型
 * @param[in] handle - 指向硬件相关句柄的void指针
 * @param[in] data   - 指向接收到的数据缓冲区的指针
 * @param[in] len    - 接收到的数据长度
 */
typedef void (*uart_rx_callback_t)(void* handle, uint8_t* data, uint16_t len);

/**
 * @brief 定义了串口驱动所需的所有平台依赖项的API函数指针结构体。
 */
typedef struct uart_api_s {
    /**
     * @brief 初始化串口的底层硬件 (GPIO, UART, DMA, 中断)。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @param[in] callback - 当接收到数据时，需要被调用的回调函数。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*init)(void* handle, uart_rx_callback_t callback);

    /**
     * @brief 反初始化串口的底层硬件。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*deinit)(void* handle);

    /**
     * @brief 通过DMA异步发送数据。
     * @note  此函数会立即返回，发送过程在后台由DMA完成。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @param[in] data   - 指向要发送的数据缓冲区的指针。
     * @param[in] len    - 要发送的数据长度。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*transmit_dma)(void* handle, const uint8_t* data, uint16_t len);
    
    /**
     * @brief 获取系统时间戳 (单位: 毫秒)。
     * @return 当前系统时间戳。
     */
    uint32_t (*get_tick)(void);

} uart_api_t;


#ifdef __cplusplus
}
#endif

#endif // __DRIVER_UART_INTERFACE_H
