#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "driver_spi_interface.h"
#include "stm32f4xx_hal.h"

/**
 * @brief 包含STM32平台SPI硬件具体信息的句柄结构体。
 * @note  它不仅包含SPI外设句柄，还包含了手动控制的CS引脚信息。
 */
typedef struct {
    SPI_HandleTypeDef* const hspi;    /**< 指向HAL库SPI句柄的指针 */
    GPIO_TypeDef* const cs_port; /**< CS引脚所在的GPIO端口 */
    const uint16_t           cs_pin;  /**< CS引脚号 */
} bsp_spi_handle_t;

/**
 * @brief 获取为STM32平台实现的SPI硬件API单例。
 * @return 一个指向spi_api_t结构体的常量指针。
 */
const spi_api_t* bsp_spi_get_api(void);

/**
 * @brief 通过extern声明开发板上定义的SPI硬件句柄。
 */
extern const bsp_spi_handle_t g_bsp_spi1;

#endif // __BSP_SPI_H