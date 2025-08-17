#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "driver_i2c_interface.h"
#include "stm32f4xx_hal.h"

/**
 * @brief 包含STM32平台I2C硬件具体信息的句柄结构体。
 */
typedef struct {
    I2C_HandleTypeDef* const hi2c; /**< 指向HAL库I2C句柄的指针 */
} bsp_i2c_handle_t;

/**
 * @brief 获取为STM32平台实现的I2C硬件API单例。
 * @return 一个指向i2c_api_t结构体的常量指针。
 */
const i2c_api_t* bsp_i2c_get_api(void);

/**
 * @brief 通过extern声明开发板上定义的I2C硬件句柄。
 */
extern const bsp_i2c_handle_t g_bsp_i2c1;

#endif // __BSP_I2C_H