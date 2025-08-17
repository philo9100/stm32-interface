#ifndef __DRIVER_I2C_INTERFACE_H
#define __DRIVER_I2C_INTERFACE_H

#include "driver_led_interface.h" // 复用led_status_t等定义
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 定义I2C内存地址大小的枚举
typedef enum {
    I2C_MEM_ADDR_SIZE_8BIT  = 1,
    I2C_MEM_ADDR_SIZE_16BIT = 2,
} i2c_mem_addr_size_t;

/**
 * @brief 定义了I2C驱动所需的所有平台依赖项的API函数指针结构体。
 */
typedef struct i2c_api_s {
    /**
     * @brief 初始化I2C的底层硬件 (GPIO, I2C, DMA)。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*init)(void* handle);

    /**
     * @brief 反初始化I2C的底层硬件。
     * @param[in] handle - 指向硬件相关句柄的void指针。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*deinit)(void* handle);

    /**
     * @brief 通过DMA向I2C从设备的指定内存地址写入数据。
     * @param[in] handle         - 指向硬件相关句柄的指针。
     * @param[in] dev_address    - 7位的I2C从设备地址。
     * @param[in] mem_address    - 要写入的从设备内部寄存器/内存地址。
     * @param[in] mem_addr_size  - 内存地址的长度 (8位或16位)。
     * @param[in] data           - 指向要发送的数据缓冲区的指针。
     * @param[in] len            - 要发送的数据长度。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*mem_write_dma)(void* handle, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, const uint8_t* data, uint16_t len);

    /**
     * @brief 通过DMA从I2C从设备的指定内存地址读取数据。
     * @param[in] handle         - 指向硬件相关句柄的指针。
     * @param[in] dev_address    - 7位的I2C从设备地址。
     * @param[in] mem_address    - 要读取的从设备内部寄存器/内存地址。
     * @param[in] mem_addr_size  - 内存地址的长度 (8位或16位)。
     * @param[out] data          - 用于存放读取数据的缓冲区。
     * @param[in] len            - 期望读取的数据长度。
     * @return led_status_t - 操作的状态码。
     */
    led_status_t (*mem_read_dma)(void* handle, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, uint8_t* data, uint16_t len);
    
    /**
     * @brief 检查指定的I2C设备是否在总线上就绪。
     * @param[in] handle         - 指向硬件相关句柄的指针。
     * @param[in] dev_address    - 7位的I2C从设备地址。
     * @param[in] timeout_ms     - 检查的超时时间。
     * @return led_status_t - 如果设备就绪返回OK，否则返回ERROR。
     */
    led_status_t (*is_device_ready)(void* handle, uint16_t dev_address, uint32_t timeout_ms);

    /**
     * @brief 获取系统时间戳 (单位: 毫秒)。
     * @return 当前系统时间戳。
     */
    uint32_t (*get_tick)(void);

} i2c_api_t;


#ifdef __cplusplus
}
#endif

#endif // __DRIVER_I2C_INTERFACE_H
