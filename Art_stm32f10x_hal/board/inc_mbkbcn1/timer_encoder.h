#ifndef __TIMER_ENCODER_H
#define __TIMER_ENCODER_H

#include "bsp_defines.h"


extern __IO int32_t CaptureNumber;
extern int16_t OverflowCount;

extern TIM_HandleTypeDef htim2;

void Timer_EncoderInit(void);

#endif

