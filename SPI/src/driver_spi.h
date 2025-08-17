#ifndef __DRIVER_SPI_H
#define __DRIVER_SPI_H

#include "driver_spi_interface.h"

/**
 * @brief SPI驱动的 "对象" 或 "类" 定义
 * @note  它封装了SPI总线的操作接口。
 */
typedef struct {
    const spi_api_t* api;    /**< 指向平台依赖API函数表的指针 */
    void* handle;               /**< 指向具体硬件实例句柄的void指针 */
} spi_t;


// ===================================================================================
// 公共API函数
// ===================================================================================

/**
 * @brief  初始化一个SPI总线管理器对象
 * @param[in] spi    - 指向要初始化的spi_t对象的指针
 * @param[in] api    - 指向底层硬件API函数表的指针
 * @param[in] handle - 指向具体硬件实例句柄的void指针
 * @return led_status_t - 操作的状态码
 */
led_status_t spi_init(spi_t* spi, const spi_api_t* api, void* handle);

/**
 * @brief  反初始化一个SPI总线管理器对象
 * @param[in] spi - 指向spi_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t spi_deinit(spi_t* spi);

/**
 * @brief  执行一次完整的SPI事务 (片选->数据交换->取消片选)。
 * @note   这是核心的、最灵活的函数。
 * @param[in]  spi     - 指向spi_t对象的指针。
 * @param[in]  tx_data - 指向要发送的数据缓冲区的指针。
 * @param[out] rx_data - 用于存放接收数据的缓冲区。
 * @param[in]  len     - 要交换的数据长度。
 * @return led_status_t - 操作的状态码。
 */
led_status_t spi_transceive(spi_t* spi, const uint8_t* tx_data, uint8_t* rx_data, uint16_t len);

/**
 * @brief  向SPI总线写入数据 (便利性封装函数)。
 * @note   在写入时，MISO线上的数据将被忽略。
 * @param[in] spi     - 指向spi_t对象的指针。
 * @param[in] tx_data - 指向要发送的数据缓冲区的指针。
 * @param[in] len     - 要发送的数据长度。
 * @return led_status_t - 操作的状态码。
 */
led_status_t spi_write(spi_t* spi, const uint8_t* tx_data, uint16_t len);

/**
 * @brief  从SPI总线读取数据 (便利性封装函数)。
 * @note   在读取时，会从MOSI线发送虚拟数据(通常是0xFF)。
 * @param[in]  spi     - 指向spi_t对象的指针。
 * @param[out] rx_data - 用于存放接收数据的缓冲区。
 * @param[in]  len     - 期望读取的数据长度。
 * @return led_status_t - 操作的状态码。
 */
led_status_t spi_read(spi_t* spi, uint8_t* rx_data, uint16_t len);

#endif // __DRIVER_SPI_H

