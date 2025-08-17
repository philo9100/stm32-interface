#include "driver_spi_bsp.h"

#include <stddef.h>

// 假设我们使用SPI1，hspi1由CubeMX自动生成
// 假设CS引脚为PA4
extern SPI_HandleTypeDef hspi1;
const bsp_spi_handle_t g_bsp_spi1 = {
    .hspi = &hspi1,
    .cs_port = GPIOA,
    .cs_pin = GPIO_PIN_4
};


static led_status_t stm32_spi_init(void* handle) {
    // SPI和CS引脚的GPIO初始化通常由CubeMX生成的代码在main中自动完成。
    // 我们在这里需要确保CS引脚初始状态为高电平 (未选中)。
    const bsp_spi_handle_t* bsp_handle = (const bsp_spi_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;
    
    HAL_GPIO_WritePin(bsp_handle->cs_port, bsp_handle->cs_pin, GPIO_PIN_SET);
    return LED_STATUS_OK;
}

static led_status_t stm32_spi_deinit(void* handle) {
    const bsp_spi_handle_t* bsp_handle = (const bsp_spi_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;

    if (HAL_SPI_DeInit(bsp_handle->hspi) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    return LED_STATUS_OK;
}

static led_status_t stm32_spi_transceive_dma(void* handle, const uint8_t* tx_data, uint8_t* rx_data, uint16_t len) {
    const bsp_spi_handle_t* bsp_handle = (const bsp_spi_handle_t*)handle;
    if (bsp_handle == NULL || tx_data == NULL || rx_data == NULL || len == 0) {
        return LED_STATUS_INV_ARG;
    }

    if (HAL_SPI_TransmitReceive_DMA(bsp_handle->hspi, (uint8_t*)tx_data, rx_data, len) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    
    // 等待DMA传输完成
    // 注意：在非阻塞的RTOS应用中，这里应该使用中断或信号量来同步，而不是轮询
    while (HAL_SPI_GetState(bsp_handle->hspi) != HAL_SPI_STATE_READY);

    return LED_STATUS_OK;
}

static led_status_t stm32_spi_chip_select(void* handle) {
    const bsp_spi_handle_t* bsp_handle = (const bsp_spi_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;
    
    HAL_GPIO_WritePin(bsp_handle->cs_port, bsp_handle->cs_pin, GPIO_PIN_RESET);
    return LED_STATUS_OK;
}

static led_status_t stm32_spi_chip_deselect(void* handle) {
    const bsp_spi_handle_t* bsp_handle = (const bsp_spi_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;

    HAL_GPIO_WritePin(bsp_handle->cs_port, bsp_handle->cs_pin, GPIO_PIN_SET);
    return LED_STATUS_OK;
}


// 填充API结构体实例
static const spi_api_t s_spi_api_stm32 = {
    .init = stm32_spi_init,
    .deinit = stm32_spi_deinit,
    .transceive_dma = stm32_spi_transceive_dma,
    .chip_select = stm32_spi_chip_select,
    .chip_deselect = stm32_spi_chip_deselect,
    .get_tick = HAL_GetTick,
};

const spi_api_t* bsp_spi_get_api(void) {
    return &s_spi_api_stm32;
}

