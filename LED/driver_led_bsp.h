#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "driver_led_interface.h" // 包含我们刚刚完成的极致优化接口
#include "stm32f4xx_hal.h"       // 包含平台相关的HAL库头文件
#include "main.h"


#define Active_Low        0

/**
 * @brief  包含STM32平台LED硬件具体信息的句柄结构体。
 * @note   这是对上层驱动隐藏的具体实现细节。
 */
typedef struct {
    GPIO_TypeDef* const port;         /**< LED连接的GPIO端口 */
    const uint16_t      pin;          /**< LED连接的GPIO引脚 */
    const uint8_t       active_level; /**< 点亮LED的有效电平 (1: 高电平, 0: 低电平) */
} bsp_led_handle_t;


/**
 * @brief  获取为STM32平台实现的LED硬件API单例。
 * @note   应用层通过此函数获取底层的具体实现，并将其传递给上层驱动。
 * @retval 一个指向包含硬件操作函数地址的led_api_t结构体的常量指针。
 */
const led_api_t* bsp_led_get_api(void);

/**
 * @brief  通过extern声明开发板上定义的LED硬件句柄。
 * @note   这是将硬件资源暴露给应用层(main.c)的清晰方式。
 */
extern const bsp_led_handle_t g_bsp_led1;
extern const bsp_led_handle_t g_bsp_led2;

#endif // __BSP_LED_H