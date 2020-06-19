
#ifndef _TMC429_PARAMETER_H
#define _TMC429_PARAMETER_H

#include "bsp_defines.h"

#define N_O_MOTORS 			 6                         
#define MOTOR_NUMBER(a) (a)                    
#define WHICH_262(a) 		(a)     
#define DISABLE_DRIVERS() 	
#define ENABLE_DRIVERS()  
	
//! Motor configuration data
typedef struct
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)
	
	UCHAR SpeedChangedFlag;

/*		TMC262
  UCHAR IRun;                  //!< run current (0..255)
  UCHAR IStandby;              //!< stand-by current(0..255)
  UINT StallVMin;              //!< minimum speed for stallGuard
  UINT FreewheelingDelay;      //!< freewheeling delay time (*10ms)
  UINT SettingDelay;           //!< delay for switching to stand-by current (*10ms)
*/
} TMotorConfig;

//! coolStep configuration data
typedef struct
{
  UINT ThresholdSpeed;         //!< coolStep threshold speed
  UCHAR SlowRunCurrent;        //!< coolStep slow run current
  UCHAR HysteresisStart;       //!< coolStep hysteresis start
} TCoolStepConfig;

/********************************************************************/
//				User
/********************************************************************/
typedef struct 
{
	uint8_t Homed;
	int HomeSpeed;
	uint8_t GoHome;
	uint8_t GoLimit;
	uint8_t homeSensorPin;

}THomeInfo;

extern TMotorConfig MotorConfig[N_O_MOTORS];

extern UCHAR SmartEnergy[N_O_MOTORS];
extern UCHAR StallFlag[N_O_MOTORS];
extern UINT StallLevel[N_O_MOTORS];
extern UCHAR DriverFlags[N_O_MOTORS];
extern UCHAR MotorDisable[N_O_MOTORS];
extern UCHAR StandbyFlag[N_O_MOTORS];
extern UCHAR FreewheelingActive[N_O_MOTORS];
extern UCHAR SlowRunCurrent[N_O_MOTORS];
extern TCoolStepConfig CoolStepConfig[N_O_MOTORS];



#define TCS_IDLE  0            //!< TMCL interpreter is in idle mode (no command to process)
#define TCS_UART  1            //!< processing a command from RS485
#define TCS_UART_ERROR 2       //!< last command from RS485 had bad check sum


//Supported TMCL commands
#define TMCL_ROR   1           //!< ROR command opcode
#define TMCL_ROL   2           //!< ROL command opcode
#define TMCL_MST   3           //!< MST command opcode
#define TMCL_MVP   4           //!< MVP command opcode
#define TMCL_SAP   5           //!< SAP command opcode
#define TMCL_GAP   6           //!< GAP command opcode
#define TMCL_GetVersion 136    //!< GetVersion command opcode
#define TMCL_Boot 0xf2         //!< Boot command opcode
#define TMCL_SoftwareReset 0xff  //!< software reset command opcode

//Type codes of the MVP command
#define MVP_ABS   0            //!< absolute movement (with MVP command)
#define MVP_REL   1            //!< relative movement (with MVP command)
#define MVP_COORD 2            //!< coordinate movement (with MVO command)

//Reply format
#define RF_STANDARD 0               //!< use standard TMCL reply
#define RF_SPECIAL 1                //!< use special reply

//Data structures needed by the TMCL interpreter
//! TMCL command
typedef struct
{
  UCHAR Opcode;      //!< command opcode
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
} TTMCLCommand;

//! TMCL reply
typedef struct
{
  UCHAR Status;      //!< status code
  UCHAR Opcode;      //!< opcode of executed command
  union
  {
    long Int32;      //!< reply value as 32 bit integer
    UCHAR Byte[4];   //!< reply value as 4 bytes
  } Value;           //!< value parameter
} TTMCLReply;


//Prototypes of exported functions
void InitTMCL(void);
void ProcessCommand(void);


extern uint8_t TMC429_Dev;
#endif






