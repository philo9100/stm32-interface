#include "driver_uart.h"

#include <string.h> // For memcpy

/**
 * @brief 内部中断回调函数
 * @note  这个函数是传递给BSP层的，当硬件通过DMA+IDLE接收到数据时，它被调用。
 * 它的职责是将接收到的数据块安全地写入环形缓冲区。
 */
static void internal_rx_callback(void* handle, uint8_t* data, uint16_t len) {
    // 在BSP的实现中，handle总是指向bsp_uart_handle_t，但我们的驱动对象需要它
    // 为了解耦，我们这里假设只有一个uart实例，或者需要一个更复杂的机制来查找uart对象
    // 在测试层，我们会确保只有一个实例。
    extern uart_t g_uart1; // 假设在测试层有一个全局的uart对象
    uart_t* uart = &g_uart1;

    if (uart == NULL || data == NULL || len == 0) {
        return;

    }

    // 将数据写入环形缓冲区
    for (uint16_t i = 0; i < len; ++i) {
        uint16_t next_head = (uart->head + 1) % uart->rx_buffer_size;
        // 检查缓冲区是否已满 (写指针追上读指针)
        if (next_head == uart->tail) {
            // 缓冲区已满，丢弃后续数据。在实际产品中，这里可以设置一个溢出标志位。
            break;
        }
        uart->rx_buffer[uart->head] = data[i];
        uart->head = next_head;
    }
}

led_status_t uart_init(uart_t* uart, const uart_api_t* api, void* handle, uint8_t* rx_buffer, uint16_t rx_buffer_size) {
    if (uart == NULL || api == NULL || handle == NULL || rx_buffer == NULL || rx_buffer_size == 0) {
        return LED_STATUS_INV_ARG;
    }
    if (api->init == NULL || api->transmit_dma == NULL || api->get_tick == NULL) {
        return LED_STATUS_INV_ARG;
    }

    // 初始化对象成员
    uart->api = api;
    
    uart->handle = handle;
    uart->rx_buffer = rx_buffer;
    uart->rx_buffer_size = rx_buffer_size;
    uart->head = 0;
    uart->tail = 0;

    // 调用底层API初始化硬件，并将内部回调函数注册进去
    return uart->api->init(uart->handle, internal_rx_callback);
}

led_status_t uart_deinit(uart_t* uart) {
    if (uart == NULL || uart->api == NULL || uart->api->deinit == NULL) {
        return LED_STATUS_INV_ARG;
    }
    return uart->api->deinit(uart->handle);
}

uint16_t uart_read(uart_t* uart, uint8_t* data, uint16_t len) {
    if (uart == NULL || data == NULL || len == 0) {
        return 0;
    }

    uint16_t bytes_to_read = 0;
    for (bytes_to_read = 0; bytes_to_read < len; ++bytes_to_read) {
        // 检查缓冲区是否为空 (读指针追上写指针)
        if (uart->tail == uart->head) {
            break;
        }
        data[bytes_to_read] = uart->rx_buffer[uart->tail];
        uart->tail = (uart->tail + 1) % uart->rx_buffer_size;
    }
    return bytes_to_read;
}

led_status_t uart_write(uart_t* uart, const uint8_t* data, uint16_t len) {
    if (uart == NULL || uart->api == NULL || uart->api->transmit_dma == NULL) {
        return LED_STATUS_INV_ARG;
    }
    return uart->api->transmit_dma(uart->handle, data, len);
}

uint16_t uart_get_bytes_available(uart_t* uart) {
    if (uart == NULL) {
        return 0;
    }
    if (uart->head >= uart->tail) {
        return uart->head - uart->tail;
    } else {
        return uart->rx_buffer_size - uart->tail + uart->head;
    }
}
