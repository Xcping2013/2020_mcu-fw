#ifndef __BSP_MCU_DELAY_H
#define __BSP_MCU_DELAY_H 

#include "bsp_defines.h"

void drv_udelay(uint32_t us);
void drv_mdelay(uint16_t nms);

void delay_us(int us);
void delay_ms(uint16_t nms);

#endif
