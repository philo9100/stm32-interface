#include "driver_i2c.h"

led_status_t i2c_init(i2c_t* i2c, const i2c_api_t* api, void* handle) {
    // 防御性编程: 检查所有指针是否有效
    if (i2c == NULL || api == NULL || handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 检查必要的API函数是否已实现
    if (api->init == NULL || api->mem_write_dma == NULL || api->mem_read_dma == NULL) {
        return LED_STATUS_INV_ARG;
    }

    i2c->api = api;
    i2c->handle = handle;

    // 调用底层API初始化硬件
    return i2c->api->init(i2c->handle);
}

led_status_t i2c_deinit(i2c_t* i2c) {
    if (i2c == NULL || i2c->api == NULL || i2c->api->deinit == NULL) {
        return LED_STATUS_INV_ARG;
    }
    return i2c->api->deinit(i2c->handle);
}

led_status_t i2c_mem_write(i2c_t* i2c, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, const uint8_t* data, uint16_t len) {
    if (i2c == NULL || i2c->api == NULL || i2c->api->mem_write_dma == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 将调用请求转发给底层的具体实现
    return i2c->api->mem_write_dma(i2c->handle, dev_address, mem_address, mem_addr_size, data, len);
}

led_status_t i2c_mem_read(i2c_t* i2c, uint16_t dev_address, uint16_t mem_address, i2c_mem_addr_size_t mem_addr_size, uint8_t* data, uint16_t len) {
    if (i2c == NULL || i2c->api == NULL || i2c->api->mem_read_dma == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 将调用请求转发给底层的具体实现
    return i2c->api->mem_read_dma(i2c->handle, dev_address, mem_address, mem_addr_size, data, len);
}

led_status_t i2c_is_device_ready(i2c_t* i2c, uint16_t dev_address, uint32_t timeout_ms) {
    if (i2c == NULL || i2c->api == NULL || i2c->api->is_device_ready == NULL) {
        return LED_STATUS_INV_ARG;
    }
    // 将调用请求转发给底层的具体实现
    return i2c->api->is_device_ready(i2c->handle, dev_address, timeout_ms);
}
