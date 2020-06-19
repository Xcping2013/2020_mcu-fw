#ifndef __TMC429_USER_H__
#define __TMC429_USER_H__

#include "tmc429.h"
         

#define SPI_DEV0_TMC429  0                     
#define SPI_DEV1_TMC429  1                     
#define SPI_DEV_TMC262   2  
	
#define	N_O_MOTORS 3
//#define AXIS_X	0
//#define AXIS_Y	1
//#define AXIS_Z	2

//#define MVP_ABS   0            	
//#define MVP_REL   1                 

//void 	  SetAmaxAutoByspeed(u8 axisNum,int speed);

//uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
//uint8_t TMC429_MotorStop(uint8_t motor_number);
//void 		StopMotorByRamp(UINT Motor);

//int 		MotorLimitCheck_thread_init(void);
//int 		motor(int argc, char **argv);

#endif

