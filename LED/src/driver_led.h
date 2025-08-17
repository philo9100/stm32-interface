#ifndef __DRIVER_LED_H
#define __DRIVER_LED_H

#include "driver_led_interface.h" // 依赖于抽象接口

/**
 * @brief 定义LED的工作模式 (状态机的状态)
 */
typedef enum {
    LED_MODE_OFF,         /**< 模式: 始终熄灭 */
    LED_MODE_ON,          /**< 模式: 始终点亮 */
    LED_MODE_BLINK,       /**< 模式: 持续闪烁 */
    LED_MODE_PULSE_ONCE,  /**< 模式: 单次脉冲 (点亮指定时长后自动熄灭) */
} led_mode_t;

/**
 * @brief LED驱动的 "对象" 或 "类" 定义
 */
typedef struct led_s {
    const led_api_t* api;    /**< 指向平台依赖API函数表的指针 */
    void* handle;            /**< 指向具体硬件实例句柄的void指针 */
    led_mode_t mode;         /**< 当前的工作模式 (状态机的当前状态) */

    /**
     * @brief 使用联合体(union)来节省内存，因为不同模式的参数是互斥的
     */
    union {
        struct {
            uint32_t on_time_ms;
            uint32_t off_time_ms;
        } blink;
        struct {
            uint32_t duration_ms;
        } pulse;
    } params;

    uint32_t last_event_time; /**< 上次事件(如状态翻转)发生的时间戳 */
    uint8_t  is_on;           /**< 标记LED当前物理状态是亮(1)还是灭(0) */
} led_t;


// ===================================================================================
// 公共API函数
// ===================================================================================

/**
 * @brief  初始化一个LED对象
 * @param[in] led    - 指向要初始化的led_t对象的指针
 * @param[in] api    - 指向底层硬件API函数表的指针
 * @param[in] handle - 指向具体硬件实例句柄的void指针
 * @return led_status_t - 操作的状态码
 */
led_status_t led_init(led_t* led, const led_api_t* api, void* handle);

/**
 * @brief  反初始化一个LED对象，释放资源
 * @param[in] led - 指向led_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t led_deinit(led_t* led);

/**
 * @brief  设置LED为常亮模式
 * @param[in] led - 指向led_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t led_set_mode_on(led_t* led);

/**
 * @brief  设置LED为常灭模式
 * @param[in] led - 指向led_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t led_set_mode_off(led_t* led);

/**
 * @brief  设置LED为闪烁模式
 * @param[in] led         - 指向led_t对象的指针
 * @param[in] on_time_ms  - 点亮持续时间 (毫秒)
 * @param[in] off_time_ms - 熄灭持续时间 (毫秒)
 * @return led_status_t - 操作的状态码
 */
led_status_t led_set_mode_blink(led_t* led, uint32_t on_time_ms, uint32_t off_time_ms);

/**
 * @brief  触发一个单次脉冲
 * @param[in] led         - 指向led_t对象的指针
 * @param[in] duration_ms - 脉冲点亮的持续时间 (毫秒)
 * @return led_status_t - 操作的状态码
 */
led_status_t led_trigger_pulse_once(led_t* led, uint32_t duration_ms);

/**
 * @brief  LED状态处理函数 (状态机的核心)
 * @note   此函数必须在主循环或定时器中被周期性地调用，以驱动LED的状态变化。
 * @param[in] led - 指向led_t对象的指针
 * @return led_status_t - 操作的状态码
 */
led_status_t led_process(led_t* led);

#endif // __DRIVER_LED_H
