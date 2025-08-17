#include "driver_i2c_bsp.h"

#include <stddef.h>

// 假设我们使用I2C1，hi2c1由CubeMX自动生成
extern I2C_HandleTypeDef hi2c1;
const bsp_i2c_handle_t g_bsp_i2c1 = {&hi2c1};


static led_status_t stm32_i2c_init(void* handle) {
    // I2C的初始化通常由CubeMX生成的代码在main函数中自动完成，
    // 这里我们假设它已经被调用。如果需要，可以添加额外的初始化逻辑。
    (void)handle;
    return LED_STATUS_OK;
}

static led_status_t stm32_i2c_deinit(void* handle) {
    const bsp_i2c_handle_t* bsp_handle = (const bsp_i2c_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;

    if (HAL_I2C_DeInit(bsp_handle->hi2c) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    return LED_STATUS_OK;
}

static led_status_t stm32_i2c_mem_write_dma(void* handle, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, const uint8_t* data, uint16_t len) {
    const bsp_i2c_handle_t* bsp_handle = (const bsp_i2c_handle_t*)handle;
    if (bsp_handle == NULL || data == NULL || len == 0) {
        return LED_STATUS_INV_ARG;
    }

    // HAL库的设备地址需要左移一位
    if (HAL_I2C_Mem_Write_DMA(bsp_handle->hi2c, (dev_address << 1), mem_address, mem_addr_size, (uint8_t*)data, len) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    
    // 等待DMA传输完成
    // 注意：在实际应用中，为了不阻塞，这里应该使用中断或信号量来同步
    while (HAL_I2C_GetState(bsp_handle->hi2c) != HAL_I2C_STATE_READY);

    return LED_STATUS_OK;
}

static led_status_t stm32_i2c_mem_read_dma(void* handle, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, uint8_t* data, uint16_t len) {
    const bsp_i2c_handle_t* bsp_handle = (const bsp_i2c_handle_t*)handle;
    if (bsp_handle == NULL || data == NULL || len == 0) {
        return LED_STATUS_INV_ARG;
    }

    if (HAL_I2C_Mem_Read_DMA(bsp_handle->hi2c, (dev_address << 1), mem_address, mem_addr_size, data, len) != HAL_OK) {
        return LED_STATUS_ERROR;
    }

    // 等待DMA传输完成
    while (HAL_I2C_GetState(bsp_handle->hi2c) != HAL_I2C_STATE_READY);

    return LED_STATUS_OK;
}

static led_status_t stm32_i2c_is_device_ready(void* handle, uint16_t dev_address, uint32_t timeout_ms) {
    const bsp_i2c_handle_t* bsp_handle = (const bsp_i2c_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;

    // 尝试通信1次，超时时间为timeout_ms
    if (HAL_I2C_IsDeviceReady(bsp_handle->hi2c, (dev_address << 1), 1, timeout_ms) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    return LED_STATUS_OK;
}


// 填充API结构体实例
static const i2c_api_t s_i2c_api_stm32 = {
    .init = stm32_i2c_init,
    .deinit = stm32_i2c_deinit,
    .mem_write_dma = stm32_i2c_mem_write_dma,
    .mem_read_dma = stm32_i2c_mem_read_dma,
    .is_device_ready = stm32_i2c_is_device_ready,
    .get_tick = HAL_GetTick,
};

const i2c_api_t* bsp_i2c_get_api(void) {
    return &s_i2c_api_stm32;
}
