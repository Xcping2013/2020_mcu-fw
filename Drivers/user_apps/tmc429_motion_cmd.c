/*-------------------->>>>>>>>>>		应用说明    <<<<<<<<<<--------------------
	驱动主要配置：
	1.SPI设置	SPI模式3	SPI速率<=pdf	SPI_CS_PIN
	2.TMC429设置	
		Write429Datagram(Which429, IDX_SMGP, 0x00, 0x00->0x04, 0x02);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x06->0x04);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR0, 0x00, NO_REF->HARD_REF, 0x00);


	需要重新整理下TMC429 晶振对应的速度和加速度关系
	
	运动说明：复位状态下未屏蔽电机运行控制 需要等待复位完成
	
*/
/*******************************速度和位置变量溢出 16000000***********************************************/
#include "inc_controller.h"	
#include "app_gpio_dido.h"
#include "app_projects.h"
#include "tmc429_motion_cmd.h"
#include "motion_control.h"
/******************************************************************************************/
#define DBG_ENABLE	0
#if 	( DBG_ENABLE )
#define DEG_TRACE		rt_kprintf
#else
#define DEG_TRACE(...)		
#endif

#define MOTOR_SENSOR_THREAD_PRIORITY	18

/******************************************************************************************/
homeInfo_t HomeInfo[N_O_MOTORS];

uint8_t TMC429_motor=0;

enum axisParameter {

position_next, 
position_actual, 
position_reached,	
	
speed_next, 				//int
speed_actual,				//int 
speed_max, 					//int
acc_max, 						//int

rightLimit_SwS,
leftLimit_SwS,
allLimit_SwS,
rightLimit_disable,	
leftLimit_disable,
	
ramp_mode,

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

//
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
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;
//
TMC429_Parameter TMC429_ParameterRW;
/*********************************************************************************************************************/
static uint8_t  getTMC429_DEV(uint8_t motor_number);
static uint8_t  getMotor0_2(uint8_t motor_number);

//static void 		TMC429_ParmConifg(void);
static void 		TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);

static void 		TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void			TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);

static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);

/*********************************************************************************************************************/
static uint8_t getMotor0_2(uint8_t motor_number)
{
	return (motor_number<3 ? motor_number:(motor_number-3)) ;	
}
static uint8_t getTMC429_DEV(uint8_t motor_number)
{
	if(motor_number<3)
	{	
		return SPI_DEV1_TMC429;	
	}
	return SPI_DEV0_TMC429; 
}


/***************************************************************//**
  \fn MoveToPosition(void)
  \brief Command MVP

  MVP (Move To Position) command (see TMCL manual).
********************************************************************/
static void TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
	{		
		if(MotorConfig[motor_number].SpeedChangedFlag)
		{
			Write429Short(TMC429_DEV, IDX_VMAX|(TMC429_motor<<5), MotorConfig[motor_number].VMax);
			
			SetAmaxBySpeed(motor_number, MotorConfig[motor_number].VMax);
			
			MotorConfig[motor_number].SpeedChangedFlag=FALSE;
		}
		if(motion_mode==MVP_ABS)
		{
			//rt_kprintf("motor[%d] is start to make absolute motion\n",motor_number);
			Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), position);
		}
		else
		{
			//rt_kprintf("motor[%d] is start to make relative motion\n",motor_number);
			Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), position + Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5)));
		}
		Set429RampMode(TMC429_DEV,TMC429_motor, RM_RAMP);
	}
}
/***************************************************************//**
  \fn GetAxisParameter(void)
  \brief Command GAP

  GAP (Get Axis Parameter) command (see TMCL manual).
********************************************************************/
static void TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type)
{	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
  TMC429_ParameterRW.Value.Int32=0;
	
	switch(parameter_type)
	{
		case position_next:
			TMC429_ParameterRW.Value.Int32=Read429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5));				
			rt_kprintf("motor[%d] next position=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case position_actual:
			TMC429_ParameterRW.Value.Int32=Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5));
			rt_kprintf("motor[%d] actual position=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case speed_next:
			TMC429_ParameterRW.Value.Int32=Read429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5));
			
			rt_kprintf("motor[%d] next speed=%d\n",motor_number,ChangeVMaxToSpeed_ms(TMC429_ParameterRW.Value.Int32));
		
			break;

		case speed_actual:
			TMC429_ParameterRW.Value.Int32=Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5));
			rt_kprintf("motor[%d] actual speed=%d\n",motor_number,ChangeVMaxToSpeed_ms(TMC429_ParameterRW.Value.Int32));
			break;

		case speed_max:
			TMC429_ParameterRW.Value.Int32=MotorConfig[motor_number].VMax;
			rt_kprintf("motor[%d] max positioning speed=%d\n",motor_number,ChangeVMaxToSpeed_ms(TMC429_ParameterRW.Value.Int32));
			break;

		case acc_max:
			TMC429_ParameterRW.Value.Int32=MotorConfig[motor_number].AMax;
			rt_kprintf("motor[%d] max acceleration=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case position_reached:
		
			rt_kprintf("motor[%d] position reached=%d\n",motor_number,(Read429Status(TMC429_DEV) & (0x01<<TMC429_motor*2)) ? 1:0);
			break;

		case leftLimit_SwS:
			TMC429_ParameterRW.Value.Int32=(Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x02<<TMC429_motor*2)) ? 1:0;
			rt_kprintf("motor[%d] left limit switch status=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case rightLimit_SwS:
			TMC429_ParameterRW.Value.Int32=(Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x01<<TMC429_motor*2)) ? 1:0;
			rt_kprintf("motor[%d] right limit switch status=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;
		
		case allLimit_SwS:
			{
					uint8_t limit_char[12];
					uint8_t i;
					uint8_t limit = Read429SingleByte(SPI_DEV0_TMC429,IDX_REF_SWITCHES, 3);
					for(i=0;i<6;i++)
					{
						limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
					}
					limit = Read429SingleByte(SPI_DEV1_TMC429,IDX_REF_SWITCHES, 3);
					for(i=0;i<6;i++)
					{
						limit_char[i+6]=(limit&(0x20>>i)) ? '1':'0';
					}
					rt_kprintf("motor limit switch status L5R5L4R4L3R3L2R2L1R1L0R0=%.12s\n",limit_char);
			}
			break;

		case rightLimit_disable:
			TMC429_ParameterRW.Value.Byte[0]=(Read429SingleByte(TMC429_DEV,IDX_REFCONF_RM|(TMC429_motor<<5), 2) & 0x02) ? 1:0;
			rt_kprintf("motor[%d] right limit switch disable=%d\n",motor_number,TMC429_ParameterRW.Value.Byte[0]);
			break;

		case leftLimit_disable:
			TMC429_ParameterRW.Value.Byte[0]=(Read429SingleByte(TMC429_DEV,IDX_REFCONF_RM|(TMC429_motor<<5), 2) & 0x01) ? 1:0;
			rt_kprintf("motor[%d] left limit switch disable=%d\n",motor_number,TMC429_ParameterRW.Value.Byte[0]);
			break;

		case ramp_mode:
			TMC429_ParameterRW.Value.Byte[0]=Read429SingleByte(TMC429_DEV,IDX_REFCONF_RM|(TMC429_motor<<5), 3);
			rt_kprintf("motor[%d] ramp mode=%d\n",motor_number,TMC429_ParameterRW.Value.Byte[0]);
			break;

		case ramp_divisor:
			TMC429_ParameterRW.Value.Byte[0]=Read429SingleByte(TMC429_DEV,IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5), 2) & 0x0f;
			rt_kprintf("motor[%d] ramp divisor=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case pulse_divisor:
			TMC429_ParameterRW.Value.Byte[0]=Read429SingleByte(TMC429_DEV,IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5), 2) >> 4;
			rt_kprintf("motor[%d] pulse divisor=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case is_homed:
			rt_kprintf("motor[%d] homed=%d\n",motor_number,HomeInfo[motor_number].Homed);
			break;
		case is_stop:
			rt_kprintf("motor[%d] stop=%d\n",motor_number,Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))? 0:1) ;
			break;
		case is_reach:

			rt_kprintf("motor[%d] reach=%d\n",motor_number,(Read429Status(TMC429_DEV) & (0x01<<TMC429_motor*2)) ? 1:0);

			break;
		case home_SenS:
			rt_kprintf("motor[%d] homeSensor=%d\n",motor_number, rt_pin_read(OriginSensorPin[motor_number]) ? 0:1) ;
			break;						
		default:
			break;
	}
}
static void TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value)
{
	
  UCHAR Read[4], Write[4];
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);

	TMC429_ParameterRW.Value.Int32=parameter_value;
	
	switch(parameter_type)
	{
		case position_next:
			Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), TMC429_ParameterRW.Value.Int32);
			rt_kprintf("set motor[%d] next position=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case position_actual:
			Write429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5), TMC429_ParameterRW.Value.Int32);
			rt_kprintf("set motor[%d] actual position=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case speed_next:

			{			
				Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));
				
				rt_kprintf("set motor[%d] next speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			}
			break;

		case speed_actual:
			{			
				Write429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5), ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));
			
				rt_kprintf("set motor[%d] actual speed=%d ok\n",motor_number,CMDGetFromUart.Value.Int32);	
			}
			break;

		case speed_max:
			{			
				MotorConfig[motor_number].VMax=ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32);
				Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), MotorConfig[motor_number].VMax);
				SetAmaxBySpeed(motor_number,MotorConfig[motor_number].VMax);
				rt_kprintf("set motor[%d] speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);	
			}			
			break;

		case acc_max:
			MotorConfig[motor_number].AMax=TMC429_ParameterRW.Value.Int32;
			SetAMax(TMC429_DEV,TMC429_motor, MotorConfig[motor_number].AMax);
			rt_kprintf("set motor[%d] max acceleration speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);		
			break;

		case rightLimit_disable:	//12
			Write[0]=IDX_REFCONF_RM|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			if(CMDGetFromUart.Value.Byte[0]!=0)
				Write[2]=Read[2]|0x02;
			else
				Write[2]=Read[2]&  ~0x02;
			Write[3]=Read[3];
			Write[0]=IDX_REFCONF_RM|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			if(CMDGetFromUart.Value.Byte[0]!=0)
				rt_kprintf("motor[%d] right limit switch is disable\n",motor_number);	
			else 
				rt_kprintf("motor[%d] right limit switch is enable\n",motor_number);	
			break;

		case leftLimit_disable: //13
			Write[0]=IDX_REFCONF_RM|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			if(CMDGetFromUart.Value.Byte[0]!=0)
				Write[2]=Read[2]|0x01;
			else
				Write[2]=Read[2]&  ~0x01;
			Write[3]=Read[3];
			Write[0]=IDX_REFCONF_RM|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			if(CMDGetFromUart.Value.Byte[0]!=0)
				rt_kprintf("motor[%d] left limit switch is disable\n",motor_number);	
			else 
				rt_kprintf("motor[%d] left limit switch is enable\n",motor_number);	
			break;

		case ramp_mode:
			Set429RampMode(TMC429_DEV,TMC429_motor, TMC429_ParameterRW.Value.Byte[0]);
			rt_kprintf("set motor[%d] ramp mode=%d ok\n",motor_number,TMC429_ParameterRW.Value.Byte[0]);
			break;

		case ramp_divisor:
			Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			Write[2]=(Read[2] & 0xf0) | (CMDGetFromUart.Value.Byte[0] & 0x0f);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			MotorConfig[motor_number].RampDiv=CMDGetFromUart.Value.Byte[0] & 0x0f;
			rt_kprintf("set motor[%d] ramp divisor=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case pulse_divisor:
			Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			Write[2]=(Read[2] & 0x0f) | (CMDGetFromUart.Value.Byte[0] << 4);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			MotorConfig[motor_number].PulseDiv=CMDGetFromUart.Value.Byte[0]& 0x0f;
			rt_kprintf("set motor[%d] pulse divisor=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;
		case limitSignal:
			if(TMC429_ParameterRW.Value.Int32!=0) 
			{
				Write429Int(TMC429_DEV,IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
			}
			else Write429Int(TMC429_DEV,IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
			rt_kprintf("set limit signal effective trigger level=%d ok\n",TMC429_ParameterRW.Value.Int32);
			break;
			
		default:
			break;
	}
}

void SetAmaxBySpeed(u8 motor_number , int32_t speed)	//命令输入的是脉冲数 不再是TMC429内部寄存器值
{	

	SelectAMaxByVMax(motor_number,speed);
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	SetAMax(TMC429_DEV,TMC429_motor,  MotorConfig[motor_number].AMax);
	
//	if(MotorConfig[motor_number].PulseDiv==6 &&	MotorConfig[motor_number].RampDiv==8)
//	{
//		//按项目进行速度设置 0.5mm/S为最小速度  其他速度都大于此设置值
//		if (speed==105)										MotorConfig[motor_number].AMax=275;			
//		else if (speed<50)								MotorConfig[motor_number].AMax=10;	
//		else if (49<speed && speed<105)		MotorConfig[motor_number].AMax=speed+10;	
//		else if (105<speed && speed<210)	MotorConfig[motor_number].AMax=speed*2;	
//		else if (209<speed && speed<400)	MotorConfig[motor_number].AMax=1000;	
//		else 															MotorConfig[motor_number].AMax=2047;	
//		
//		SetAMax(TMC429_DEV,TMC429_motor, MotorConfig[motor_number].AMax);
//	}
//	else if(MotorConfig[motor_number].PulseDiv==5 &&	MotorConfig[motor_number].RampDiv==8)
//	{
//		if (speed==70)										MotorConfig[motor_number].AMax=200;			//0.05s-->210---0.5/S
//		else if (speed<50)								MotorConfig[motor_number].AMax=speed;	
//		else if (49<speed && speed<105)		MotorConfig[motor_number].AMax=speed*2;	
//		else if (104<speed && speed<210)	MotorConfig[motor_number].AMax=speed*3;		
//		else 															MotorConfig[motor_number].AMax=2000;	
//		SetAMax(TMC429_DEV,TMC429_motor,  MotorConfig[motor_number].AMax);
//	}
}
//
//
//
/*
 对轴号和参数范围先进行判断	内部调用函数不再进行判断
		0		 		1       		2 							3					4						5
argc=2
  motor   reset			//封装一系列动作，需要提取项目号来触发，不建议应用
argc=3
	motor 	stop  		<motor_number> 
argc=4
	motor 	rotate 		<motor_number> 	<axis_speed>  
  motor 	move	 		<motor_number> 	<position> 
	motor 	moveto	 	<motor_number> 	<position> 
	motor 	get 			<motor_number> 	<type>  

  motor 	gohome 		<motor_number> 	<speed> 
	motor 	golimit		<motor_number> 	<speed> 
argc=5
  motor 	set 			<type> 			<motor_number> 				<value>  

  	
*/
static void	printf_cmdList_motor(void)
{
		rt_kprintf("\n");
		rt_kprintf("Usage: \n");
		rt_kprintf("\n");
		rt_kprintf("motor stop <var>\n");		
		rt_kprintf("    var:0~5\n");
		rt_kprintf("\n");
		rt_kprintf("motor rotate <var1> <var2>\n");	
	  rt_kprintf("    var1:0~5 var2:pulse per ms\n");
		rt_kprintf("\n");
		rt_kprintf("motor moveto <var1> <var2>\n");
		rt_kprintf("motor move <var1> <var2>\n");
	  rt_kprintf("    var1:0~5	var2:position pulse\n");
		rt_kprintf("\n");
		rt_kprintf("motor get <var1> <var2>\n");	
		rt_kprintf("    var2:0~5\n");
		rt_kprintf("    var1: VMAX | AMAX | position | next_position | speed | next_speed\n");
		rt_kprintf("    var1: homeSensor | limit | rightLimit | leftLimit | is_homed | is_reach | is_stop\n");
	  rt_kprintf("    var1: ramp_div | pulse_div\n");
		rt_kprintf("\n");
		rt_kprintf("motor set <var1> <var2> <var3>\n");	
	  rt_kprintf("    var2:0~5 var3:value to set\n");
		rt_kprintf("    var1: speed | VMAX | AMAX | position_next\n");
	  rt_kprintf("    var1: ramp_div | pulse_div | limitSignal | leftLimit | rightLimit\n");
		rt_kprintf("\n");
		rt_kprintf("motor gohome <var1> <var2>\n");	
  	rt_kprintf("motor golimit <var1> <var2>\n");	
		rt_kprintf("    var1:0~5 var2:pulse per ms\n");
		rt_kprintf("\n");
//		rt_kprintf("motor reset                           - motor reset\n");	
//		rt_kprintf("motor stop reset                      - motor reset\n");	
//		rt_kprintf("motor reset?                          - is axis reset or not\n");		
		rt_kprintf("\n");
}
static void printf_cmdList_motor_set(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor set speed <axis> <value>             -set the rotate speed \n");				
		rt_kprintf("motor set position_next <axis> <value>      -set the target position to move\n");
		rt_kprintf("motor set position_actual <axis> <value>    -set the current position\n");				
		rt_kprintf("motor set speed_actual <axis> <value>       -set the current speed \n");
		rt_kprintf("motor set VMAX <axis> <value>               -set max positioning speed\n");
		rt_kprintf("motor set AMAX <axis> <value>               -set max acceleration\n");
		rt_kprintf("motor set ramp_div <axis> <value>           -set ramp divisor value\n");
		rt_kprintf("motor set pulse_div <axis> <value>          -set pulse divisor value\n");	
		rt_kprintf("motor set rightLimit <axis> <value>         -set right limit switch\n");
		rt_kprintf("motor set leftLimit <axis> <value>          -set left limit switch\n");
		rt_kprintf("motor set limitSignal <axis> <value>        -set limit signal effective trigger level\n");		
}
static void printf_cmdList_motor_get(void)
{
	rt_kprintf("Usage: \n");
	//用户接口
	rt_kprintf("motor get speed <axis>             -get the current speed \n");
	rt_kprintf("motor get position <axis>          -get the current position\n");
	 
	rt_kprintf("motor get limit all                -get all limit switch status\n");	
	rt_kprintf("motor get rightLimit <axis>        -get right switch status\n");
	rt_kprintf("motor get leftLimit  <axis>        -get left switch status\n");	
	
	rt_kprintf("motor get is_homed <axis>          -is axis homed or not\n");	
	rt_kprintf("motor get is_stop <axis>           -is axis stop or not\n");	
	rt_kprintf("motor get is_reach <axis>          -is axis reach the position\n");	
	rt_kprintf("motor get homeSensor <axis>        -get home sensor status\n");	
	//调试接口
	rt_kprintf("motor get next_speed <axis>        -get the target speed \n");
	rt_kprintf("motor get next_position <axis>     -get the target position to move\n");
	
	rt_kprintf("motor get VMAX <axis>              -get max positioning speed\n");
	rt_kprintf("motor get AMAX <axis>              -get max acceleration\n");

	rt_kprintf("motor get position_reached <axis>  -is reach positon or not\n");
	rt_kprintf("motor get ramp_div <axis>          -get ramp divisor value\n");
	rt_kprintf("motor get pulse_div <axis>         -get pulse divisor value\n");			
	
}
/*********************************************************************************************************************/
/***************************************************************//**
  \fn RotateRight(void)
  \brief Command ROR (see TMCL manual)
********************************************************************/
void TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	MotorConfig[motor_number].SpeedChangedFlag=TRUE;
	
	Set429RampMode(TMC429_DEV,	TMC429_motor, RM_VELOCITY);

	//需要重新配加速度
	SetAmaxBySpeed(motor_number,motor_speed);	
	
	Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), 2047);
	
	Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), motor_speed);

	//rt_kprintf("motor%d is rotate at vmax= %d\n",motor_number,motor_speed);
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
void TMC429_MotorStop(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	HardStop(TMC429_DEV,TMC429_motor);
	//StopMotorByRamp(motor_number);
	/* 回原点过程中响应停止命令的变量置位 */
	HomeInfo[motor_number].GoHome=0;
	HomeInfo[motor_number].GoLimit=0;
	/* 电机停止会自动返回当前位置 */
	rt_kprintf("motor[%d] is stop and P[%d]=%d\n",motor_number,motor_number,Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5)));
}
void StopMotorByRamp(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	HomeInfo[motor_number].GoHome=0;
	HomeInfo[motor_number].GoLimit=0;
	
  Set429RampMode(TMC429_DEV,MOTOR_NUMBER(TMC429_motor), RM_VELOCITY);
  Write429Zero(TMC429_DEV,(MOTOR_NUMBER(TMC429_motor)<<5)|IDX_VTARGET);
}

//buttonRESETpressed  --> motorIsResetting
void tmc429_hw_init(void)
{
	TMC429_ParmConifg();

	pinMode(DEV0CS_PIN,PIN_MODE_OUTPUT);
	pinSet(DEV0CS_PIN);	
	pinMode(DEV1CS_PIN,PIN_MODE_OUTPUT);
	pinSet(DEV1CS_PIN);	

	MX_SPI1_Init();
	MX_SPI2_Init();

	Init429(SPI_DEV0_TMC429);
	Init429(SPI_DEV1_TMC429);
	
	if((Read429SingleByte(SPI_DEV0_TMC429,IDX_PULSEDIV_RAMPDIV|(0<<5), 2) & 0x0f )== (MotorConfig[0].RampDiv))
	{
		rt_kprintf("spi dev0 tmc429 [found]\n");
	}
	//else rt_kprintf("spi dev0 tmc429 [no found]\n");
	
	if((Read429SingleByte(SPI_DEV1_TMC429,IDX_PULSEDIV_RAMPDIV|(0<<5), 2) & 0x0f )== (MotorConfig[3].RampDiv))
	{
		rt_kprintf("spi dev1 tmc429 [found]\n");
	}
	//else rt_kprintf("spi dev1 tmc429 [no found]\n");
}

/*	项目复位动作在项目文件中编写  另起命令 如 motion reset
if 			(!strcmp(argv[1], "reset?"))	{	rt_kprintf("motor reset=%d\n",motorsResetOK) ;}	
else	if (!strcmp(argv[1], "reset"))	{	motorsResetInOrder=TRUE;}
*/
/*#################################################################################################
*/
int motor(int argc, char **argv)
{
	int result = REPLY_OK;
	
	static uint8_t tmc429_inited=0;
	
	if(tmc429_inited==0)
	{
		tmc429_inited=1;
		//	MX_TIM3_Init();	
		//	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
		tmc429_hw_init();						//TMC429_CLK=16MHZ
		MotorSensor_thread_init();
	
	}
	CMDGetFromUart.Type=Type_none;
		
	if		  (argc == 1 )  									{	printf_cmdList_motor();	}
	else if (argc == 2 )					
	{
		if 			 (!strcmp(argv[1], "set"))		{ printf_cmdList_motor_set();}
		else	if (!strcmp(argv[1], "get"))		{	printf_cmdList_motor_get();}
		else																	{ result = REPLY_INVALID_CMD;}
	}
	else if (argc == 3 )	
	{
		CMDGetFromUart.Motor=atoi(argv[2]);
		
		if (CMDGetFromUart.Motor > N_O_MOTORS) result = REPLY_INVALID_VALUE;
			
		else if(!strcmp(argv[1], "stop"))
		{
			TMC429_MotorStop(CMDGetFromUart.Motor);
		}
		else result = REPLY_INVALID_CMD;
	}
	else if (argc == 4)
	{	
		if (!strcmp(argv[1], "get"))
		{
			if      (!strcmp(argv[2], "speed")) 	 				CMDGetFromUart.Type=speed_actual;
			else if (!strcmp(argv[2], "position")) 				CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart.Type=is_homed;
			else if (!strcmp(argv[2], "is_stop")) 				CMDGetFromUart.Type=is_stop;
			else if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart.Type=is_reach;

			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;		
			else if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart.Type=speed_max;
			else if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=acc_max;
			else if (!strcmp(argv[2], "position_reached"))CMDGetFromUart.Type=position_reached;		
			
			else if (!strcmp(argv[2], "rightLimit")) 			CMDGetFromUart.Type=rightLimit_SwS;
			else if (!strcmp(argv[2], "leftLimit")) 			CMDGetFromUart.Type=leftLimit_SwS;
			else if (!strcmp(argv[2], "limit")) 			    CMDGetFromUart.Type=allLimit_SwS;
			
			else if (!strcmp(argv[2], "homeSensor")) 			CMDGetFromUart.Type=home_SenS;
			
			else if (!strcmp(argv[2], "rightLimit?"))			CMDGetFromUart.Type=rightLimit_disable;
			else if (!strcmp(argv[2], "leftLimit?")) 			CMDGetFromUart.Type=leftLimit_disable;
			
			else if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
			else if (!strcmp(argv[2], "pulse_div")) 			CMDGetFromUart.Type=pulse_divisor;	
				
			if(CMDGetFromUart.Type!=Type_none)
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				else TMC429_GetAxisParameter(CMDGetFromUart.Motor,CMDGetFromUart.Type);	
			}
			else 
			{
				result=REPLY_WRONG_TYPE;
				printf_cmdList_motor_get();			
			}
		}		
		else 
		{
			CMDGetFromUart.Motor=atoi(argv[2]);
			CMDGetFromUart.Value.Int32=atol(argv[3]);	
													
			if (CMDGetFromUart.Motor >=N_O_MOTORS) result = REPLY_INVALID_VALUE;
			
			else if (!strcmp(argv[1], "rotate"))
			{						
				TMC429_MotorRotate(CMDGetFromUart.Motor,ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32))	;
				
				rt_kprintf("motor%d is rotate at %d pulse per ms\n",CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);				
			}
			else if (!strcmp(argv[1], "move") )
			{			
				TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32);
				rt_kprintf("motor[%d] is start to make relative motion\n",CMDGetFromUart.Motor);
			}
			else if (!strcmp(argv[1], "moveto"))
			{
				rt_kprintf("motor[%d] is start to make absolute motion\n",CMDGetFromUart.Motor);
				TMC429_MoveToPosition(CMDGetFromUart.Motor, MVP_ABS, CMDGetFromUart.Value.Int32);
			}
			else if (!strcmp(argv[1], "gohome"))
			{
				rt_kprintf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);		
				motor_gohome_config(CMDGetFromUart.Motor, ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));		
			}
			else if (!strcmp(argv[1], "golimit"))
			{	
				rt_kprintf("motor[%d] is start go home by searching limit sensor\n",CMDGetFromUart.Motor);	
				motor_golimit_config(CMDGetFromUart.Motor,ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));							
			}
			else result = REPLY_INVALID_CMD;
		}	
	}
	else if (argc == 5)
	{
		if (!strcmp(argv[1], "set"))
		{
			//user
			if (!strcmp(argv[2], "speed")) 					 		  CMDGetFromUart.Type=speed_max;
			//debug
			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;		
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;
			else if (!strcmp(argv[2], "actual_position")) CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "speed_actual")) 	 	CMDGetFromUart.Type=speed_actual;
			else if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart.Type=speed_max;
			else if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=acc_max;
			
			else if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart.Type=rightLimit_disable;
			else if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_disable;	  		
			else if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart.Type=limitSignal;		
			else if (!strcmp(argv[2], "ramp_div")) 			  	CMDGetFromUart.Type=ramp_divisor;
			else if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;

			if(CMDGetFromUart.Type!=Type_none)	
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				CMDGetFromUart.Value.Int32=atoi(argv[4]);
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				else TMC429_SetAxisParameter(CMDGetFromUart.Motor, CMDGetFromUart.Type, CMDGetFromUart.Value.Int32);
			}
			else 
			{
				result =REPLY_WRONG_TYPE;				
				printf_cmdList_motor_set();													
			}				
		}
	}		
	if(	result == REPLY_INVALID_VALUE )
	{
		rt_kprintf("motor number must be 0~5\n");
	}
	else if( result == REPLY_INVALID_CMD )	{	printf_cmdList_motor();	}

	return result;
}

/*#################################################################################################
*/
/**************************************************************************************************/
/******************************回原点相关动作 || 找限位等相关动作**********************************/
/**************************************************************************************************/
/******************************************Origin*********************************************************************

default OriginSensor --> IN GPIO

回原点动作中,需要及时采集原点信号,进行电机停止并设立当前位置为0,这个动作如果delay太久会影响其他轴的回0动作！！！

SO: 
-->设置单轴回原点-->设置原点传感器为外部中断（触发方式串口传感器型号)-->采集到信号，判断回零方向正确的话，设立当前位置为0，并关闭此外部中断。(加调试LOG)
-->回0动作中尽量不要使用串口采集信息，外部中断优先级大于串口，串口数据会出现异常

多轴自动回原点在单轴回原点基础上进行HomeInfo.GoHome和HomeInfo.Homed 进行管理

*********************************************************************************************************************/
/******************************************Limit*********************************************************************

回原点动作中会触发限位信号，对检测时间要求不高，可计数检测到50ms后进行反转再寻原点信号

*********************************************************************************************************************/


/***************************************************************************/
//寻找原点的过程被PC停止或者运动命令干涉的话，回原动作会出现异超
//所以回原点未完成的状态需要屏蔽其他动作命令,且增加自锁不要自己反复复位
/***************************************************************************/
//USER:	motorsResetInOrder=TRUE 触发复位动作
/***********************************************************************/
static rt_uint8_t MotorSensorStack[ 512 ];
static struct	rt_thread MotorSensorThread;
/***************************************************************************/
static void InitOriginSensorAsEXTI(void);
static void OriginSensorIRQEnable(uint8_t motor_number);
static void OriginSensorIRQDisable(uint8_t motor_number);
static void OriginSensorIRQCall(void *args);

static void setPositionAsOrigin(uint8_t motor_number);
static void LimitSensorProcess(uint8_t motor_number);
static void MotorSensor_Thread_entry(void *parameter);

/***************************************************************************/
// //原点过程触发限位		//正常运动中触发限位,超时停止
static void LimitSensorProcess(uint8_t motor_number)
{
	static uint8_t motorlimitedCNT[6];	 
	static uint8_t limitedCNT_NoHome[6];
	
//	static uint8_t time_delay;
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);	
	
	if(HomeInfo[motor_number].GoHome && Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0	)
	{
		motorlimitedCNT[motor_number]++;
		if(motorlimitedCNT[motor_number]>=3)
		{
			//确定停止是由于限位触发的原因后，电机反转
			u8 SwitchStatus=Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3);
			
			if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && ( (SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0))
			{
				//两个限位都触发，不正常
				//Do nothing
			}
			else if((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0)													//触发左限位
			{		
				//TMC429_MotorRotate(motor_number,	(abs(HomeInfo[motor_number].HomeSpeed)/4));	//向右转			
				TMC429_MotorRotate(motor_number,	(abs(HomeInfo[motor_number].HomeSpeed)));	//向右转		
			}
			else if((SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0)													//触发右限位
			{																													 		
				TMC429_MotorRotate(motor_number,-abs(HomeInfo[motor_number].HomeSpeed));				//左转
			}
			motorlimitedCNT[motor_number]=0;
		}
	}
	//电机回限位过程中停止了
	else if(HomeInfo[motor_number].GoLimit && Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0	)
	{
		//确定停止是由于限位触发的原因后，电机复位OK
		uint8_t SwitchStatus=Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3);	
		
		//rt_kprintf("home_speed=%d\n",HomeInfo[motor_number].HomeSpeed);
		
		if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && ( (SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0))
		{
			//两个限位都触发，不正常
			//Do nothing
			//L TMC429_ParameterRW.Value.Int32=(Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x02<<TMC429_motor*2)) ? 1:0;
		}
		
		else if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && (HomeInfo[motor_number].HomeSpeed<0)) 								
		{																													 
			setPositionAsOrigin(motor_number);							
		}
		else if(((SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0) && (HomeInfo[motor_number].HomeSpeed>0)) 
		{																													
			setPositionAsOrigin(motor_number);				
		}
	}
	//limitedCNT_NoHome[motor_number] 需要一一对应 不要错位
	else if( limitedCNT_NoHome[motor_number]++>20 && HomeInfo[motor_number].GoHome==FALSE && HomeInfo[motor_number].GoLimit==FALSE )	//间隔一段时间检查是否触发限位	
	{
		if(Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0	) 		//谨慎使用 电机速度为0 说明电机正常停止后者触发限位了
		{

			if(limitedCNT_NoHome[motor_number]>40) 
			{
				HardStop(TMC429_DEV,TMC429_motor);							//停止
				limitedCNT_NoHome[motor_number]=0;		
			}
		}	
		else limitedCNT_NoHome[motor_number]=0;		
	}
//	rt_kprintf("motor_number befor=%d\n",motor_number);

//	if( ( time_delay++>8) && (HomeInfo[motor_number].GoHome==FALSE) && (HomeInfo[motor_number].GoLimit==FALSE )	)//间隔一段时间检查是否触发限位	
//	{
//		
//		uint8_t SwitchStatus=Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3);	
//		
//		time_delay=0;
//		
//		rt_kprintf("motor_number after=%d\n",motor_number);

//		
//		if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) || ((SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0))
//		{
//			if(limitedCNT_NoHome[motor_number]++>5)
//			{
//				rt_kprintf("%d HardStop\n",motor_number);
//					
//				HardStop(TMC429_DEV,TMC429_motor);
//				
//			}
//				
//		}
//		else limitedCNT_NoHome[motor_number]=0;
//	}
}
//
static void MotorSensor_Thread_entry(void *parameter)
{
	InitOriginSensorAsEXTI();
	while (1)
	{  
		rt_enter_critical();		
		
		motorResetByProID();
		
		for(uint8_t i=0;i<N_O_MOTORS;i++)
		{			
			LimitSensorProcess(i);
		}	
		
		rt_exit_critical();
		
		rt_thread_mdelay(50);		
	}		
}
int MotorSensor_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&MotorSensorThread,
                            "MotorSensor",
                            MotorSensor_Thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&MotorSensorStack[0],
                            sizeof(MotorSensorStack),
                            MOTOR_SENSOR_THREAD_PRIORITY,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&MotorSensorThread);
    }
    return 0;
}
static void setPositionAsOrigin(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	HardStop(TMC429_DEV,TMC429_motor); 
	delay_ms(20);
	Set429RampMode(TMC429_DEV,MOTOR_NUMBER(TMC429_motor), RM_VELOCITY);
	Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), 0);
	Write429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5), 0);
	
	HomeInfo[motor_number].Homed=TRUE;
	HomeInfo[motor_number].GoHome=FALSE;	
	HomeInfo[motor_number].GoLimit=FALSE;	
	
	rt_kprintf("motor[%d] go home ok\n",motor_number);	
}

//GPIO_PULLUP	stm32_pin_irq_enable
static void InitOriginSensorAsEXTI(void)
{
	uint8_t i;
	for(i=0;i<6;i++)
	{
		if(OriginSensorON[i]==HIGH)
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_RISING,  OriginSensorIRQCall, (void*)i);	
		else
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_FALLING, OriginSensorIRQCall, (void*)i);	
	}
	//rt_kprintf("Motor sensor initialization is completed\n");
}

static void OriginSensorIRQEnable(uint8_t motor_number)
{
	rt_pin_irq_enable(OriginSensorPin[motor_number], PIN_IRQ_ENABLE);
}
static void OriginSensorIRQDisable(uint8_t motor_number)
{
	rt_pin_irq_enable(OriginSensorPin[motor_number], PIN_IRQ_DISABLE);
}
static void OriginSensorIRQCall(void *args)
{
	rt_uint32_t motor_number;
	motor_number = (rt_uint32_t)args;
	
	if(motor_number<N_O_MOTORS)
	{
		TMC429_motor=getMotor0_2(motor_number);
		TMC429_DEV=getTMC429_DEV(motor_number);
		//确定触发原点信号
		if((OriginSensorON[motor_number]==HIGH && pinRead(OriginSensorPin[motor_number])==HIGH )||
			 (OriginSensorON[motor_number]==LOW && pinRead(OriginSensorPin[motor_number])==LOW	 ))
		{
			
			//rt_kprintf("触发原点信号\n");
			
			if((HomeInfo[motor_number].HomeSpeed >0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))<0 ) || 
				 (HomeInfo[motor_number].HomeSpeed <0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))>0 )    )
			{	
				//触发原点的状态满足回原方式,停止电机把当前位置寄存器写0,并关闭原点中断功能			
				setPositionAsOrigin(motor_number);
				OriginSensorIRQDisable(motor_number);	
			}
		}	
	}
}
//
/*******************************************************************/
//reset使用的是eeprom内参数 否则使用命令的参数												
/***************************************************************************/
void motor_gohome_config(uint8_t motor_number, int home_speed)
{
	HomeInfo[motor_number].GoHome			=	TRUE;
	HomeInfo[motor_number].GoLimit		=	FALSE;
	HomeInfo[motor_number].Homed			=	FALSE;
	HomeInfo[motor_number].HomeSpeed	=	home_speed;	
		
	OriginSensorIRQEnable(motor_number);
	TMC429_MotorRotate(motor_number,home_speed);
}
void motor_golimit_config(uint8_t motor_number, int home_speed)
{
	HomeInfo[motor_number].GoHome			=	FALSE;
	HomeInfo[motor_number].GoLimit		=	TRUE;
	HomeInfo[motor_number].Homed			=	FALSE;
	HomeInfo[motor_number].HomeSpeed=	home_speed;	
	
//	SetAmaxBySpeed(motor_number, home_speed);
//	rt_kprintf("home_speed=%d\n",home_speed);
	TMC429_MotorRotate(motor_number,home_speed);	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/******************************************Origin std Code***************************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t  CommmandLine_speed(char *Commands)
{
	if(strncmp(Commands+1,"# velocity ",11)==0 ) 
  {
		CMDGetFromUart.Motor=Commands[0]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			char *p = NULL;
			char *s = &Commands[12];
			u8 len;		
			MotorConfig[CMDGetFromUart.Motor].VMax=strtol(s, &p, 10);	
			len=p-s;				
			if(0<len&&len<=4)			
      {
				Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), MotorConfig[CMDGetFromUart.Motor].VMax);
				//SetAMaxBySpeed by project
				rt_kprintf("\nVset[%d-%d]<OK>\n",CMDGetFromUart.Motor,MotorConfig[CMDGetFromUart.Motor].VMax);

				return REPLY_OK;
			}
			else return REPLY_INVALID_VALUE;
     }
		 else return REPLY_INVALID_CMD;
	}
/**   读取单个电机当前速度
**					
**/	
	else if(strncmp(Commands,"velocity #",10)==0 ) 
	{
		CMDGetFromUart.Motor=Commands[10]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_ParameterRW.Value.Int32=Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5));
			rt_kprintf("\nVset[%d]<%d>,V[%d]<%d>\n",CMDGetFromUart.Motor,MotorConfig[CMDGetFromUart.Motor].VMax,CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
			return REPLY_OK;
		}	
		else return REPLY_INVALID_VALUE;
	}
	else return REPLY_INVALID_CMD;	
}
//
uint8_t  CommmandLine_acc(char *Commands)
{
	if(strncmp(Commands+1,"# acc ",6)==0 ) 
  {
		CMDGetFromUart.Motor=Commands[0]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			char *p = NULL;
			char *s = &Commands[7];
			u8 len;
			MotorConfig[CMDGetFromUart.Motor].AMax=strtol(s, &p, 10);	
			len=p-s;				
			if(0<len&&len<=4)				
      {
        SetAMax(TMC429_DEV,TMC429_motor, MotorConfig[CMDGetFromUart.Motor].AMax);
				rt_kprintf("A[%d-%d]<OK>\n",CMDGetFromUart.Motor,MotorConfig[CMDGetFromUart.Motor].AMax);				
				return REPLY_OK;
      }
    }
		else return REPLY_INVALID_VALUE;
	}		
/**   读取所有电机当前加速度
**					
**/		
	else if(strcmp(Commands,"acc #all")==0 ) 
	{
		rt_kprintf("\nAcc<%d,%d,%d,%d,%d,%d>\n",MotorConfig[0].AMax,MotorConfig[1].AMax,MotorConfig[2].AMax,MotorConfig[3].AMax,MotorConfig[4].AMax,MotorConfig[5].AMax);
		return REPLY_OK;
	}
	return REPLY_INVALID_VALUE;
}
//
uint8_t  CommmandLine_rotate(char *Commands)
{
	if(strncmp(Commands+1,"# rotate ",9)==0 ) 
  {
		if( Commands[10]=='+' || Commands[10]=='-' )
		{
			CMDGetFromUart.Motor=Commands[0]-'0';
			TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
			TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
			if(CMDGetFromUart.Motor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[11];		
				u8 len;				
				TMC429_ParameterRW.Value.Int32=strtol(s, &p, 10);		
				len=p-s;				
				if(0<len&&len<=4)					
				{
					MotorConfig[CMDGetFromUart.Motor].SpeedChangedFlag=TRUE;
					Set429RampMode(TMC429_DEV,TMC429_motor, RM_VELOCITY);
									
					Write429Short(TMC429_DEV, IDX_VMAX|(TMC429_motor<<5), 2047);
					if( Commands[10]=='+' )
					{
						Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), TMC429_ParameterRW.Value.Int32);
					}
					else Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), -TMC429_ParameterRW.Value.Int32);

					rt_kprintf("\n<OK>\n");
					return REPLY_OK;
				}
			}
		}
		else return REPLY_INVALID_VALUE;		
	}
	return REPLY_INVALID_CMD;
}
//
uint8_t  CommmandLine_move(char *Commands)
{
/**   绝对运动
**					
**/		
	if(strncmp(Commands+1,"# move to ",10)==0 ) 
  {
		if( Commands[11]=='+' || Commands[11]=='-' )
		{
			CMDGetFromUart.Motor=Commands[0]-'0';
			TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
			TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
			if(CMDGetFromUart.Motor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[12];	
				u8 len;
				TMC429_ParameterRW.Value.Int32=(int)strtol(s, &p, 10);			
				len=p-s;				
				if(0<len&&len<=8)	
				{
					if(MotorConfig[CMDGetFromUart.Motor].SpeedChangedFlag)
					{
						Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), MotorConfig[CMDGetFromUart.Motor].VMax);
						SetAMax(TMC429_DEV,TMC429_motor, MotorConfig[CMDGetFromUart.Motor].AMax);
						MotorConfig[CMDGetFromUart.Motor].SpeedChangedFlag=FALSE;
					}
					Set429RampMode(TMC429_DEV,TMC429_motor, RM_VELOCITY);
					if( Commands[11]=='+' )
					{
						Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), TMC429_ParameterRW.Value.Int32);
					}
					else Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), -TMC429_ParameterRW.Value.Int32);
					
					Set429RampMode(TMC429_DEV,TMC429_motor, RM_RAMP);	
					rt_kprintf("\n<OK>\n");					
					return REPLY_OK;
				}
			}
		}
		else return REPLY_INVALID_VALUE;
	}
/**   相对运动
**					
**/		
	else	if(strncmp(Commands+1,"# move ",7)==0 ) 
  {
		if( Commands[8]=='+' || Commands[8]=='-' )
		{
			CMDGetFromUart.Motor=Commands[0]-'0';
			TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
			TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
			if(CMDGetFromUart.Motor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[9];	
				u8 len;
				TMC429_ParameterRW.Value.Int32=(int)strtol(s, &p, 10);							
				len=p-s;				
				if(0<len&&len<=8)	
				{
					if(MotorConfig[CMDGetFromUart.Motor].SpeedChangedFlag)
					{
						Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), MotorConfig[CMDGetFromUart.Motor].VMax);
						SetAMax(TMC429_DEV, TMC429_motor, MotorConfig[CMDGetFromUart.Motor].AMax);
						MotorConfig[CMDGetFromUart.Motor].SpeedChangedFlag=FALSE;
					}
					Set429RampMode(TMC429_DEV,TMC429_motor, RM_VELOCITY);
					if( Commands[8]=='+' )
					{
						Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5))+TMC429_ParameterRW.Value.Int32);
					}
					else Write429Int(TMC429_DEV,IDX_XTARGET|(TMC429_motor<<5), Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5))-TMC429_ParameterRW.Value.Int32);
					
					Set429RampMode(TMC429_DEV,CMDGetFromUart.Motor, RM_RAMP);	
					rt_kprintf("\n<OK>\n");	
					return REPLY_OK;
				}
			}
		}
		else return REPLY_INVALID_VALUE;
	}
/**   停止运动
**					
**/	
	else 	if(strncmp(Commands+1,"# stop",6)==0 ) 
  {
		CMDGetFromUart.Motor=Commands[0]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			HardStop(TMC429_DEV,TMC429_motor);
//			Set429RampMode(CMDGetFromUart.Motor, RM_VELOCITY);
//			Write429Zero(IDX_VTARGET|(CMDGetFromUart.Motor<<5));			
			rt_kprintf("\nSTOP[%d]<OK>\n",CMDGetFromUart.Motor);			
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}
	return REPLY_INVALID_CMD;
}
//
uint8_t  CommmandLine_pos(char *Commands)
{
	/**   读取位置
**					
**/	
	if(strcmp(Commands,"position #all")==0 ) 
	{
//		MotorConfig[0].XACTUAL=Read429Int(SPI_DEV0_TMC429,IDX_XACTUAL|(0<<5));
//		MotorConfig[1].XACTUAL=Read429Int(SPI_DEV0_TMC429,IDX_XACTUAL|(1<<5));
//		MotorConfig[2].XACTUAL=Read429Int(SPI_DEV0_TMC429,IDX_XACTUAL|(2<<5));
//		MotorConfig[3].XACTUAL=Read429Int(SPI_DEV1_TMC429,IDX_XACTUAL|(0<<5));
//		MotorConfig[4].XACTUAL=Read429Int(SPI_DEV1_TMC429,IDX_XACTUAL|(1<<5));
//		MotorConfig[5].XACTUAL=Read429Int(SPI_DEV1_TMC429,IDX_XACTUAL|(2<<5));
//		rt_kprintf("\nP<%d,%d,%d,%d,%d,%d>\n",MotorConfig[0].XACTUAL,MotorConfig[1].XACTUAL,MotorConfig[2].XACTUAL,MotorConfig[3].XACTUAL,MotorConfig[4].XACTUAL,MotorConfig[5].XACTUAL);
		//rt_kprintf("P[0]=%d,P[1]=%d,P[2]=%d\r\n",MotorConfig[0].XACTUAL,MotorConfig[1].XACTUAL,MotorConfig[2].XACTUAL);	
		return REPLY_OK;
	}
	else 	if(strncmp(Commands,"position #",10)==0 ) 
	{
		CMDGetFromUart.Motor=Commands[10]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_ParameterRW.Value.Int32=Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5));
			rt_kprintf("\nP[%d]<%d>\n",CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
			//rt_kprintf("P[%d]=%d\r\n",CMDGetFromUart.Motor,MotorConfig[CMDGetFromUart.Motor].XACTUAL);
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}
	else return REPLY_INVALID_CMD;
}
//
uint8_t  CommmandLine_limit(char *Commands)
{
/**   读取指定轴左极限状态
**					
**/	
	if(strncmp(Commands,"is left #",9)==0 ) 
	{
		CMDGetFromUart.Motor=Commands[9]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_ParameterRW.Value.Int32=(Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x02<<TMC429_motor*2)) ? 1:0;
			rt_kprintf("REFL[%d]<%d>\n",CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}
/**   读取指定轴右极限状态
**					
**/		
	else if(strncmp(Commands,"is right #",10)==0 ) 
	{
		CMDGetFromUart.Motor=Commands[10]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_ParameterRW.Value.Int32=(TMC429_DEV,Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x01<<TMC429_motor*2)) ? 1:0;
			rt_kprintf("\nREFR[%d]<%d>\n",CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}
/**   读取所有轴极限开关状态
**					
**/	
	else if(strcmp(Commands,"is limit")==0 ) 
	{
		u8 limit_char[12];u8 i;
		u8 limit = Read429SingleByte(SPI_DEV0_TMC429,  IDX_REF_SWITCHES, 3);
		for(i=0;i<6;i++)
		{
			limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
		}
		limit = Read429SingleByte(SPI_DEV1_TMC429,  IDX_REF_SWITCHES, 3);
		for(i=0;i<6;i++)
		{
			limit_char[i+6]=(limit&(0x20>>i)) ? '1':'0';
		}
		rt_kprintf("\nLIMIT<%.12s>\n",limit_char);
		//rt_kprintf("limit=%6s\r\n",limit_char);		
		return REPLY_OK;
	}
	else return REPLY_INVALID_VALUE;
}
//
uint8_t  CommmandLine_reach(char *Commands)
{
	/**   判断指定轴是否运动到位
**					
**/	
	if(strncmp(Commands,"is reach #",10)==0 ) 
	{
		CMDGetFromUart.Motor=Commands[10]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_ParameterRW.Value.Int32=(Read429Status(TMC429_DEV) & (0x01<<TMC429_motor*2)) ? 1:0;
			rt_kprintf("\nREACH[%d]<%d>\n",TMC429_ParameterRW.Value.Int32);
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}
/**   判断指定轴是否停止
**					
**/	
	else 	if(strncmp(Commands,"is stop #",9)==0 ) 
	{
		u8 Swith;//=Read429SingleByte(IDX_REF_SWITCHES, 3);
		CMDGetFromUart.Motor=Commands[9]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			if( Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0 )  TMC429_ParameterRW.Value.Int32=1;
			else TMC429_ParameterRW.Value.Int32=0;
			
			Swith=Read429SingleByte(SPI_DEV0_TMC429,IDX_REF_SWITCHES, 3);
			
			if((Swith&0x03)==0x03) TMC429_ParameterRW.Value.Int32=1;
			if((Swith&0x0c)==0x0c) TMC429_ParameterRW.Value.Int32=1;
			if((Swith&0x30)==0x30) TMC429_ParameterRW.Value.Int32=1;	
			
			Swith=Read429SingleByte(SPI_DEV1_TMC429,IDX_REF_SWITCHES, 3);
			if((Swith&0x03)==0x03) TMC429_ParameterRW.Value.Int32=1;
			if((Swith&0x0c)==0x0c) TMC429_ParameterRW.Value.Int32=1;
			if((Swith&0x30)==0x30) TMC429_ParameterRW.Value.Int32=1;	
			
			rt_kprintf("\nSTOP[%d]<%d>\n",CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;
	}							
	else return REPLY_INVALID_CMD;
}
//待完善
uint8_t  CommmandLine_home(char *Commands)
{
	if(strncmp(Commands+1,"# search home from left to right ",33)==0 ) 
  {
		CMDGetFromUart.Motor=Commands[0]-'0';
		TMC429_motor=getMotor0_2(CMDGetFromUart.Motor);
		TMC429_DEV=getTMC429_DEV(CMDGetFromUart.Motor);
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			char *p = NULL;
			char *s = &Commands[34];
			u8 len=p-s;					
			if(0<len&&len<=4)			
      {
				HomeInfo[CMDGetFromUart.Motor].HomeSpeed=strtol(s, &p, 10);	
				if(CMDGetFromUart.Motor==0)			Write429Int(TMC429_DEV,IDX_VMAX|MOTOR0, 2047);		
				else if(CMDGetFromUart.Motor==1)	Write429Int(TMC429_DEV,IDX_VMAX|MOTOR1, 2047);		
				else if(CMDGetFromUart.Motor==2)	Write429Int(TMC429_DEV,IDX_VMAX|MOTOR2, 2047);					
				else if(CMDGetFromUart.Motor==3)	Write429Int(TMC429_DEV,IDX_VMAX|MOTOR0, 2047);		
				else if(CMDGetFromUart.Motor==4)	Write429Int(TMC429_DEV,IDX_VMAX|MOTOR1, 2047);		
				else if(CMDGetFromUart.Motor==5)	Write429Int(TMC429_DEV,IDX_VMAX|MOTOR2, 2047);					

//				SetAmaxAuto(CMDGetFromUart.Motor, Homing_speed[CMDGetFromUart.Motor]);
//				RotateLeft(CMDGetFromUart.Motor,Homing_speed[CMDGetFromUart.Motor]);
				rt_kprintf("<%d# is homing>\n",CMDGetFromUart.Motor);
				return REPLY_OK;
			}
			else return REPLY_INVALID_VALUE;
    }
		else return REPLY_INVALID_VALUE;
	}
	else return REPLY_INVALID_CMD;	
}
//
uint8_t Command_analysis_motor(char *string)
{
//  CMDGetFromUart.Type=Type_none;
//	TMC429_ParameterGet.Status=REPLY_OK;
	
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"motor help") )		 					printf_cmdList_motor();
	else if( !strcmp(string,"motor get help") )		 	printf_cmdList_motor_get();
	else if( !strcmp(string,"motor set help") )		 	printf_cmdList_motor_set();
	
	else if(!CommmandLine_speed(string))			{;}
	else if(!CommmandLine_acc(string))				{;}
	else if(!CommmandLine_rotate(string))			{;}
	else if(!CommmandLine_move(string))				{;}
	else if(!CommmandLine_pos(string))				{;}
	else if(!CommmandLine_limit(string))			{;}
	else if(!CommmandLine_reach(string))			{;}		
	else if(!CommmandLine_home(string))				{;}		
			

	else if( !strcmp(string,"motor reset")) 				{motorsResetInOrder=TRUE;}//MotorAutoReset_preset();
	else if( !strcmp(string,"motor stop reset"))		 		
	{
		HomeInfo[0].GoHome=0;
		HomeInfo[1].GoHome=0;
		HomeInfo[2].GoHome=0;
		TMC429_MotorStop(0);TMC429_MotorStop(1);TMC429_MotorStop(2);	
		printf("motor stop reset and all motor is stop\r\n") ;		
	}
	else if( !strncmp(string,"motor stop ",11)) 		
	{

		char *p = NULL;
		char *s = &string[11];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		if(CMDGetFromUart.Motor<N_O_MOTORS)
		{
			TMC429_MotorStop(CMDGetFromUart.Motor);
			printf("motor[%d] is stop and P[%d]=%d\n",CMDGetFromUart.Motor,CMDGetFromUart.Motor,Read429Int(TMC429_DEV,IDX_XACTUAL|(CMDGetFromUart.Motor<<5)));			
		}

	}
	else if( !strncmp(string,"motor rotate ",13)) 
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0 )	
			{
				TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
			}
		}
	}
	/*motor move 2 -1000*/
	else if( !strncmp(string,"motor move ",11)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[11];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ' && CMDGetFromUart.Motor<N_O_MOTORS)
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
				TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32);
			}
		}
	}
	else if( !strncmp(string,"motor moveto ",13)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ' && CMDGetFromUart.Motor<N_O_MOTORS)
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{		
					TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_ABS, CMDGetFromUart.Value.Int32);
			}
		}
	}
	else if( !strncmp(string,"motor gohome ",13)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
					HomeInfo[CMDGetFromUart.Motor].GoHome = TRUE;
				  HomeInfo[CMDGetFromUart.Motor].GoLimit= FALSE;
					HomeInfo[CMDGetFromUart.Motor].Homed	= FALSE;

					HomeInfo[CMDGetFromUart.Motor].HomeSpeed=CMDGetFromUart.Value.Int32;	
				
					SetAmaxBySpeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
		
				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					
					printf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);
			}
		}
	}
	else if( !strncmp(string,"motor golimit ",14)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[14];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
				  HomeInfo[CMDGetFromUart.Motor].GoHome	=FALSE;
					HomeInfo[CMDGetFromUart.Motor].GoLimit=TRUE;
					HomeInfo[CMDGetFromUart.Motor].Homed	=FALSE;
					HomeInfo[CMDGetFromUart.Motor].HomeSpeed=CMDGetFromUart.Value.Int32;	
					SetAmaxBySpeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));

				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					printf("motor[%d] is start go home by searching limit sensor\n",CMDGetFromUart.Motor);

			}
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
//
uint8_t Command_analysis_motor_get(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"motor get help") )		 	printf_cmdList_motor_get();
	
	else if( !strncmp(string,"motor get ",10)) 		
	{
		char *string1 = &string[10];	
				
		if( !strncmp(string1,"speed ",6)) 		
		{
			char *p = NULL;
			char *s = &string1[6];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_actual);
		}
		else if( !strncmp(string1,"position ",9)) 		
		{
			char *p = NULL;
			char *s = &string1[9];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_actual);
		}
		else if( !strncmp(string1,"is_homed ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_homed);
		}	
		else if( !strncmp(string1,"is_stop ",8)) 		
		{
				char *p = NULL;
				char *s = &string1[8];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_stop);
		}		
		else if( !strncmp(string1,"is_reach ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_reach);
		}		
		else if( !strncmp(string1,"next_speed ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_next);
		}	
		else if( !strncmp(string1,"next_position ",14)) 		
		{
				char *p = NULL;
				char *s = &string1[14];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_next);
		}	
		else if( !strncmp(string1,"VMAX ",5)) 		
		{
				char *p = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_max);
		}	
		else if( !strncmp(string1,"AMAX ",5)) 		
		{
				char *p = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,acc_max);
		}	
		else if( !strncmp(string1,"position_reached ",17)) 		
		{
				char *p = NULL;
				char *s = &string1[17];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_reached);
		}	
		else if( !strncmp(string1,"rightLimit ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,rightLimit_SwS);
		}
		else if( !strncmp(string1,"leftLimit ",10)) 		
		{
				char *p = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,leftLimit_SwS);
		}
		else if( !strcmp(string1,"limit all")) 		
		{
			TMC429_GetAxisParameter(CMDGetFromUart.Motor,allLimit_SwS);
		}
		else if( !strncmp(string1,"homeSensor ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,home_SenS);
		}
		else if( !strncmp(string1,"ramp_div ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,ramp_divisor);
		}
		else if( !strncmp(string1,"pulse_div ",10)) 		
		{
				char *p = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,pulse_divisor);
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
//不需要配置负数
uint8_t Command_analysis_motor_set(char *string)
{
	uint8_t result = REPLY_OK;

	if( !strncmp(string,"motor set ",10)) 		
	{
		char *string1 = &string[10];	
		
		if( !strcmp(string1,"help") )		 	printf_cmdList_motor_set();		
		
		else if( !strncmp(string1,"speed ",6)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[6];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_max,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"next_speed ",11)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[11];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_next,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"next_position ",14)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[14];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,position_next,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"actual_position ",16)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[16];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,position_actual,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"VMAX ",5)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{	
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_max,CMDGetFromUart.Value.Int32);
				}
		}	
		else if( !strncmp(string1,"AMAX ",5)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{	
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,acc_max,CMDGetFromUart.Value.Int32);
				
				}
		}	
		else if( !strncmp(string1,"ramp_div ",9)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,ramp_divisor,CMDGetFromUart.Value.Int32);
				
				}
		}
		else if( !strncmp(string1,"pulse_div ",10)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,pulse_divisor,CMDGetFromUart.Value.Int32);
				
				}
		}
		else if( !strncmp(string1,"limitSignal ",12)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[12];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,limitSignal,CMDGetFromUart.Value.Int32);	
				}
		}
//		else if( !strncmp(string1,"actual_speed ",13)) 		
//		{
//				char *p = NULL;char *p1 = NULL;
//				char *s = &string1[13];	
//				CMDGetFromUart.Motor =strtol(s, &p, 10);
//				if(*p==' ')	
//				{
//					if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
//					else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
//					if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
//						TMC429_SetAxisParameter(CMDGetFromUart.Motor,actual_speed,CMDGetFromUart.Value.Int32);
//				}
//		}	
	}

	else result = REPLY_INVALID_CMD;
	return result;
}
//


