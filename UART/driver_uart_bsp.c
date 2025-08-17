#include "driver_uart_bsp.h"

#include <stddef.h>

// 假设我们使用USART1
extern UART_HandleTypeDef huart1;
const bsp_uart_handle_t g_bsp_usart1 = {&huart1};

// 定义接收缓冲区大小
#define RX_BUFFER_SIZE 256
static uint8_t g_rx_buffer[RX_BUFFER_SIZE];

// 定义回调函数指针
static uart_rx_callback_t g_rx_callback = NULL;

static led_status_t stm32_uart_init(void* handle, uart_rx_callback_t callback) {

    /******* 这里有问题 *****/
    const bsp_uart_handle_t* bsp_handle = (const bsp_uart_handle_t*)handle;


    if (bsp_handle == NULL || callback == NULL) {
        return LED_STATUS_INV_ARG;
    }

    // 注册回调函数
    g_rx_callback = callback;

    // 使能IDLE中断
    __HAL_UART_ENABLE_IT(bsp_handle->huart, UART_IT_IDLE);

    // 启动DMA接收
    if (HAL_UART_Receive_DMA(bsp_handle->huart, g_rx_buffer, RX_BUFFER_SIZE) != HAL_OK) {
        return LED_STATUS_ERROR;
    }

    return LED_STATUS_OK;
}

static led_status_t stm32_uart_deinit(void* handle) {
    const bsp_uart_handle_t* bsp_handle = (const bsp_uart_handle_t*)handle;
    if (bsp_handle == NULL) return LED_STATUS_INV_ARG;
    
    // 实际项目中可以调用 HAL_UART_DeInit 等
    g_rx_callback = NULL;
    return LED_STATUS_OK;
}

static led_status_t stm32_uart_transmit_dma(void* handle, const uint8_t* data, uint16_t len) {
    const bsp_uart_handle_t* bsp_handle = (const bsp_uart_handle_t*)handle;
    if (bsp_handle == NULL || data == NULL || len == 0) {
        return LED_STATUS_INV_ARG;
    }

    if (HAL_UART_Transmit_DMA(bsp_handle->huart, (uint8_t*)data, len) != HAL_OK) {
        return LED_STATUS_ERROR;
    }
    return LED_STATUS_OK;
}

// BSP层提供的中断处理函数
void bsp_uart_irq_handler(UART_HandleTypeDef *huart) {
    // 检查是否是IDLE中断
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(huart); // 清除IDLE标志位

        // 停止DMA，防止新数据覆盖缓冲区
        HAL_UART_DMAStop(huart);

        // 计算接收到的数据长度
        uint16_t len = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

        if (len > 0 && g_rx_callback != NULL) {
            // 调用上层注册的回调函数，处理数据
            g_rx_callback((void*)&g_bsp_usart1, g_rx_buffer, len);
        }

        // 重新启动DMA接收
        HAL_UART_Receive_DMA(huart, g_rx_buffer, RX_BUFFER_SIZE);
    }
}

// 填充API结构体实例
static const uart_api_t s_uart_api_stm32 = {
    .init = stm32_uart_init,
    .deinit = stm32_uart_deinit,
    .transmit_dma = stm32_uart_transmit_dma,
    .get_tick = HAL_GetTick,
};

const uart_api_t* bsp_uart_get_api(void) {
    return &s_uart_api_stm32;
}
