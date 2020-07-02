
#ifndef _TMC429_INTERFACE_H
#define _TMC429_INTERFACE_H

#include "bsp_defines.h"
#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"
	
#define N_O_MOTORS 			6                          
#define MOTOR_NUMBER(a) (a)                   

#define SPI_DEV0_TMC429  0                     
#define SPI_DEV1_TMC429  1 
 
#define DATA_SAVED								'A'
 
extern uint8_t	SPI_DEV0_CSPin;		
extern uint8_t	SPI_DEV1_CSPin;

typedef struct	
{																						//bytes	
	uint8_t  saved;														//1
	
	uint8_t  limit_level_valid;								//1
	uint8_t  orgin_level_valid;								//1
	
	UINT 		 VMax[N_O_MOTORS];  							//24   			//!< maximum positioning velocity
	UINT 		 AMax[N_O_MOTORS];  							//24  			//!< maximum acceleration	
	UCHAR 	 PulseDiv[N_O_MOTORS]; 						//6    			//!< pulse divisor (TMC429)
	UCHAR 	 RampDiv[N_O_MOTORS];  						//6    			//!< ramp divisor (TMC429)
	UCHAR 	 SpeedChangedFlag[N_O_MOTORS];		//6
}
TMC429_PARAM_T;	

typedef struct 
{
	uint8_t Homed[N_O_MOTORS];
	int HomeSpeed[N_O_MOTORS];
	uint8_t GoHome[N_O_MOTORS];
	uint8_t GoLimit[N_O_MOTORS];
	
}motorHoming_t;

extern TMC429_PARAM_T motorSetting;
extern motorHoming_t motorHoming;

extern volatile  uint8_t TMC429_Dev;

extern const float Vmax_factor;
extern const float Amax_factor_slow;
extern const float Amax_factor_fast;

extern uint8_t motionLocked[N_O_MOTORS];
extern uint8_t OriginSensorPin[N_O_MOTORS];
extern uint8_t OriginSensorON[N_O_MOTORS];

extern uint8_t ReadWriteSPI(uint8_t DeviceNumber, uint8_t Data, uint8_t LastTransfer);
extern void TMC429_DefaulSetting(void);
extern void tmc429_clk_init(void);
extern void bsp_spi_hw_init(void);

#endif






