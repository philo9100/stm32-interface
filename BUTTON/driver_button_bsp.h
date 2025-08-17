#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

#include "driver_button_interface.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/**
 * @brief 包含STM32平台按键硬件具体信息的句柄结构体。
 */
typedef struct {
    GPIO_TypeDef* const port; /**< 按键连接的GPIO端口 */
    // const uint16_t      pin;  /**< 按键连接的GPIO引脚 */
    const GPIO_InitTypeDef init;
} bsp_button_handle_t;

/**
 * @brief 获取为STM32平台实现的按键硬件API单例。
 * @return 一个指向button_api_t结构体的常量指针。
 */
const button_api_t* bsp_button_get_api(void);

/**
 * @brief BSP层提供的中断处理函数。
 * @note  这个函数需要在真正的STM32中断服务函数 `HAL_GPIO_EXTI_Callback` 中被调用。
 * @param[in] pin - 触发中断的引脚号。
 */
void bsp_button_irq_handler(uint16_t pin);

/**
 * @brief 通过extern声明开发板上定义的按键硬件句柄。
 */
extern const bsp_button_handle_t g_bsp_key0; // 假设在 PE3
extern const bsp_button_handle_t g_bsp_key1; // 假设在 PA0

#endif // __BSP_BUTTON_H
