#ifndef __USER_ADC_H
#define __USER_ADC_H

#include "main.h"

#include "inc_mbb372chg.h"

typedef struct	
{
	uint8_t reload;
	float K[8];
	float B[8];
}
Voltage_KB_T;

extern Voltage_KB_T Voltage_KB;

void User_ADC_Init(void);

//uint16_t GetADCReg_Average(ADC_HandleTypeDef* hadc,uint32_t ch,uint8_t times);

float GetVol_Average(uint8_t Ainx, uint8_t times);

#endif

