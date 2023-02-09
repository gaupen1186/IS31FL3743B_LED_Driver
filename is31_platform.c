#include "is31_platform.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


// 等待 SPI 空闲的最大时间
#define LED_SPI_WAIT_FREE_MS_MAX     100


void is37_platform_hw_off(void)
{
  bsp_board_led_powerdown();
}

void is37_platform_hw_on(void)
{
  bsp_board_led_poweron();
}

/* SPI 接口
============================================================================*/
bool is3743b_spi_write_byte(uint8_t cmd, uint8_t reg, uint8_t data)
{
  uint8_t buf[3];
  buf[0] = cmd;
  buf[1] = reg;
  buf[2] = data;

  if(is3743b_cs_enable() != true)
    return false;

  ret_code_t ret = spi_transfer(buf, 3, NULL, 0);
  is3743b_cs_disable();

  if(ret != NRF_SUCCESS)
  {
    NRF_LOG_ERROR("[%s]: Failed! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }
  return true;
}

//bool is3743b_spi_read_byte(uint8_t cmd, uint8_t reg, uint8_t *data)
//{
//  uint8_t buf[2], rx[3];
//  buf[0] = cmd;
//  buf[1] = reg;
//
//  if(is3743b_cs_enable() != true)
//    return false;
//
//  ret_code_t ret = spi_transfer(buf, 2, rx, 3);
//  is3743b_cs_disable();
//
//  if(ret != NRF_SUCCESS)
//  {
//    NRF_LOG_ERROR("[%s]: Failed! L%u\r\n", (uint32_t)__func__, __LINE__);
//    return false;
//  }
//
//  *data = rx[2];
//  return true;
//}

// size 需限制在 uint8_t 长度范围
bool is3743b_spi_multi_write(uint8_t cmd, uint8_t reg, const uint8_t *data, uint16_t size)
{
  if(data == NULL || size == 0 || size > 0xC6)
  {
    NRF_LOG_ERROR("[%s]: Params Invalid! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }

  uint16_t len = size + 2;
#if 0 // 实测 VLA 会导致偶现的卡住, 导致看门狗复位
  uint8_t buf[len];   // 变长数组, 需要 C99 + VLA 支持
#endif

  uint8_t buf[0xC6 + 2];
  buf[0] = cmd;
  buf[1] = reg;
  memcpy(&buf[2], data, size);

  if(is3743b_cs_enable() != true)
    return false;

  ret_code_t ret = spi_transfer(buf, len, NULL, 0);
  is3743b_cs_disable();

  if(ret != NRF_SUCCESS)
  {
    NRF_LOG_ERROR("[%s]: Failed! L%u\r\n", (uint32_t)__func__, __LINE__);
    return false;
  }
  return true;
}

bool is3743b_cs_enable(void)
{
  // wait spi free
  if(spi_wait_free(LED_SPI_WAIT_FREE_MS_MAX) == true)
  {
    spi_enable_cs_led();
    return true;
  }
  // else
  NRF_LOG_ERROR("[%s]: wait spi free timeout! L%u\r\n", (uint32_t)__func__, __LINE__);
  return false;
}

void is3743b_cs_disable(void)
{
  spi_disable_cs_led();
}

void is3743b_delay_ms(uint32_t ms)
{
  sys_sleep_ms(ms);
}

//void is3743b_delay_us(uint32_t us)
//{
//  nrf_delay_us(us);
//}

bool is3743b_platform_init(void)
{
  // spi init
  if(spi_init() != true)
  {
    NRF_LOG_ERROR("is3743b_spi_init Failed!\r\n");
    return false;
  }

  return true;
}
