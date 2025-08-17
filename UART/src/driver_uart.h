#ifndef __DRIVER_UART_H
#define __DRIVER_UART_H

#include "driver_uart_interface.h"

/**
 * @brief 串口驱动的 "对象" 或 "类" 定义
 */
typedef struct {
    const uart_api_t* api;    /**< 指向平台依赖API函数表的指针 */
    void* handle;               /**< 指向具体硬件实例句柄的void指针 */

    // --- 内部环形缓冲区 (Ring Buffer) ---
    uint8_t* rx_buffer;         /**< 指向用于接收的环形缓冲区的指针 */
    uint16_t rx_buffer_size;    /**< 环形缓冲区的大小 */
    volatile uint16_t head;     /**< 缓冲区写指针 (由中断/DMA回调更新) */
    volatile uint16_t tail;     /**< 缓冲区读指针 (由应用层读取更新) */

} uart_t;


// ===================================================================================
// 公共API函数
// ===================================================================================

/**
 * @brief  初始化一个串口驱动对象
 * @param[in] uart           - 指向要初始化的uart_t对象的指针
 * @param[in] api            - 指向底层硬件API函数表的指针
 * @param[in] handle         - 指向具体硬件实例句柄的void指针
 * @param[in] rx_buffer      - 上层应用提供的用于接收的缓冲区
 * @param[in] rx_buffer_size - 接收缓冲区的大小
 * @return led_status_t - 操作的状态码
 */
led_status_t uart_init(uart_t* uart, const uart_api_t* api, void* handle, uint8_t* rx_buffer, uint16_t rx_buffer_size);

/**
 * @brief  反初始化一个串口对象
 * @param[in] uart - 指向uart_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t uart_deinit(uart_t* uart);

/**
 * @brief  从串口读取数据 (从内部环形缓冲区读取)
 * @param[in]  uart   - 指向uart_t对象的指针
 * @param[out] data   - 用于存放读取数据的缓冲区
 * @param[in]  len    - 期望读取的数据长度
 * @return uint16_t - 实际读取到的数据长度 (可能小于期望长度)
 */
uint16_t uart_read(uart_t* uart, uint8_t* data, uint16_t len);

/**
 * @brief  向串口写入数据
 * @param[in] uart - 指向uart_t对象的指针
 * @param[in] data - 指向要发送的数据的指针
 * @param[in] len  - 要发送的数据长度
 * @return led_status_t - 操作的状态码
 */
led_status_t uart_write(uart_t* uart, const uint8_t* data, uint16_t len);

/**
 * @brief  获取当前环形缓冲区中可读的数据字节数
 * @param[in] uart - 指向uart_t对象的指针
 * @return uint16_t - 可读的字节数
 */
uint16_t uart_get_bytes_available(uart_t* uart);

#endif // __DRIVER_UART_H