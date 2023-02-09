#include "is31fl3743b.h"
#include "is31_symbol_def.h"
#include "is31_platform.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/* IS31FL3743B 的 page 0 的寄存器值
通过函数 is3743b_set_multi_pwm 周期性地写入
*/
static volatile uint8_t RAW_PWM_DATA[TOTAL_LEDS_CNT] = { 0x00 };  // 3 * 6 * 10

/* 是否需要将 RAW_PWM_DATA 写入 Page 0
调用函数 led_pixel_to_inter_raw 或 led_show_pixel_def 后必须更新为 true
写入完成后再改为 false
*/
static volatile bool  is_new_data_ready = false;


/* IC相关的寄存器级函数 ========================================================
*/

void is3743b_hw_power_off(void)
{
  is37_platform_hw_off();
}

void is3743b_hw_power_on(void)
{
  is37_platform_hw_on();
}

// power off
//bool is3743b_soft_shutdown(void)
//{
//  bool ret = true;
//  uint8_t value;
//  reg_cfg_t *p;
//  p = (reg_cfg_t *)&value;
//
//  if(is3743b_spi_read_byte(CMD_PG2_READ, REG_CONFIG, &value) != true)
//  {
//    ret = false;
//  }
//  else  // read back OK, change value, then write back
//  {
//    p->SSD = 0;
//    p->DUMMY = 1;
//    p->OSDE = 2;
//    if(is3743b_spi_write_byte(CMD_PG2_WRITE, REG_CONFIG, value) != true)
//    {
//      ret = false;
//    }
//  }
//
//  if(ret != true)
//  {
//    NRF_LOG_ERROR("[%s]: Failed! line %d\r\n",
//                  (uint32_t)__func__, __LINE__);
//  }
//
//  return ret;
//}

// power on
//bool is3743b_soft_poweron(void)
//{
//  bool ret = true;
//  uint8_t value;
//  reg_cfg_t *p;
//  p = (reg_cfg_t *)&value;
//
//  if(is3743b_spi_read_byte(CMD_PG2_READ, REG_CONFIG, &value) != true)
//  {
//    ret = false;
//  }
//  else  // read back OK, change value, then write back
//  {
//    p->SSD = 1;
//    p->DUMMY = 1;
//    if(is3743b_spi_write_byte(CMD_PG2_WRITE, REG_CONFIG, value) != true)
//    {
//      ret = false;
//    }
//  }
//
//  if(ret != true)
//  {
//    NRF_LOG_ERROR("[%s]: Failed! line %d\r\n",
//                  (uint32_t)__func__, __LINE__);
//  }
//
//  return ret;
//}

// led 开路检测
bool is3743b_enable_open_detection(void)
{
  // NEEDTODO...
  return true;
}

// 获取 led 开路检测结果
bool is3743b_get_open_detection_result(uint8_t *data)
{
  // NEEDTODO...
  return true;
}

// 全局电流控制
bool is3743b_set_global_current_control(uint8_t gcc)
{
  return is3743b_spi_write_byte(CMD_PG2_WRITE, REG_GCC, gcc);
}

// 上下拉设置
bool is3743b_set_pull_up_down(uint8_t cs_up, uint8_t sw_down, uint8_t phase)
{
  if(cs_up > 7 || sw_down > 7)
    return false;

  // NEEDTODO...
  return true;
}

// 设置高温阈值
bool is3743b_set_temperature(uint8_t ts, uint8_t trof)
{
  if(trof > 3 || ts > 3)
    return false;

  // NEEDTODO...
  return true;
}

// need todo ...
bool is3743b_set_spread_spectrum(void)
{
  return true;
}

// 设置单个 led_index 的 pwm 值
bool is3743b_set_pwm(uint8_t led_index, uint8_t pwm)
{
  if(led_index < REG_PWM_START || led_index > REG_PWM_STOP)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  return is3743b_spi_write_byte(CMD_PG0_WRITE, led_index, pwm);
}

// 设置从 led_index 开始的 leds_cnt 个连续的led 的 pwm 值
bool is3743b_set_multi_pwm(uint8_t led_index, uint8_t leds_cnt, const uint8_t *pwm)
{
  if(led_index < REG_PWM_START || led_index > REG_PWM_STOP ||
     (led_index + leds_cnt) > (REG_PWM_STOP + 1) ||
     pwm == NULL)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  return is3743b_spi_multi_write(CMD_PG0_WRITE, led_index, pwm, leds_cnt);
}

// 设置单个 led_index 的 scaling 值
bool is3743b_set_scaling(uint8_t led_index, uint8_t scaling)
{
  if(led_index < REG_SCALING_START || led_index > REG_SCALING_STOP)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  return is3743b_spi_write_byte(CMD_PG1_WRITE, led_index, scaling);
}

// 设置从 led_index 开始的 leds_cnt 个连续的led 的 scaling 值
bool is3743b_set_multi_scaling(uint8_t led_index, uint8_t leds_cnt, const uint8_t *scaling)
{
  if(led_index < REG_SCALING_START || led_index > REG_SCALING_STOP ||
     (led_index + leds_cnt) > (REG_SCALING_STOP + 1) ||
     scaling == NULL)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  return is3743b_spi_multi_write(CMD_PG1_WRITE, led_index, scaling, leds_cnt);
}

// 复位所有寄存器
bool is3743b_soft_reset(void)
{
  return is3743b_spi_write_byte(CMD_PG2_WRITE, REG_RESET, REG_RESET_VALUE);
}

/* init
   ssd: 1 = Normal operation, 0 = Software shutdown
   osde: 0 or 3 = Disable open/short detection, 1 = Enable open detection, 2 = Enable short detection
   sw: 0: SW1~SW11, 1/11, all active
       1: SW1~SW10, 1/10, SW11 no-active
       2: SW1~SW9, 1/9, SW10~SW11 no-active
       3: SW1~SW8, 1/8, SW9~SW11 no-active
       4: SW1~SW7, 1/7, SW8~SW11 no-active
       5: SW1~SW6, 1/6, SW7~SW11 no-active
       6: SW1~SW5, 1/5, SW6~SW11 no-active
       7: SW1~SW4, 1/4, SW5~SW11 no-active
       8: SW1~SW3, 1/3, SW4~SW11 no-active
       9: SW1~SW2, 1/2, SW3~SW11 no-active
       10: All CSx work as current sinks only, no scan
 */
bool is3743b_init(uint8_t ssd, uint8_t osde, uint8_t sw)
{
  if(ssd > 1 || osde > 3 || sw > 10)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  uint32_t ret = 0;
  ret = ! is3743b_platform_init();

  is3743b_delay_ms(10);
  is3743b_hw_power_on();
  is3743b_delay_ms(10);

  ret += ! is3743b_soft_reset();

  // PG2 reg 0
  uint8_t val = 0;
  reg_cfg_t *cfg_val = (reg_cfg_t *)&val;
  cfg_val->SSD = ssd;
  cfg_val->OSDE = osde;
  cfg_val->DUMMY = 0;
  cfg_val->SWS = sw;
  ret += ! is3743b_spi_write_byte(CMD_PG2_WRITE, REG_CONFIG, val);

  ret += ! is3743b_set_global_current_control(255);

  uint8_t scaling[REG_SCALING_STOP];
  memset(scaling, 255, REG_SCALING_STOP);
  ret += !is3743b_set_multi_scaling(1, REG_SCALING_STOP, scaling);

  if(ret != 0)
  {
    NRF_LOG_ERROR("[%s]: ERROR! L%u\r\n", (uint32_t)__func__,__LINE__);
    return false;
  }

  return true;
}

/* 私有函数 ====================================================================
*/

// 将指定位置的 led 像素点转换为内部寄存器使用的原始数据
static bool led_pixel_to_inter_raw(uint8_t led_pos,     // 位置
                                   uint32_t rgb24)      // 颜色
{
  // 参数检查
  if(led_pos < REG_PWM_START || led_pos > REG_PWM_STOP)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  uint8_t pos_r = (led_pos - 1) * 3 + 1;
//  uint8_t pos_g = (led_pos - 1) * 3 + 2;
//  uint8_t pos_b = (led_pos - 1) * 3 + 3;
  uint8_t pos_b = (led_pos - 1) * 3 + 2;
  uint8_t pos_g = (led_pos - 1) * 3 + 3;

  uint8_t r = (rgb24 >> 16) & 0xff;
  uint8_t g = (rgb24 >> 8) & 0xff;
  uint8_t b = rgb24 & 0xff;

  if(RAW_PWM_DATA[pos_r - 1] != r)
  {
    RAW_PWM_DATA[pos_r - 1] = r;
    is_new_data_ready = true;
  }
  if(RAW_PWM_DATA[pos_g - 1] != g)
  {
    RAW_PWM_DATA[pos_g - 1] = g;
    is_new_data_ready = true;
  }
  if(RAW_PWM_DATA[pos_b - 1] != b)
  {
    RAW_PWM_DATA[pos_b - 1] = b;
    is_new_data_ready = true;
  }

  return true;
}

/* 公共函数 ====================================================================
*/

// 清屏(整个屏幕关闭变黑)
void led_clear_screen(void)
{
  uint8_t i;
  // 若缓存不全为 0 则全部清零并标记更新
  for(i = 0; i < sizeof(RAW_PWM_DATA); i++)
  {
    if(RAW_PWM_DATA[i] != 0x00)
    {
      memset((void *)RAW_PWM_DATA, 0x00, sizeof(RAW_PWM_DATA));
      is_new_data_ready = true;
//      NRF_LOG_INFO("[%s]: ok\r\n", (uint32_t)__func__);
      return;
    }
  }
}

// LED 是否已经为清屏(黑屏)状态, return true = 黑屏
bool is_led_cleared(void)
{
  // 有数据尚未刷新给 IC
  if(is_new_data_ready == true)
    return false;

  uint8_t index;
  // 有数据不为 0
  for(index = 0; index < TOTAL_LEDS_CNT; index ++)
  {
    if(RAW_PWM_DATA[index] != 0x00)
      return false;
  }

  // is_new_data_ready == false 并且 RAW_PWM_DATA[TOTAL_LEDS_CNT] == { 0x00 } 则表示 LED 完全黑屏
  return true;
}

// 在指定位置的单个led，显示指定的颜色
bool led_show_pixel_dot(uint8_t led_idx, uint32_t rgb24)
{
  return led_pixel_to_inter_raw(led_idx, rgb24);
}

// 设置背景色
bool led_show_backgroud_color(uint32_t rgb24)
{
  uint8_t i;
  // 将所有的点转换为 独立的 rgb 坐标并保存
  for(i = 1; i <= TOTAL_LED_PIXELS; i++)
  {
    if(led_pixel_to_inter_raw(i, rgb24) != true)
    {
      NRF_LOG_ERROR("[%s]: Failed! line %d\r\n",
                    (uint32_t)__func__, __LINE__);
      return false;
    }
  }

//  NRF_LOG_INFO("[%s]: 0x%06x\r\n", (uint32_t)__func__, rgb24);
  return true;
}

// 显示预定义的字符或图形
bool led_show_pixel_def(led_pixel_def_t *pixel_def,   // 预定义字符或图形
                        uint32_t rgb24,               // 字符图形的颜色
                        uint8_t move_x)               // 往 x 方向(右)移动的像素数量
{
  // 参数检查
  if(pixel_def == NULL)
  {
    NRF_LOG_ERROR("[%s]: PARAM INVALID! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  if(rgb24 == 0x000000)
  {
    led_clear_screen();
    return true;
  }

  uint8_t i;
  // 所有点往 x 方向右移
  if(move_x > 0)
  {
    uint8_t cnt = pixel_def->pixel_cnt;
    // 循环移动所有点
    for(i = 0; i < pixel_def->pixel_cnt; i++)
    {
      pixel_def->position[i] += move_x * TOTAL_LED_ROW; // 右移
      // 若右移后超过了最右边的一列，则清除此点
      if(pixel_def->position[i] > TOTAL_LED_PIXELS)
      {
        pixel_def->position[i] = 0;
        cnt --;   // 更新有效的点个数
      }
    }
    pixel_def->pixel_cnt = cnt;
  }

  // 将所有的点转换为 独立的 rgb 坐标并保存
  for(i = 0; i < pixel_def->pixel_cnt; i++)
  {
    if(led_pixel_to_inter_raw(pixel_def->position[i], rgb24) != true)
    {
      NRF_LOG_ERROR("[%s]: Failed! line %d\r\n",
                    (uint32_t)__func__, __LINE__);
      return false;
    }
  }

  return true;
}

// 更新所有像素点
bool led_internal_refresh_periodic(void)
{
  if(is_new_data_ready == true)
  {
    is3743b_hw_power_on();
    if(is3743b_set_multi_pwm(1, TOTAL_LEDS_CNT, (uint8_t *)RAW_PWM_DATA) == true)
    {
//      NRF_LOG_HEXDUMP_WARNING((const void *)RAW_PWM_DATA, 6); NRF_LOG_INFO("\r\n\r\n");
      is_new_data_ready = false;
      return true;
    }
    // else
    NRF_LOG_ERROR("[%s]: ERROR! L%u\r\n", (uint32_t)__func__,__LINE__);
    return false;
  }

  return true;
}
