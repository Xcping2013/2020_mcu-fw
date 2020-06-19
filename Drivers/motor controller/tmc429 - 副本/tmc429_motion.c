/*  TMCL Instruction
1-ROR rotate right
2-ROL rotate left
3-MST Motor  stop
4-MVP move to position
5-SAP set axis parameter
6-GAP get axis parameter
7-STAP store axis parameter
8-RSAP restore axis parameter
9-SGP set global parameter
10-SGP get global parameter
11-STGP store global parameter
12-RSGP restore global parameter
13-REF reference search
14-SIO set output
15-GIO get input
*/
#include "tmc429_motion.h"
/*********************************************************************************************************************/
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
  uint8_t Status;               
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;

TMC429_Parameter TMC429_ParameterGet;
TMC429_Parameter TMC429_ParameterSet;

uint8_t OriginSensorPin[3]={PB_9,PE_0,PE_1};

static UCHAR SpeedChangedFlag[N_O_MOTORS];

static uint8_t MotorStopByLimit[N_O_MOTORS]={DISABLE,DISABLE,DISABLE};								//碰到限位处理一次
	
/*********************************************************************************************************************/

static uint8_t 	TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);

static void 		TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void 		TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);
static void 		MotorKeepStop_removeLimitSigal(uint8_t motor_number);

static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);

/*********************************************************************************************************************/
void TMC429_ParameterPresetToRam(void);

/***************************************************************************************************/
void TMC429_ParameterPresetToRam(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					
	{																  
		MotorConfig[i].VMax 		= g_tParam.tmc429_VMax[i];   				
		MotorConfig[i].AMax 		=	g_tParam.tmc429_AMax[i];
		MotorConfig[i].PulseDiv	=	g_tParam.tmc429_PulseDiv[i];	
		MotorConfig[i].RampDiv	=	g_tParam.tmc429_RampDiv[i];	
		
		SpeedChangedFlag[i]			=	TRUE;
		homeInfo.Homed[i]				=	FALSE;
		homeInfo.GoHome[i]			=	FALSE;
		homeInfo.GoLimit[i]			=	FALSE;
		homeInfo.HomeSpeed[i]		=	g_tParam.speed_home[i];
	}
}
static uint8_t TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
	uint8_t result = REPLY_OK;
	
  if(motor_number<N_O_MOTORS)
  {
    if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
    {
			//MotorStopByLimit[motor_number]=1;
      if(SpeedChangedFlag[motor_number])
      {
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
        SpeedChangedFlag[motor_number]=FALSE;
      }
      if(motion_mode==MVP_ABS)
			{
				rt_kprintf("motor[%d] is start to make absolute motion\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position);
			}
      else
			{
				rt_kprintf("motor[%d] is start to make relative motion\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position + Read429Int(IDX_XACTUAL|(motor_number<<5)));
			}
      Set429RampMode(motor_number, RM_RAMP);
    }
    else 
		{
			result = REPLY_WRONG_TYPE;
		}
  }
  else result = REPLY_INVALID_VALUE;
	
	return result;
}
/***************************************************************//**
  \fn GetAxisParameter(void)
  \brief Command GAP

  GAP (Get Axis Parameter) command (see TMCL manual).
********************************************************************/
static void TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type)
{
		
  TMC429_ParameterGet.Value.Int32=0;
	
  if(motor_number < N_O_MOTORS)	
  {
    switch(parameter_type)
    {
      case next_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XTARGET|(motor_number<<5));				
				rt_kprintf("motor[%d] next position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
				break;

      case actual_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XACTUAL|(motor_number<<5));
				rt_kprintf("motor[%d] actual position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case next_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VTARGET|(motor_number<<5));
			  rt_kprintf("motor[%d] next speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case actual_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VACTUAL|(motor_number<<5));
			  rt_kprintf("motor[%d] actual speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_v_positioning:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].VMax;
			  rt_kprintf("motor[%d] max positioning speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_acc:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].AMax;
			  rt_kprintf("motor[%d] max acceleration=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IRun;
				rt_kprintf("motor[%d] max current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case standby_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IStandby;
				rt_kprintf("motor[%d] standby current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case position_reached:
//        if(Read429Status() & 0x01) TMC429_ParameterGet.Value.Byte[0]=1;
//				rt_kprintf("motor[%d] position reached=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				rt_kprintf("motor[%d] position reached=%d\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);
        break;

      case leftLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<motor_number*2)) ? 1:0;
			  rt_kprintf("motor[%d] left limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case rightLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<motor_number*2)) ? 1:0;
			  rt_kprintf("motor[%d] right limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;
			
      case allLimit_SwS:
				{
						uint8_t limit_char[6];
						uint8_t i;
						uint8_t limit = Read429SingleByte(IDX_REF_SWITCHES, 3);
						for(i=0;i<6;i++)
						{
							limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
						}
						rt_kprintf("motor limit switch status L2R2L1R1L0R0=%.6s\n",limit_char);
				}
        break;

      case rightLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x02) ? 1:0;
			  rt_kprintf("motor[%d] right limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				break;

      case leftLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x01) ? 1:0;
				rt_kprintf("motor[%d] left limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case min_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VMIN|(motor_number<<5));
				rt_kprintf("motor[%d] minimum speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case ramp_mode:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 3);
			  rt_kprintf("motor[%d] ramp mode=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        TMC429_ParameterGet.Value.Int32=Read429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE);
				rt_kprintf("motor[%d] ref switch tolerance=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case softStop_flag:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, 0) & 0x04) ? 1:0;
				rt_kprintf("motor[%d] soft stop status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
			  break;

      case ramp_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) & 0x0f;
		  	rt_kprintf("motor[%d] ramp divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case pulse_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) >> 4;
				rt_kprintf("motor[%d] pulse divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

			case is_homed:
				rt_kprintf("motor[%d] homed=%d\n",motor_number,homeInfo.Homed[motor_number]);
				break;
			case is_stop:
				rt_kprintf("motor[%d] stop=%d\n",motor_number,Read429Short(IDX_VACTUAL|(motor_number<<5))? 0:1) ;
				break;
			case is_reach:

				rt_kprintf("motor[%d] reach=%d\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);

				break;
			case home_SenS:
				rt_kprintf("motor[%d] homeSensor=%d\n",motor_number, rt_pin_read(OriginSensorPin[motor_number]) ? 0:1) ;
				break;						
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
}



static void TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value)
{
  UCHAR Read[4], Write[4];
  if(motor_number < N_O_MOTORS)
  {
		TMC429_ParameterSet.Value.Int32=parameter_value;
		
    switch(parameter_type)
    {
      case next_position:
        Write429Int(IDX_XTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] next position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_position:
        Write429Int(IDX_XACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] actual position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case next_speed:
        Write429Short(IDX_VTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] next speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_speed:
        Write429Short(IDX_VACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] actual speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_v_positioning:
        MotorConfig[motor_number].VMax=TMC429_ParameterSet.Value.Int32;
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
				rt_kprintf("set motor[%d] speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_acc:
        MotorConfig[motor_number].AMax=TMC429_ParameterSet.Value.Int32;
        SetAMax(motor_number, MotorConfig[motor_number].AMax);
				rt_kprintf("set motor[%d] max acceleration speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case rightLimit_disable:	//12
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x02;
        else
          Write[2]=Read[2]&  ~0x02;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					rt_kprintf("motor[%d] right limit switch is disable\n",motor_number);	
				else 
					rt_kprintf("motor[%d] right limit switch is enable\n",motor_number);	
        break;

      case leftLimit_disable: //13
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x01;
        else
          Write[2]=Read[2]&  ~0x01;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					rt_kprintf("motor[%d] left limit switch is disable\n",motor_number);	
				else 
					rt_kprintf("motor[%d] left limit switch is enable\n",motor_number);	
        break;

      case min_speed:
        Write429Short(IDX_VMIN|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] minimum speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_mode:
        Set429RampMode(motor_number, TMC429_ParameterSet.Value.Byte[0]);
				rt_kprintf("set motor[%d] ramp mode=%d ok\n",motor_number,TMC429_ParameterSet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        Write429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE, TMC429_ParameterSet.Value.Int32);
				rt_kprintf("set motor[%d] ref switch tolerance=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
			  break;

      case softStop_flag:
        Read429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
        if(TMC429_ParameterSet.Value.Int32!=0)
          Read[1]|=0x04;
        else
          Read[1]&= ~0x04;
        Write429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
				rt_kprintf("set motor[%d] soft stop status=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (CMDGetFromUart.Value.Byte[0] & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].RampDiv=CMDGetFromUart.Value.Byte[0] & 0x0f;
				rt_kprintf("set motor[%d] ramp divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case pulse_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (CMDGetFromUart.Value.Byte[0] << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].PulseDiv=CMDGetFromUart.Value.Byte[0]& 0x0f;
				rt_kprintf("set motor[%d] pulse divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;
			case limitSignal:
				if(TMC429_ParameterSet.Value.Int32!=0) 
				{
				  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
				}
				else Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
				rt_kprintf("set limit signal effective trigger level=%d ok\n",TMC429_ParameterSet.Value.Int32);
				break;
				
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
}

/*
 对轴号和参数范围先进行判断	内部调用函数不再进行判断
		0		 		1       		2 							3					4						5
argc=2
  motor   reset			//封装一系列动作，需要提取项目号来触发，不建议应用
argc=3
	motor 	stop  		<axisNum> 
argc=4
	motor 	rotate 		<axisNum> 	<axis_speed>  
  motor 	move	 		<axisNum> 	<position> 
	motor 	moveto	 	<axisNum> 	<position> 
	motor 	get 			<axisNum> 	<type>  

  motor 	gohome 		<axisNum> 	<speed> 
	motor 	golimit		<axisNum> 	<speed> 
argc=5
  motor 	set 			<type> 			<axisNum> 				<value>  

  	
*/
static void	printf_cmdList_motor(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor stop <axisNum>                  - stop motor\n");		
		rt_kprintf("motor rotate <axisNum> <speed>        - rotate motor\n");	
		rt_kprintf("motor moveto <axisNum> <position>     - absolute motion of the motor\n");
		rt_kprintf("motor move <axisNum> <position>       - relative motion of the motor\n");

		rt_kprintf("motor get <type> <axisNum>            - get axis parameter\n");	
		rt_kprintf("motor set <type> <axisNum> <value>    - set axis parameter\n");	
		
		rt_kprintf("motor gohome <axisNum> <speed>        - home sensor as origin position\n");	
		rt_kprintf("motor golimit <axisNum> <speed>       - limit sensor as origin position\n");	
		
		rt_kprintf("motor reset                           - motor reset\n");
    rt_kprintf("motor stop reset                      - motor stop reset\n");	
	
		rt_kprintf("motor reset?                          - is axis reset or not\n");		
}
static void printf_cmdList_motor_set(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor set speed <axis> <value>             -set the rotate speed \n");				
		rt_kprintf("motor set next_position <axis> <value>      -set the target position to move\n");
		rt_kprintf("motor set actual_position <axis> <value>    -set the current position\n");				
		rt_kprintf("motor set actual_speed <axis> <value>       -set the current speed \n");
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
uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
  if(motor_number < N_O_MOTORS)
  {
		//MotorStopByLimit[motor_number]=1;	
    SpeedChangedFlag[motor_number]=TRUE;
		
    Set429RampMode(motor_number, RM_VELOCITY);
		
    Write429Short(IDX_VMAX|(motor_number<<5), 2047);
    Write429Short(IDX_VTARGET|(motor_number<<5), motor_speed);
		//rt_kprintf("motor%d is rotate at %d\n",motor_number,motor_speed);
		return 0;	
  }
	return 1;
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
uint8_t TMC429_MotorStop(uint8_t motor_number)
{
  if(motor_number < N_O_MOTORS)
  {	
		/*  立刻停止  */
		HardStop(motor_number);
		/* 回原点过程中响应停止命令的变量置位 */
		homeInfo.GoHome[motor_number]=0;
		homeInfo.GoLimit[motor_number]=0;
		/* 电机停止会自动返回当前位置 */
		
		return 0;
  }
	return 1;
}
void StopMotorByRamp(UINT Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
}
int motor(int argc, char **argv)
{
	  CMDGetFromUart.Type=Type_none;
	
		TMC429_ParameterGet.Status=REPLY_OK;
	
    int result = RT_ERROR;
	
		if (argc == 2 )
		{
			if (!strcmp(argv[1], "reset?"))
			{
				rt_kprintf("motor reset=%d\n",motorsReset_InOrder==OK_2 ? 1:0) ;
				return RT_EOK	; 				
			}
			else	if (!strcmp(argv[1], "reset"))
			{
				MotorAutoReset_preset();
				return RT_EOK	; 
			}
			else	if (!strcmp(argv[1], "set"))
			{
				printf_cmdList_motor_set();
				return RT_EOK	; 
			}
			else	if (!strcmp(argv[1], "get"))
			{
				printf_cmdList_motor_get();
				return RT_EOK	; 
			}
      else
			{		
				result =RT_ERROR;//REPLY_INVALID_CMD;
			}     
		}
		if (argc == 3)
		{
			CMDGetFromUart.Motor=atoi(argv[2]);
			if (!strcmp(argv[1], "stop"))
			{
				if (!strcmp(argv[2], "reset"))
				{
					stop_timer_5ms();
					buttonRESETpressed=FALSE;
					homeInfo.GoHome[0]=0;
					homeInfo.GoHome[1]=0;
					homeInfo.GoHome[2]=0;
					TMC429_MotorStop(0);TMC429_MotorStop(1);TMC429_MotorStop(2);return RT_EOK;
				}
				if(buttonRESETpressed!=FALSE || 	homeInfo.GoHome[2]==1) {rt_kprintf("motor is reseting\n");return RT_EOK;}
				if(CMDGetFromUart.Motor==AXIS_Z) 
				{	
					closeSerial();
				}				
				if(!TMC429_MotorStop(CMDGetFromUart.Motor))	//电机停止后自动会关闭串口打印压力
				{
//					homeInfo.GoHome[0]=FALSE;		homeInfo.GoHome[1]=FALSE;		homeInfo.GoHome[2]=FALSE;	
//					homeInfo.GoLimit[0]=FALSE;	homeInfo.GoLimit[1]=FALSE;	homeInfo.GoLimit[2]=FALSE;						
//					buttonRESETpressed=FALSE;								  //只要有轴停止啊，就可以开启下次复位功能，复位过去被停止的话	
					rt_kprintf("motor[%d] is stop and P[%d]=%d\n",CMDGetFromUart.Motor,CMDGetFromUart.Motor,Read429Int(IDX_XACTUAL|(CMDGetFromUart.Motor<<5)));
					
					return RT_EOK	;
				}
				result = RT_EINVAL;
			}
		}
		else if (argc == 4)
		{	
			CMDGetFromUart.Motor=atoi(argv[2]);
			CMDGetFromUart.Value.Int32=atoi(argv[3]);

			if (!strcmp(argv[1], "rotate"))
			{
				if(!(TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32)))	return RT_EOK	;
				result = RT_EINVAL;
			}
			if (!strcmp(argv[1], "move") )
			{
				if(buttonRESETpressed!=FALSE || homeInfo.GoHome[2]==1) {rt_kprintf("motor is reseting\n");return RT_EOK;}
				else 
				{
					if(!TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32))	return RT_EOK;
					result =RT_EINVAL;
				}
			}
			if (!strcmp(argv[1], "moveto"))
			{
				closeSerial();
				if(buttonRESETpressed!=FALSE || homeInfo.GoHome[2]==1) {rt_kprintf("motor is reseting\n");return RT_EOK;}
				else 
				{
					if(!TMC429_MoveToPosition(CMDGetFromUart.Motor, MVP_ABS, CMDGetFromUart.Value.Int32))	return RT_EOK;
					result =RT_EINVAL;
				}
			}
			if (!strcmp(argv[1], "gohome"))
			{
					closeSerial();
					//pressureAlarm=0;								//移除压力报警信号
				
				  //buttonRESETpressed=TRUE;			
					if(buttonRESETpressed!=FALSE) {rt_kprintf("motor is reseting\n");return RT_EOK;}		
					else
					{
					stop_timer_5ms();		
		
					homeInfo.GoHome[CMDGetFromUart.Motor] = TRUE;
				  homeInfo.GoLimit[CMDGetFromUart.Motor]= FALSE;
					homeInfo.Homed[CMDGetFromUart.Motor]	= FALSE;

					homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
				
					SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
		
				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					
					rt_kprintf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);
				
				  //电机回原点速度需要保存，下次复位按键需要调用，或者按键默认速度
				
					start_timer_5ms();
				
					return RT_EOK	;
				}
			}
			if (!strcmp(argv[1], "golimit"))
			{
					//pressureAlarm=0;
				
					stop_timer_5ms();
				
				  homeInfo.GoHome[CMDGetFromUart.Motor]	=FALSE;
					homeInfo.GoLimit[CMDGetFromUart.Motor]=TRUE;
					homeInfo.Homed[CMDGetFromUart.Motor]	=FALSE;
					homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
					SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));

				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					start_timer_5ms();
				
					return RT_EOK	;
			}
			else if (!strcmp(argv[1], "get"))
			{
				if (!strcmp(argv[2], "speed")) 	 					CMDGetFromUart.Type=actual_speed;
				if (!strcmp(argv[2], "position")) 				CMDGetFromUart.Type=actual_position;
				if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart.Type=is_homed;
				if (!strcmp(argv[2], "is_stop")) 					CMDGetFromUart.Type=is_stop;
				if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart.Type=is_reach;
			
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=next_speed;
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart.Type=next_position;		
				if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=max_acc;
				if (!strcmp(argv[2], "position_reached")) CMDGetFromUart.Type=position_reached;		
				
				if (!strcmp(argv[2], "rightLimit")) 			CMDGetFromUart.Type=rightLimit_SwS;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_SwS;
				if (!strcmp(argv[2], "limit")) 			      CMDGetFromUart.Type=allLimit_SwS;
				
				if (!strcmp(argv[2], "homeSensor")) 			CMDGetFromUart.Type=home_SenS;
				
				if (!strcmp(argv[2], "rightLimit?"))			CMDGetFromUart.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit?")) 			CMDGetFromUart.Type=leftLimit_disable;
				
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;	
				
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart.Type=standby_current;
//				if (!strcmp(argv[2], "Llimit_off")) 			CMDGetFromUart.Type=leftLimit_disable;
				
				if(CMDGetFromUart.Type!=Type_none)
				{
					CMDGetFromUart.Motor=atoi(argv[3]);
					TMC429_GetAxisParameter(CMDGetFromUart.Motor,CMDGetFromUart.Type);
					if(TMC429_ParameterGet.Status==REPLY_OK) return RT_EOK;
					result=RT_EINVAL;
				}
				else 
				{
					printf_cmdList_motor_get();
					result =REPLY_INVALID_CMD;
				}
			}
		}
		else if (argc == 5)
		{
			if (!strcmp(argv[1], "set"))
			{
				//user
				if (!strcmp(argv[2], "speed")) 					 	CMDGetFromUart.Type=max_v_positioning;
					
				//debug
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=next_speed;		
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart.Type=next_position;
				if (!strcmp(argv[2], "actual_position")) 	CMDGetFromUart.Type=actual_position;
				if (!strcmp(argv[2], "actual_speed")) 	 	CMDGetFromUart.Type=actual_speed;
				if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=max_acc;
				//设置限位失能无效，需要调试原因
				if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_disable;	  		
				if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart.Type=limitSignal;		
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart.Type=standby_current;
				if(CMDGetFromUart.Type!=Type_none)	
				{
					CMDGetFromUart.Motor=atoi(argv[3]);
					CMDGetFromUart.Value.Int32=atoi(argv[4]);
					TMC429_SetAxisParameter(CMDGetFromUart.Motor, CMDGetFromUart.Type, CMDGetFromUart.Value.Int32);
					if(TMC429_ParameterGet.Status==REPLY_OK) return RT_EOK;
					else result =RT_EINVAL;
				}
				else 
				{
					printf_cmdList_motor_set();
					result =REPLY_INVALID_CMD;														
				}				
			}

		}		
		if(	result == RT_EINVAL )
		{
			rt_kprintf("motor number must be 0~2\n",CMDGetFromUart.Motor);
		}
		else if( result == RT_ERROR )
		{
			printf_cmdList_motor();
		}
	  return result;
}

//
static rt_uint8_t motion_stack[ 512 ];
static struct rt_thread motion_thread;
static void motion_thread_entry(void *parameter);
//////////////////////////////////////////////////////////
static void MotorKeepStop_removeLimitSigal(uint8_t motor_number)
{
	if(homeInfo.GoHome[motor_number]==FALSE && homeInfo.GoLimit[motor_number]==FALSE)	//不是回原点动作
	{	
		if(MotorStopByLimit[motor_number]==1)
		{
			uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);				//限位状态读取
			//rt_kprintf("%d GoHome=%d,GoLimit=%d,SwitchStatus=%d,V=%d\n",motor_number,homeInfo.GoHome[motor_number],homeInfo.GoLimit[motor_number],SwitchStatus,Read429Short(IDX_VACTUAL|(motor_number<<5)));

			//左				需要再判断速度													右              //碰到限位停止，需要再次发命令运动
			if(((((SwitchStatus& (0x02<<motor_number*2))?1:0) ==1)	||  (((SwitchStatus& (0x01<<motor_number*2)) ?1:0)==1))	\
				   && ( Read429Short(IDX_VACTUAL|(motor_number<<5))==0 )	)						 
			{					
					
					TMC429_MotorStop(motor_number);		MotorStopByLimit[motor_number]=0;
					//rt_kprintf("MotorStop(%d) is act\n",axisNum);
			}		
		}
	}
}

static void motion_thread_entry(void *parameter)
{
	  static uint8_t cnt_delay=0;
    while (1)
    {  
//			for(uint8_t i=0;i<N_O_MOTORS;i++)
//			{			
//				MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
//				MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
//			}	
			if(cnt_delay++>5) 
			{
				for(uint8_t i=0;i<N_O_MOTORS;i++)	
				{
					MotorKeepStop_removeLimitSigal(i);
					cnt_delay=0;
				}
			}

			rt_thread_delay(10);
		}		
}
int MotorLimitCheck_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&motion_thread,
                            "motion",
                            motion_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&motion_stack[0],
                            sizeof(motion_stack),
                            25,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&motion_thread);
    }
    return 0;
}

//

void SetAmaxAutoByspeed(u8 axisNum,int speed)
{
	//16MHZ|1600steps PulseDiv=6 //speed=105->0.25/s  15.02/m 2047=4.88/S  292.83/m
	//16MHZ|1600steps PulseDiv=7 //speed=210->0.25/s  15.02/m 2047=2.44/S  146.41/m
	if(MotorConfig[axisNum].PulseDiv==6 &&	MotorConfig[axisNum].RampDiv==8)
	{
		//按项目进行速度设置 0.5mm/S为最小速度  其他速度都大于此设置值
		if (speed==105)										MotorConfig[axisNum].AMax=275;			//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=10;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed+10;	
		else if (105<speed && speed<210)	MotorConfig[axisNum].AMax=speed*2;	
		else if (209<speed && speed<400)	MotorConfig[axisNum].AMax=1000;	
		else 															MotorConfig[axisNum].AMax=2047;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed==70)										MotorConfig[axisNum].AMax=200;			//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=speed;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed*2;	
		else if (104<speed && speed<210)	MotorConfig[axisNum].AMax=speed*3;		
		else 															MotorConfig[axisNum].AMax=2000;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==7&&	MotorConfig[axisNum].RampDiv==10)
	{
		if (speed<25)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}			
		else if (speed<100)
		{		SetAMax(axisNum, speed+10);	MotorConfig[axisNum].AMax=speed+10;}
		else if (speed<150)
		{		SetAMax(axisNum, speed+30);MotorConfig[axisNum].AMax=speed+30;}
		else if (speed<280)
		{		SetAMax(axisNum, speed+60);MotorConfig[axisNum].AMax=speed+60;}
		else if (speed<420)
		{		SetAMax(axisNum, speed+100);MotorConfig[axisNum].AMax=speed+100;}
		else if (speed<560)
		{		SetAMax(axisNum, speed+150);MotorConfig[axisNum].AMax=speed+150;}
		else if (speed<700)
		{		SetAMax(axisNum, speed+200);MotorConfig[axisNum].AMax=speed+200;}
		else if (speed<840)
		{		SetAMax(axisNum, speed+250);MotorConfig[axisNum].AMax=speed+250;}
		else if (speed<980)
		{		SetAMax(axisNum, speed+300);MotorConfig[axisNum].AMax=speed+300;}
		else if (speed<=1180)
		{		SetAMax(axisNum, speed+350);MotorConfig[axisNum].AMax=speed+350;}
		else if (speed<2047)
		{		SetAMax(axisNum, speed+600);MotorConfig[axisNum].AMax=speed+600;}	
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed<25)	//1~24
		{		SetAMax(axisNum, speed);	MotorConfig[axisNum].AMax=speed;}			
		else if (speed<50)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}
		else if (speed<100)
		{		SetAMax(axisNum, 100);	MotorConfig[axisNum].AMax=100;}
		else if (speed<150)
		{		SetAMax(axisNum, 500);MotorConfig[axisNum].AMax=500;}
		else if (speed<250)
		{		SetAMax(axisNum, 1000);MotorConfig[axisNum].AMax=1000;}
		else if (speed<2047)
		{		SetAMax(axisNum, 2047);MotorConfig[axisNum].AMax=2047;}	
	}
}
//
MSH_CMD_EXPORT(motor, control motor motion by commands);

