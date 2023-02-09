#ifndef __IS31_PLATFORM_H__
#define __IS31_PLATFORM_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


void is37_platform_hw_off(void);
void is37_platform_hw_on(void);

bool is3743b_spi_write_byte(uint8_t cmd, uint8_t reg, uint8_t data);
//bool is3743b_spi_read_byte(uint8_t cmd, uint8_t reg, uint8_t *data);
bool is3743b_spi_multi_write(uint8_t cmd, uint8_t reg, const uint8_t *data, uint16_t size);

bool is3743b_cs_enable(void);
void is3743b_cs_disable(void);
void is3743b_delay_ms(uint32_t ms);
//void is3743b_delay_us(uint32_t us);
bool is3743b_platform_init(void);


#endif // __IS31_PLATFORM_H__
