#ifndef __APP_TMC429_MOTION_H__
#define __APP_TMC429_MOTION_H__

#include "tmc429.h"
#include "app_eeprom_24xx.h"
                                  
#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2

#define MVP_ABS   0            	
#define MVP_REL   1                 

void 	  SetAmaxAutoByspeed(u8 axisNum,int speed);

uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
uint8_t TMC429_MotorStop(uint8_t motor_number);
void 		StopMotorByRamp(UINT Motor);

int 		MotorLimitCheck_thread_init(void);
int 		motor(int argc, char **argv);

#endif

