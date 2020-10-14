#ifndef  __APP_MOTION_H
#define  __APP_MOTION_H

#include "tmc429.h"

#define PARAM_ADDR		0			
#define PARAM_VER			0x00000100			

/* 全局参数 */
typedef struct
{
	uint32_t ParamVer;				//4/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
	uint32_t MasterBaudrate;				//4
	uint32_t SlaveBaudrate;
	uint8_t	 Project_ID;			//1
	
	/*  电机配置参数 */
	UINT tmc429_VMax[N_O_MOTORS];  //12   		//!< maximum positioning velocity
	UINT tmc429_AMax[N_O_MOTORS];  //12  			//!< maximum acceleration
	UCHAR tmc429_PulseDiv[N_O_MOTORS]; //3    //!< pulse divisor (TMC429)
	UCHAR tmc429_RampDiv[N_O_MOTORS];  //3    //!< ramp divisor (TMC429)

	UINT speed_home[N_O_MOTORS];	 //12	
}
PARAM_T;

extern PARAM_T g_tParam;

#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2
//Type codes of the MVP command
#define MVP_ABS   0            //!< absolute movement (with MVP command)
#define MVP_REL   1            //!< relative movement (with MVP command)
#define MVP_COORD 2            //!< coordinate movement (with MVO command)

/* USER CODE END Private defines */
//! Motor configuration data
typedef struct
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)
//  UCHAR IRun;                  //!< run current (0..255)
//  UCHAR IStandby;              //!< stand-by current(0..255)
//  UINT StallVMin;              //!< minimum speed for stallGuard
//  UINT FreewheelingDelay;      //!< freewheeling delay time (*10ms)
//  UINT SettingDelay;           //!< delay for switching to stand-by current (*10ms)
} TMotorConfig;

extern TMotorConfig MotorConfig[N_O_MOTORS];

void tmc429_hw_init(void);


uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
uint8_t TMC429_MotorStop(uint8_t motor_number);
void 		StopMotorByRamp(UINT Motor);

int 		MotorLimitCheck_thread_init(void);
int 		motor(int argc, char **argv);




#endif


