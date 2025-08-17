#include "driver_led_bsp.h"

// ===================================================================================
// 1. 定义您开发板上所有LED的硬件句柄实体
//    根据您的描述，LED为低电平有效 (active_level = 0)。
// ===================================================================================
const bsp_led_handle_t g_bsp_led1 = { LED1_GPIO_Port, LED1_Pin, Active_Low }; // PF9, Active-Low
const bsp_led_handle_t g_bsp_led2 = { LED2_GPIO_Port, LED2_Pin, Active_Low }; // PF10, Active-Low


// ===================================================================================
// 2. 实现接口所要求的、与硬件相关的私有函数 (Static Functions)
// ===================================================================================

/**
 * @brief 使能指定GPIO端口的时钟。这是一个内部辅助函数。
 */
static void enable_gpio_clock(GPIO_TypeDef* port) {
    if (port == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
    // 可在此处为其他端口添加 "else if"
}

/**
 * @brief 针对STM32平台的LED硬件初始化函数。
 */
static led_status_t stm32_led_init(void* handle) {
    const bsp_led_handle_t* bsp_handle = (const bsp_led_handle_t*) handle;
    if (bsp_handle == NULL) {
        return LED_STATUS_INV_ARG; // 返回明确的错误码
    }

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    enable_gpio_clock(bsp_handle->port);

    GPIO_InitStruct.Pin = bsp_handle->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(bsp_handle->port, &GPIO_InitStruct);

    // 初始化后默认关闭LED (对于低电平有效，即输出高电平)
    HAL_GPIO_WritePin(bsp_handle->port, bsp_handle->pin, GPIO_PIN_SET);

    return LED_STATUS_OK;
}

/**
 * @brief 针对STM32平台的LED硬件反初始化函数。
 */
static led_status_t stm32_led_deinit(void* handle) {
    const bsp_led_handle_t* bsp_handle = (const bsp_led_handle_t*) handle;
    if (bsp_handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    HAL_GPIO_DeInit(bsp_handle->port, bsp_handle->pin);
    return LED_STATUS_OK;
}

/**
 * @brief 针对STM32平台的LED状态设置函数。
 */
static led_status_t stm32_led_set_state(void* handle, uint8_t state) {
    const bsp_led_handle_t* bsp_handle = (const bsp_led_handle_t*) handle;
    if (bsp_handle == NULL) {
        return LED_STATUS_INV_ARG;
    }

    GPIO_PinState pin_state;
    if (state == 1) { // 期望点亮 (ON)
        pin_state = (bsp_handle->active_level == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    } else { // 期望熄灭 (OFF)
        pin_state = (bsp_handle->active_level == 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
    HAL_GPIO_WritePin(bsp_handle->port, bsp_handle->pin, pin_state);
    return LED_STATUS_OK;
}

/**
 * @brief 针对STM32平台的LED状态翻转函数。
 */
static led_status_t stm32_led_toggle(void* handle) {
    const bsp_led_handle_t* bsp_handle = (const bsp_led_handle_t*) handle;
    if (bsp_handle == NULL) {
        return LED_STATUS_INV_ARG;
    }
    HAL_GPIO_TogglePin(bsp_handle->port, bsp_handle->pin);
    return LED_STATUS_OK;
}

/**
 * @brief 针对STM32平台的亮度设置函数 (不支持)。
 * @note  这是一个优雅处理不支持功能的好例子。它不返回通用错误，
 * 而是返回一个明确的“不支持”状态码。
 */
static led_status_t stm32_led_set_brightness(void* handle, uint8_t brightness_percent) {
    // 参数未使用，避免编译器警告
    (void) handle;
    (void) brightness_percent;

    return LED_STATUS_NOT_SUPPORTED;
}

// ===================================================================================
// 3. 创建并填充统一的API结构体实例
// ===================================================================================
static const led_api_t s_led_api_stm32 = {
    .init = stm32_led_init,
    .deinit = stm32_led_deinit,
    .set_state = stm32_led_set_state,
    .toggle = stm32_led_toggle,
    .set_brightness = stm32_led_set_brightness, // 链接到明确返回不支持的函数
    .get_tick = HAL_GetTick,              // 直接链接HAL库的函数
};


// ===================================================================================
// 4. 实现获取API实例的公共函数
// ===================================================================================
const led_api_t* bsp_led_get_api(void) {
    return &s_led_api_stm32;
}