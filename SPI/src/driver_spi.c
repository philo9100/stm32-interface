#include "driver_spi.h"

#include "driver_spi.h"
#include <string.h> // For memset

// 定义用于便利性函数的内部静态缓冲区大小
// 注意：这限制了spi_write和spi_read单次操作的最大长度。
// 对于需要更大长度的操作，应直接使用spi_transceive函数。
#define SPI_DUMMY_BUFFER_SIZE 256

led_status_t spi_init(spi_t* spi, const spi_api_t* api, void* handle) {
    if (spi == NULL || api == NULL || handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 检查所有必要的API函数是否都已实现
    if (api->init == NULL || api->transceive_dma == NULL || 
        api->chip_select == NULL || api->chip_deselect == NULL) {
        return LED_STATUS_INV_ARG;
    }

    spi->api = api;
    spi->handle = handle;

    return spi->api->init(spi->handle);
}

led_status_t spi_deinit(spi_t* spi) {
    if (spi == NULL || spi->api == NULL || spi->api->deinit == NULL) {
        return LED_STATUS_INV_ARG;
    }
    return spi->api->deinit(spi->handle);
}

led_status_t spi_transceive(spi_t* spi, const uint8_t* tx_data, uint8_t* rx_data, uint16_t len) {
    if (spi == NULL || spi->api == NULL) {
        return LED_STATUS_INV_ARG;
    }
    if (tx_data == NULL || rx_data == NULL || len == 0) {
        return LED_STATUS_INV_ARG;
    }
    
    led_status_t status = LED_STATUS_OK;

    // 1. 片选使能 (CS拉低)
    status = spi->api->chip_select(spi->handle);
    if (status != LED_STATUS_OK) {
        return status;
    }

    // 2. 调用底层API进行数据交换
    status = spi->api->transceive_dma(spi->handle, tx_data, rx_data, len);

    // 3. 片选禁止 (CS拉高)
    //    这是一个关键步骤：无论数据交换是否成功，都应尝试禁止片选，以释放总线。
    spi->api->chip_deselect(spi->handle);

    return status;
}

led_status_t spi_write(spi_t* spi, const uint8_t* tx_data, uint16_t len) {
    if (spi == NULL || tx_data == NULL) {
        return LED_STATUS_INV_ARG;
    }
    if (len > SPI_DUMMY_BUFFER_SIZE) {
        // 如果需要写入的数据超过了内部缓冲区大小，返回错误
        // 调用者应使用spi_transceive并自己管理接收缓冲区
        return LED_STATUS_INV_ARG;
    }

    // 对于只写操作，接收到的数据是无用的，但DMA仍需要一个有效的缓冲区来写入
    static uint8_t dummy_rx_buffer[SPI_DUMMY_BUFFER_SIZE];
    return spi_transceive(spi, tx_data, dummy_rx_buffer, len);
}

led_status_t spi_read(spi_t* spi, uint8_t* rx_data, uint16_t len) {
    if (spi == NULL || rx_data == NULL) {
        return LED_STATUS_INV_ARG;
    }
    if (len > SPI_DUMMY_BUFFER_SIZE) {
        return LED_STATUS_INV_ARG;
    }

    // 对于只读操作，我们必须向总线发送一些虚拟数据 (通常是0xFF)
    static uint8_t dummy_tx_buffer[SPI_DUMMY_BUFFER_SIZE];
    static uint8_t is_inited = 0;

    // 仅在第一次调用时初始化虚拟发送缓冲区，提高效率
    if (!is_inited) {
        memset(dummy_tx_buffer, 0xFF, SPI_DUMMY_BUFFER_SIZE);
        is_inited = 1;
    }
    
    return spi_transceive(spi, dummy_tx_buffer, rx_data, len);
}
