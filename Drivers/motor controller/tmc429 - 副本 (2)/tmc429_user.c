#include "bsp_defines.h"
#include "tmc429_user.h"

enum axisParameter {

next_position, 
actual_position, 
	
next_speed, 				//int
actual_speed,				//int 
max_v_positioning, 	//int
max_acc, 						//int

position_reached,
ref_SwStatus,				//=leftLimit_SwS
rightLimit_SwS,
leftLimit_SwS,
allLimit_SwS,
rightLimit_disable,	
leftLimit_disable,

max_current,
standby_current,
	
min_speed,					//int
actual_acc,	
ramp_mode,

ref_sw_tolerance,
softStop_flag,

ramp_divisor,
pulse_divisor,	
limitSignal,
home_SenS,
is_homed,
is_stop,
is_reach,
is_reset,

Type_none,

};
typedef struct 
{
	uint8_t Homed[3];
	int HomeSpeed[3];
	uint8_t GoHome[3];
	uint8_t GoLimit[3];
}homeInfo_t;

typedef struct
{
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} TMC429_Command;

TMC429_Command CMDGetFromUart;
typedef struct
{
  uint8_t Status;               
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;

TMC429_Parameter TMC429_ParameterRW;

/*******************************user settings*************************/
struct tmc429_dev dev0=
{
	GPIOC, GPIO_PIN_4,		/*CS_PIN*/
	"spi1",								/*SPI_NAME*/
	"spi10",							/*SPI_DEV_NAME*/
	839,1000,5,8,0,				/*TMC429_DEV_SET0*/
	839,1000,5,8,0,				/*TMC429_DEV_SET1*/
	839,1000,5,8,0,				/*TMC429_DEV_SET2*/
	
	PB_9,	PE_0,	PE_1,			/*TMC429_HOME_DATA   INC-MBTMC429*/
	LOW,	LOW,	LOW,
	FALSE,FALSE,FALSE,380,	
};
struct tmc429_dev dev1=
{
	GPIOC,
	GPIO_PIN_4,
	"spi1",
	"spi10",
	800,120,5,8,0,
	800,120,5,8,0,
	800,120,5,8,0,	
	
	PB_9,	PE_0,	PE_1,			/*TMC429_HOME_DATA   INC-MBTMC429*/
	LOW,	LOW,	LOW,
	FALSE,FALSE,FALSE,380,	
};

#define PWM_DEV_NAME        "pwm3"  
#define PWM_DEV_CHANNEL     3 

struct rt_device_pwm *pwm_dev; 

void tmc429_hw_init(void)
{
	rt_uint32_t period, pulse;
	period=84;
	pulse=42;
	pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
	
	tmc429_spi_init(&dev0);
}
static uint16_t which429(uint8_t motor_number)
{
	return (motor_number<3 ? 0:1 ) ;	
}
uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
	uint8_t result = REPLY_OK;
	struct tmc429_dev dev;
  if(which429(motor_number) == 0)	dev=dev0;
	else dev=dev1;
		dev0.settings[motor_number].SpeedChangedFlag=TRUE;
		
    Set429RampMode(&dev0,motor_number, RM_VELOCITY);
		
		//SetAmaxAutoByspeed(motor_number,abs(motor_speed));
		SetAMax(&dev0,motor_number, 140);
		
    Write429Short(&dev0,IDX_VMAX|(motor_number<<5), 2047);
	  
    Write429Short(&dev0,IDX_VTARGET|(motor_number<<5), motor_speed);
	
		rt_kprintf("motor%d is rotate at %d\n",motor_number,motor_speed);
		
		return 0;	
  }
//	else if(motor_number < 6)
  return 1;
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
uint8_t TMC429_MotorStop(uint8_t motor_number)
{
  if(motor_number < 3)
  {	
		HardStop(&dev0, motor_number);
		
		rt_kprintf("motor[%d] is stop and P[%d]=%d\n",motor_number,motor_number,Read429Int(&dev0,IDX_XACTUAL|(motor_number<<5)));
		return 0;
  }
	return 1;
}

int motor(int argc, char **argv)
{

  int result = RT_ERROR;
	if (argc == 3 )
	{
		if(!strcmp(argv[1], "stop"))
			TMC429_MotorStop(atoi(argv[2]));
	}
	else if (argc == 4)
	{	
		CMDGetFromUart.Motor=atoi(argv[2]);
		CMDGetFromUart.Value.Int32=atoi(argv[3]);	

		if (!strcmp(argv[1], "rotate"))
		{
			TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32)	;
		}
	  return result;
	}
}

void tmc429_test(void)
{
	tmc429_hw_init();
	rt_thread_mdelay(100);
	TMC429_MotorRotate(2,800);
	
}
////
MSH_CMD_EXPORT(motor,...);
MSH_CMD_EXPORT(tmc429_hw_init,...);

MSH_CMD_EXPORT(tmc429_test,...);
//MSH_CMD_EXPORT(TMC429_MotorRotate,...);
//MSH_CMD_EXPORT(TMC429_MotorStop,...);


