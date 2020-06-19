#ifndef __APP_TMC429_MOTION_H__
#define __APP_TMC429_MOTION_H__

#include "spi_smc_tmc429.h"
//#include "spi.h"
#include "app_eeprom_24xx.h"

//#define	USING_INC_MBTMC429
	 


#define	 USING_TMC429_CLK_12MHZ
//#define	 USING_TMC429_CLK_16MHZ
                                        

#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2

#define MVP_ABS   0            	
#define MVP_REL   1                 

//typedef struct
//{
//  UINT VMax;                   //!< maximum positioning velocity
//  UINT AMax;                   //!< maximum acceleration
//  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
//  UCHAR RampDiv;               //!< ramp divisor (TMC429)

//} TMotorConfig;

//extern TMotorConfig MotorConfig[N_O_MOTORS];

void 	  SetAmaxAutoByspeed(u8 axisNum,int speed);

uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
uint8_t TMC429_MotorStop(uint8_t motor_number);
void 		StopMotorByRamp(UINT Motor);

int 		MotorLimitCheck_thread_init(void);
int 		motor(int argc, char **argv);

#endif

