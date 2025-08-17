#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include "driver_i2c_interface.h"

/**
 * @brief I2C驱动的 "对象" 或 "类" 定义
 * @note  它封装了I2C总线的操作接口。
 */
typedef struct {
    const i2c_api_t* api;    /**< 指向平台依赖API函数表的指针 */
    void* handle;               /**< 指向具体硬件实例句柄的void指针 */
} i2c_t;


// ===================================================================================
// 公共API函数
// ===================================================================================

/**
 * @brief  初始化一个I2C总线管理器对象
 * @param[in] i2c    - 指向要初始化的i2c_t对象的指针
 * @param[in] api    - 指向底层硬件API函数表的指针
 * @param[in] handle - 指向具体硬件实例句柄的void指针
 * @return led_status_t - 操作的状态码
 */
led_status_t i2c_init(i2c_t* i2c, const i2c_api_t* api, void* handle);

/**
 * @brief  反初始化一个I2C总线管理器对象
 * @param[in] i2c - 指向i2c_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t i2c_deinit(i2c_t* i2c);

/**
 * @brief  向I2C从设备的指定内存地址写入数据。
 * @param[in] i2c            - 指向i2c_t对象的指针。
 * @param[in] dev_address    - 7位的I2C从设备地址。
 * @param[in] mem_address    - 要写入的从设备内部寄存器/内存地址。
 * @param[in] mem_addr_size  - 内存地址的长度 (8位或16位)。
 * @param[in] data           - 指向要发送的数据缓冲区的指针。
 * @param[in] len            - 要发送的数据长度。
 * @return led_status_t - 操作的状态码。
 */
led_status_t i2c_mem_write(i2c_t* i2c, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, const uint8_t* data, uint16_t len);

/**
 * @brief  从I2C从设备的指定内存地址读取数据。
 * @param[in]  i2c           - 指向i2c_t对象的指针。
 * @param[in]  dev_address   - 7位的I2C从设备地址。
 * @param[in]  mem_address   - 要读取的从设备内部寄存器/内存地址。
 * @param[in]  mem_addr_size - 内存地址的长度 (8位或16位)。
 * @param[out] data          - 用于存放读取数据的缓冲区。
 * @param[in]  len           - 期望读取的数据长度。
 * @return led_status_t - 操作的状态码。
 */
led_status_t i2c_mem_read(i2c_t* i2c, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, uint8_t* data, uint16_t len);

/**
 * @brief  检查指定的I2C设备是否在总线上就绪。
 * @param[in] i2c            - 指向i2c_t对象的指针。
 * @param[in] dev_address    - 7位的I2C从设备地址。
 * @param[in] timeout_ms     - 检查的超时时间。
 * @return led_status_t - 如果设备就绪返回OK，否则返回ERROR。
 */
led_status_t i2c_is_device_ready(i2c_t* i2c, uint16_t dev_address, uint32_t timeout_ms);

#endif // __DRIVER_I2C_H
