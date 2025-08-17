#include "driver_button_bsp.h"

#include <stddef.h>

// 假设开发板上的按键连接情况
const bsp_button_handle_t g_bsp_key0 = {KEY0_GPIO_Port, .init = {
    .Pin = KEY0_Pin,
    .Mode = GPIO_MODE_IT_RISING,
    .Pull = GPIO_PULLDOWN,
}};

const bsp_button_handle_t g_bsp_key1 = {KEY1_GPIO_Port, .init = {
    .Pin = KEY1_Pin,
    .Mode = GPIO_MODE_IT_RISING,
    .Pull = GPIO_PULLDOWN,
}};

// 中断回调函数注册表
// 为了简单高效，我们使用一个固定大小的数组来映射引脚和回调函数
// 这里我们假设最多有16个中断引脚 (0-15)
#define MAX_EXTI_CALLBACKS 16
static button_irq_callback_t g_irq_callbacks[MAX_EXTI_CALLBACKS] = {NULL};
static void* g_irq_handles[MAX_EXTI_CALLBACKS] = {NULL};

// 内部辅助函数，用于根据引脚号获取其索引 (0-15)
static int8_t get_pin_index(uint16_t pin) {
    for (int8_t i = 0; i < 16; i++) {
        if (pin == (1U << i)) {
            return i;
        }
    }
    return -1;
}


#include "driver_button.h"

static led_status_t stm32_button_init(void* handle, button_irq_callback_t callback) {


    /*******  这里有问题  *******/
    const bsp_button_handle_t* bsp_handle = (const bsp_button_handle_t*)handle;
    // const button_t* handle_t = (const button_t*)handle;
    // const bsp_button_handle_t* bsp_handle = (const bsp_button_handle_t*)(handle_t->handle);


    if (bsp_handle == NULL || callback == NULL) {
        return LED_STATUS_INV_ARG;
    }

    // 注册回调函数
    int8_t index = get_pin_index(bsp_handle->init.Pin);
    if (index == -1) {
        return LED_STATUS_ERROR;
    }
    g_irq_callbacks[index] = callback;
    g_irq_handles[index] = handle;

    // 配置GPIO为中断模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (bsp_handle->port == KEY0_GPIO_Port) __HAL_RCC_GPIOE_CLK_ENABLE();
    if (bsp_handle->port == KEY1_GPIO_Port) __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 按键通常是下降沿触发 (按下时电平从高到低)
    GPIO_InitStruct.Mode = bsp_handle->init.Mode; 
    GPIO_InitStruct.Pull = bsp_handle->init.Pull; // 启用内部上拉，如果外部没有的话
    GPIO_InitStruct.Pin = bsp_handle->init.Pin;
    HAL_GPIO_Init(bsp_handle->port, &GPIO_InitStruct);

    // 使能中断
    IRQn_Type irq_num;
    if (index >= 5 && index <= 9) {
        irq_num = EXTI9_5_IRQn;
    } else if (index >= 10 && index <= 15) {
        irq_num = EXTI15_10_IRQn;
    } else {
        // 简化处理，可根据具体引脚添加 EXTI0_IRQn, EXTI1_IRQn...
        // irq_num = EXTI0_IRQn; 
        switch (index)
        {
        case 0:
            irq_num = EXTI0_IRQn;
            break;
        case 1:
            irq_num = EXTI1_IRQn;
            break;
        case 2:
            irq_num = EXTI2_IRQn;
            break;
        case 3:
            irq_num = EXTI3_IRQn;
            break;
        case 4:
            irq_num = EXTI4_IRQn;
            break;
        default:
            break;
        }
    }
    HAL_NVIC_SetPriority(irq_num, 15, 0);
    HAL_NVIC_EnableIRQ(irq_num);

    return LED_STATUS_OK;
}

static led_status_t stm32_button_deinit(void* handle) {
    const bsp_button_handle_t* bsp_handle = (const bsp_button_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;

    int8_t index = get_pin_index(bsp_handle->init.Pin);
    if (index != -1) {
        g_irq_callbacks[index] = NULL;
        g_irq_handles[index] = NULL;
    }

    HAL_GPIO_DeInit(bsp_handle->port, bsp_handle->init.Pin);
    return LED_STATUS_OK;
}

// BSP层提供的中断处理函数，它会分发中断到对应的回调
void bsp_button_irq_handler(uint16_t pin) {
    int8_t index = get_pin_index(pin);
    if (index != -1 && g_irq_callbacks[index] != NULL) {
        // 调用上层注册的回调函数
        g_irq_callbacks[index](g_irq_handles[index]);
    }
}

// 填充API结构体实例
static const button_api_t s_button_api_stm32 = {
    .init = stm32_button_init,
    .deinit = stm32_button_deinit,
    .get_tick = HAL_GetTick,
};

const button_api_t* bsp_button_get_api(void) {
    return &s_button_api_stm32;
}
