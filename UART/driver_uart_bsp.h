#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "driver_uart_interface.h"
#include "stm32f4xx_hal.h"

/**
 * @brief 包含STM32平台串口硬件具体信息的句柄结构体。
 */
typedef struct {
    UART_HandleTypeDef* const huart; /**< 指向HAL库UART句柄的指针 */

} bsp_uart_handle_t;

/**
 * @brief 获取为STM32平台实现的串口硬件API单例。
 * @return 一个指向uart_api_t结构体的常量指针。
 */
const uart_api_t* bsp_uart_get_api(void);

/**
 * @brief BSP层提供的中断处理函数。
 * @note  这个函数需要在真正的STM32中断服务函数 `USART1_IRQHandler` 中被调用。
 * @param[in] huart - 触发中断的UART句柄。
 */
void bsp_uart_irq_handler(UART_HandleTypeDef *huart);

/**
 * @brief 通过extern声明开发板上定义的串口硬件句柄。
 */
extern const bsp_uart_handle_t g_bsp_usart1;

#endif // __BSP_UART_H
