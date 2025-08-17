#include "driver_i2c_test.h"

#include <stdio.h>          // 用于printf
#include <string.h>         // 用于memcmp和strlen

/* Private variables ---------------------------------------------------------*/
// 定义I2C驱动对象
i2c_t g_i2c1;
extern UART_HandleTypeDef* huart1;


// 定义EEPROM设备地址 (AT24C02/AT24C04... 的地址通常是0x50)
#define EEPROM_DEVICE_ADDRESS 0x50

// printf重定向到UART1的实现
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0xFFFF);
  return ch;
}


void driver_i2c_test(void) {

    printf("\r\n--- I2C EEPROM Test Program ---\r\n");

    /* 驱动初始化 -------------------------------------------------------------*/
    const i2c_api_t* i2c_api = bsp_i2c_get_api();
    i2c_init(&g_i2c1, i2c_api, (void*) &g_bsp_i2c1);

    /* 应用逻辑：EEPROM读写测试 ------------------------------------------------*/
    // 1. 检查设备是否就绪
    printf("Checking for EEPROM at address 0x%02X... ", EEPROM_DEVICE_ADDRESS);
    if (i2c_is_device_ready(&g_i2c1, EEPROM_DEVICE_ADDRESS, 100) == LED_STATUS_OK) {
        printf("Device found!\r\n");
    } else {
        printf("Device not found! Halting.\r\n");
        while (1);
    }

    // 2. 准备要写入的数据
    uint8_t write_data[] = "Hello, I2C!";
    uint16_t data_len = strlen((char*) write_data) + 1; // +1 for null terminator
    uint8_t read_data[sizeof(write_data)] = { 0 };
    uint16_t mem_address = 0x10; // 写入到EEPROM的地址0x10

    // 3. 写入数据到EEPROM
    printf("Writing %d bytes to memory address 0x%04X...\r\n", data_len, mem_address);
    if (i2c_mem_write(&g_i2c1, EEPROM_DEVICE_ADDRESS, mem_address, I2C_MEM_ADDR_SIZE_8BIT, write_data, data_len) == LED_STATUS_OK) {
        printf("Write successful.\r\n");
    } else {
        printf("Write failed! Halting.\r\n");
        while (1);
    }

    // 4. 等待EEPROM内部写操作完成 (AT24C02典型值为5ms)
    HAL_Delay(10);

    // 5. 从EEPROM同一地址读出数据
    printf("Reading %d bytes from memory address 0x%04X...\r\n", data_len, mem_address);
    if (i2c_mem_read(&g_i2c1, EEPROM_DEVICE_ADDRESS, mem_address, I2C_MEM_ADDR_SIZE_8BIT, read_data, data_len) == LED_STATUS_OK) {
        printf("Read successful. Data: \"%s\"\r\n", (char*) read_data);
    } else {
        printf("Read failed! Halting.\r\n");
        while (1);
    }

    // 6. 比较写入和读出的数据是否一致
    printf("Verifying data... ");
    if (memcmp(write_data, read_data, data_len) == 0) {
        printf("Verification successful!\r\n");
    } else {
        printf("Verification failed!\r\n");
    }

    printf("--- Test Finished ---\r\n");

    while (1)
    {
        
    }
    

}




