#include "driver_uart_test.h"

// 1. 定义驱动对象和其所需的接收缓冲区
uart_t g_uart1; // 定义一个全局的驱动对象，以匹配驱动层中的extern声明
uint8_t g_uart1_rx_buffer[256]; // 为其分配一个256字节的环形缓冲区

#include <string.h>

void driver_uart_test(void) {

    /* 驱动初始化 -------------------------------------------------------------*/
    // a. 从BSP层获取为本平台实现的API实例
    const uart_api_t* uart_api = bsp_uart_get_api();

    // b. 使用获取到的API实例和硬件句柄，来初始化通用的驱动对象
    uart_init(&g_uart1, uart_api, (void*) &g_bsp_usart1, g_uart1_rx_buffer, sizeof(g_uart1_rx_buffer));

    /* 应用逻辑 -----------------------------------------------------------------*/
    // 发送一条欢迎信息
    char* welcome_msg = "UART Driver Initialized. Start Echo Test...\r\n";
    uart_write(&g_uart1, (uint8_t*) welcome_msg, strlen(welcome_msg));

    // 定义一个用于临时存放从环形缓冲区读出数据的缓冲区
    uint8_t temp_buffer[64];

    while (1) {
        // 检查环形缓冲区中是否有可读的数据
        uint16_t bytes_available = uart_get_bytes_available(&g_uart1);

        if (bytes_available > 0) {
            // 从驱动的环形缓冲区中读取数据到临时缓冲区
            uint16_t bytes_read = uart_read(&g_uart1, temp_buffer, sizeof(temp_buffer));

            if (bytes_read > 0) {
                // 回环测试：将读取到的数据再原样发送回去
                uart_write(&g_uart1, temp_buffer, bytes_read);
            }
        }

    }
}


