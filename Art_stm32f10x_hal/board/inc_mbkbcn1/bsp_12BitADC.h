#ifndef __USER_ADC_H
#define __USER_ADC_H

#include "bsp_defines.h"

void bsp_12BitADC_init(void);

float GetADCReg_FilterVal(uint8_t ch,uint8_t times);

float ConvertADC_to_mV(uint8_t ch, float adcVal);

#endif

