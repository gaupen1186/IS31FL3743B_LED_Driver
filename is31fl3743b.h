#ifndef __IS31FL3743B_H__
#define __IS31FL3743B_H__

/* !!! 注意 !!!
   下面使用 typedef struct 声明的包含位段 ( bit field ) 的部分,
   仅适用于小端( little endian )模式, 若用于大端( big endian ) 需要手动修改 !!!
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


/* IS31FL3743B IC register definitons ==========================================
*/

// Read & Write CMD
#define CMD_PG0_WRITE   0x50                          // Page 0 write cmd
#define CMD_PG1_WRITE   0x51                          // Page 1 write cmd
#define CMD_PG2_WRITE   0x52                          // Page 2 write cmd
//#define CMD_PG0_READ    (CMD_PG0_WRITE | (1 << 7))    // Page 0 read cmd
//#define CMD_PG1_READ    (CMD_PG1_WRITE | (1 << 7))    // Page 1 read cmd
//#define CMD_PG2_READ    (CMD_PG2_WRITE | (1 << 7))    // Page 2 read cmd

// Page 0 (0x50) PWM Register
#define REG_PWM_START         0x01
#define REG_PWM_STOP          0xC6

// Page 1 (0x51) SCALING Register
#define REG_SCALING_START     0x01
#define REG_SCALING_STOP      0xC6

// Page 2 (0x52) function register
#define REG_CONFIG      0x00  // Configuration Register
#define REG_GCC         0x01  // Global Current Control Register
#define REG_PDUR        0x02  // Pull Down/Up Resistor Selection Register
#define REG_TEMP        0x24  // Temperature Status
#define REG_SPSP        0x25  // Spread Spectrum Register
#define REG_RESET       0x2F  // Reset Register

// Page 2 (0x52) Reset Register (0x2F) default value
#define REG_RESET_VALUE       0xAE

// Configuration Register values
// #define

// structure for Configuration Register
typedef struct
{
  uint8_t SSD   :1;
  uint8_t OSDE  :2;
  uint8_t DUMMY :1;
  uint8_t SWS   :4;
}
reg_cfg_t;

// structure for Pull Down/Up Resistor Selection Register
typedef struct
{
  uint8_t CSPUR   :3;
  uint8_t DUMMY   :1;
  uint8_t SWPDR   :3;
  uint8_t PHC     :1;
}
reg_pdur_t;

// structure for Temperature Status
typedef struct
{
  uint8_t TROF    :2;
  uint8_t TS      :2;
  uint8_t DUMMY   :4;
}
reg_temp_t;

// structure for Spread Spectrum Register
typedef struct
{
  uint8_t CLT     :2;
  uint8_t RNG     :2;
  uint8_t SSP     :1;
  uint8_t DUMMY   :1;
  uint8_t SYNC    :2;
}
reg_spsp_t;


/* User hardware definitons ====================================================
*/

// 总行数
#define TOTAL_LED_ROW     6
// 总列数
#define TOTAL_LED_COLUMN  10
// 总像素数
#define TOTAL_LED_PIXELS    (TOTAL_LED_ROW * TOTAL_LED_COLUMN)
// RGB LED 的数量，每个像素的led 包含 rgb 三个 led
#define TOTAL_LEDS_CNT      (3 * TOTAL_LED_PIXELS)

// 预定义的字符或图形的信息结构定义
typedef struct
{
  // 占用的行数
  uint8_t     row;
  // 占用的列数
  uint8_t     column;
  // 所有 rgb led 的数量
  uint8_t     pixel_cnt;
  // 从 1 号像素开始所有需要点亮的 rgb led 的坐标，范围[1, 60]
  uint8_t     position[TOTAL_LED_PIXELS];
}
led_pixel_def_t;


/* functions declaration =======================================================
*/
void is3743b_hw_power_off(void);
void is3743b_hw_power_on(void);
//bool is3743b_soft_shutdown(void);
//bool is3743b_soft_poweron(void);
//bool is3743b_set_swx_active(uint8_t sw);
bool is3743b_enable_open_detection(void);
bool is3743b_get_open_detection_result(uint8_t *data);
bool is3743b_set_global_current_control(uint8_t gcc);
bool is3743b_set_pull_up_down(uint8_t cs_up, uint8_t sw_down, uint8_t phase);
bool is3743b_set_temperature(uint8_t ts, uint8_t trof);
bool is3743b_set_pwm(uint8_t led_index, uint8_t pwm);
bool is3743b_set_multi_pwm(uint8_t led_index, uint8_t leds_cnt, const uint8_t *pwm);
bool is3743b_set_scaling(uint8_t led_index, uint8_t scaling);
bool is3743b_set_multi_scaling(uint8_t led_index, uint8_t leds_cnt, const uint8_t *scaling);
bool is3743b_soft_reset(void);
bool is3743b_init(uint8_t ssd, uint8_t osde, uint8_t sw);


// 清屏
void led_clear_screen(void);
// LED 是否已经为清屏(黑屏)状态, return true = 黑屏
bool is_led_cleared(void);
// 在指定位置的单个led，显示指定的颜色
bool led_show_pixel_dot(uint8_t led_idx, uint32_t rgb24);
// 显示预定义的字符或图形
bool led_show_pixel_def(led_pixel_def_t *pixel_def,   // 预定义字符或图形
                        uint32_t rgb24,               // 字符图形的颜色
                        uint8_t move_x);              // 往 x 方向(右)移动的像素数量
// 设置背景色
bool led_show_backgroud_color(uint32_t rgb24);
// 周期性的调用, 更新所有像素点
bool led_internal_refresh_periodic(void);


#endif // __IS31FL3743B_H__
