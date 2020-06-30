#ifndef __APP_GPIO_DIDO_H
#define __APP_GPIO_DIDO_H

#include "bsp_mcu_gpio.h"
#include "inc_dido.h"

extern uint8_t inputs_pin_num[INPUT_COUNT];
extern uint8_t outputs_pin_num[OUTPUT_COUNT];
extern uint8_t rgb_pin_num[3];

#if defined(USING_INC_MB1616DEV6) 
//at24cxx_t at24c256=
//{
//	{0},
//	{PC_14,PC_15},
//	0xA2,	//1 0 1 0 0 0 1 0
//};

//void dido_hw_init(void);
//int readinput(int argc, char **argv);
//int output(int argc, char **argv);
//int rgb(int argc, char **argv);
//int beep(int argc, char **argv);

//uint8_t getChInput(uint8_t channel);
//void setChOutput(uint8_t channel, uint8_t setval);
#endif

//


