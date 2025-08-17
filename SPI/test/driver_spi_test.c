#include "driver_spi_test.h"
#include <stdio.h>


/* Private variables ---------------------------------------------------------*/
// 定义SPI驱动对象
spi_t g_spi1;
extern const UART_HandleTypeDef huart1; // 假设UART1用于printf重定向


// W25Q系列Flash JEDEC ID命令
#define W25Q_CMD_JEDEC_ID 0x9F


// 为W25Q芯片封装一个读取JEDEC ID的函数
led_status_t w25q_read_jedec_id(spi_t* spi, uint8_t* manufacturer_id, uint16_t* device_id);

// printf重定向到UART1的实现
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


void driver_spi_test(void);
{
    HAL_Init();
    SystemClock_Config();
    // 注意: CubeMX生成的MX_GPIO_Init(), MX_SPI1_Init(), MX_DMA_Init(), MX_USART1_UART_Init()
    // 等函数应在此处被调用。

    printf("\r\n--- SPI W25Q Flash Test Program ---\r\n");

    /* 驱动初始化 -------------------------------------------------------------*/
    const spi_api_t* spi_api = bsp_spi_get_api();
    spi_init(&g_spi1, spi_api, (void*) &g_bsp_spi1);

    /* 应用逻辑：读取Flash JEDEC ID -----------------------------------------*/
    uint8_t manufacturer_id = 0;
    uint16_t device_id = 0;

    printf("Reading JEDEC ID from Flash...\r\n");
    if (w25q_read_jedec_id(&g_spi1, &manufacturer_id, &device_id) == LED_STATUS_OK) {
        printf("Read successful!\r\n");
        printf(" - Manufacturer ID: 0x%02X\r\n", manufacturer_id);
        printf(" - Device ID: 0x%04X\r\n", device_id);

        // W25Q128的制造商ID是0xEF, 设备ID是0x4018
        if (manufacturer_id == 0xEF) {
            printf("   (Winbond, Correct!)\r\n");
        }
    } else {
        printf("Read failed!\r\n");
    }

    printf("--- Test Finished ---\r\n");

    while (1) {
        // 测试完成，主循环空闲
    }
}




/**
 * @brief  读取W25Q系列Flash的JEDEC ID
 * @note   这是一个更高层次的、针对具体设备的驱动函数。
 * @param[in]  spi             - 指向已初始化的spi_t对象的指针
 * @param[out] manufacturer_id - 用于存放制造商ID的指针
 * @param[out] device_id       - 用于存放设备ID的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t w25q_read_jedec_id(spi_t* spi, uint8_t* manufacturer_id, uint16_t* device_id) {
    // JEDEC ID命令需要发送1个字节的命令(0x9F)，然后接收3个字节的数据。
    // 所以总共需要进行4个字节的SPI事务。
    uint8_t tx_buffer[4] = { W25Q_CMD_JEDEC_ID, 0xFF, 0xFF, 0xFF };
    uint8_t rx_buffer[4] = { 0 };

    // 调用我们通用的SPI驱动层函数来执行事务
    led_status_t status = spi_transceive(spi, tx_buffer, rx_buffer, 4);

    if (status == LED_STATUS_OK) {
        // 如果通信成功，解析接收到的数据
        // rx_buffer[0]是无用数据 (在发送命令时接收到的)
        *manufacturer_id = rx_buffer[1];
        *device_id = (rx_buffer[2] << 8) | rx_buffer[3];
    }

    return status;
}


