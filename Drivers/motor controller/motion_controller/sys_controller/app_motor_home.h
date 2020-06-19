#ifndef  __APP_MOTOR_HOME_H
#define  __APP_MOTOR_HOME_H

#include "bsp_defines.h"

typedef struct 
{
	uint8_t Homed[3];
	int HomeSpeed[3];
	uint8_t GoHome[3];
	uint8_t GoLimit[3];
}homeInfo_t;

extern homeInfo_t homeInfo;

extern uint8_t motorsReset_InOrder;

void MotorAutoReset_preset( void );

void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed);
void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed);

#endif



