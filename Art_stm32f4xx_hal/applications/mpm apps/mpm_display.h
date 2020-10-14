#ifndef __MPM_DISPLAY_H
#define __MPM_DISPLAY_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"

void sys_GetPmmC(void);
void gfx_BGcolour(uint16_t colour);

#endif

