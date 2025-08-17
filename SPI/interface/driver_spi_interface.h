#ifndef __DRIVER_SPI_INTERFACE_H
#define __DRIVER_SPI_INTERFACE_H

#include "driver_led_interface.h" // 复用led_status_t等定义
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 定义了SPI驱动所需的所有平台依赖项的API函数指针结构体。
 */
typedef struct spi_api_s {
    /**
     * @brief 初始化SPI的底层硬件 (GPIO, SPI, DMA)。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*init)(void* handle);

    /**
     * @brief 反初始化SPI的底层硬件。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*deinit)(void* handle);

    /**
     * @brief 通过DMA同时发送和接收数据 (全双工)。
     * @param[in]  handle  - 指向硬件相关句柄的指针。
     * @param[in]  tx_data - 指向要发送的数据缓冲区的指针。
     * @param[out] rx_data - 用于存放接收数据的缓冲区。
     * @param[in]  len     - 要交换的数据长度。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*transceive_dma)(void* handle, const uint8_t* tx_data, uint8_t* rx_data, uint16_t len);
    
    /**
     * @brief 片选使能 (将CS/NSS引脚拉低)。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*chip_select)(void* handle);

    /**
     * @brief 片选禁止 (将CS/NSS引脚拉高)。
     * @param[in] handle - 指向硬件相关句柄的指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*chip_deselect)(void* handle);

    /**
     * @brief 获取系统时间戳 (单位: 毫秒)。
     * @return 当前系统时间戳。
     */
    uint32_t (*get_tick)(void);

} spi_api_t;


#ifdef __cplusplus
}
#endif

#endif // __DRIVER_SPI_INTERFACE_H
