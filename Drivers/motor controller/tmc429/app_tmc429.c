/*----------------------------------------Application note-------------------------------------------
主要配置：
	1.SPI设置	SPI模式3	SPI速率<=pdf	SPI_CS_PIN
	2.TMC429设置	
		Write429Datagram(Which429, IDX_SMGP, 0x00, 0x00->0x04, 0x02);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (motorSetting.PulseDiv[0]<<4)|(motorSetting.RampDiv[0] & 0x0f), 0x06->0x04);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR0, 0x00, NO_REF->HARD_REF, 0x00);

	需要重新整理下TMC429 晶振对应的速度和加速度关系
	
	运动说明：复位状态下未屏蔽电机运行控制 需要等待复位完成
	
	20200106 V1	使用模块封装电机控制模块;	暂时只支持6轴命令
							电机速度单位改为脉冲数;配置的速度分频和加速度分频为固定
							
	20200525 V2 更新速度加速度配置,可匹配不一样的晶振值,适应宽范围电机运行速度
	
	issue: 
	  
		 速度和位置变量溢出 16000000
		 
	底层配置命令再次完善	 
		 
	motor gohome HightSpeed1 LowSpeed2
	
	电机的基本配置与读取;基本运动；回远点运动，此状态需屏蔽电机其他运动命令
	
	20200619:更新速度加速度配置，配置为默认的1微步 200步 一圈的应用
	
	需要增加轴报警停止标志
							
*/
/*						BUG

20200701: >!	电机旋转速度不对,发送旋转命令 
							--->ram_pdiv被设置为了默认值0 :	TMC429_SetAxisParameter ->	TMC429_ParameterRW -> CMDGetFromUart
							--->MB1616DEV6的SPI3->SPI1 IO配置更新
							Vmax=2000->pulse speed=1907 只要有进行旋转运行,读取的pulse=1952都对应的Vmax=2047
				
					>!	找原点动作进行时,触发反相限位也会有动作;影响不大,正方向运行却触发错限位传感器本身硬件就已经异常
	
*/
/**************************************************************************/
#include "mbtmc429_spi.h"	
//#include "mbtmc429_timer_pwm.h"
#include "app_tmc429.h"	
#include "app_eeprom_24xx.h"

#if 1   //DBG_ENABLE
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
	#endif
#endif
   
/***************************************************************************************/
#define EEPROM_TMC429_PAR_ADDR		113 
#define EEPROM_TMC429_PAR_LEN			2 
//#define DATA_SAVED							'A'

#define MVP_ABS   0            
#define MVP_REL   1  

#define MOTOR_SENSOR_THREAD_PRIORITY	14
/**************************************Origin Sensor Config*****************************/
/***************************************************************************************/
#if 0

//uint8_t homeSensorPin[3]={PB_9,PE_0,PE_1};//IN5-IN6-IN7

uint8_t OriginSensorPin[6]=
{	
	PB_9,PE_0,PE_1,
	PB_9,PE_0,PE_1
};
uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};
#endif
/**************************************Origin Sensor Config*****************************/
/***************************************************************************************/
enum axisParameter {

position_next, 
position_actual, 
position_reached,	
	
speed_next, 				//int
speed_actual,				//int 
speed_pulse,
speed_max, 					//int
	v_max,
acc_max, 					//int

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

volatile uint8_t TMC429_DEV=SPI_DEV0_TMC429;
uint8_t  TMC429_motor=0;
int  motor_usteps=200*1;  
/***************************************************************************************/
typedef struct
{
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} 
TMC429_Command;
typedef struct
{           
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} 
TMC429_Parameter;
//
TMC429_Command CMDGetFromUart;
TMC429_Parameter TMC429_ParameterRW;

TMC429_PARAM_T motorSetting;	
motorHoming_t motorHoming;

static uint8_t getMotor0_2(uint8_t motor_number);
//static uint8_t getTMC429_DEV(uint8_t motor_number);
static void TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);
static void SetAmaxBySpeed(u8 mode,u8 motor_number , int32_t speed);	//输入的速度为pulse/s

int motor(int argc, char **argv);
int MotorSensor_thread_init(void);
/*************************************     tmc429_hw_init	    ************************************************
速度以200一圈，无微步来配置	
需要确保每个项目上使用传感器和硬件上保持一致，不再变动 ！！！	//传感器限位和原点触发电平命令着重说明
****************************************************************************************************/
#if 0
__weak const float Vmax_factor = 1.43051147;			// 1 /( 0.429153/60*200)		
__weak const float Amax_factor_slow = 0.125;			//800/80/80
__weak const float Amax_factor_fast = 0.0056689; //250/210/210

__weak uint8_t OriginSensorPin[6]=
{	
	PB_9,PE_0,PE_1,
	PB_9,PE_0,PE_1
};
__weak uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};

__weak void tmc429_clk_init(void)
{
	
}
__weak void TMC429_DefaulSetting(void)	
{
  uint8_t i;
		
	for(i=0; i<N_O_MOTORS; i++) 					
	{		
		pinMode(OriginSensorPin[i],PIN_MODE_INPUT_PULLUP);
		
		motorSetting.saved=DATA_SAVED;
		
		motorSetting.limit_level_valid=1;
		motorSetting.orgin_level_valid=0;
		
		motorSetting.SpeedChangedFlag[i]= TRUE;		
		
		motorSetting.VMax[i]=840;
		motorSetting.AMax[i]=1000;
		
		motorSetting.PulseDiv[i]=8;
		motorSetting.RampDiv[i]=7;
		
		motorHoming.Homed[i]				=	FALSE;
		motorHoming.GoHome[i]				=	FALSE;
		motorHoming.GoLimit[i]			=	FALSE;
		motorHoming.HomeSpeed[i]		=	2000;
	}
}
//
#endif
#if 1
void LoadSetting_FromEeprom(void)
{
	
	uint8_t readWrite=0;
	
	at24cxx.read(at24c256 , (EEPROM_TMC429_PAR_ADDR)*EEPROM_PAGE_BYTES, (uint8_t *)&readWrite, 1);		
	if(readWrite==DATA_SAVED)
	{
		at24cxx.read(at24c256 , (EEPROM_TMC429_PAR_ADDR)*EEPROM_PAGE_BYTES, (uint8_t *)&motorSetting, sizeof(TMC429_PARAM_T));		
	}	
	else 
	{
		TMC429_DefaulSetting();
		at24cxx.write(at24c256 , (EEPROM_TMC429_PAR_ADDR)*EEPROM_PAGE_BYTES, (uint8_t *)&motorSetting, sizeof(TMC429_PARAM_T));			
	}
}
//默认开限位,不需要限位的话 需要发送屏蔽命令
void Init429(UCHAR Which429 )
{
  UINT addr;
  UCHAR Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write429Zero(Which429, addr|(Motor<<5));
  }
	//正常应用 默认接传感器的时候 未触发 传感器亮 低电平未限位  
	if(motorSetting.limit_level_valid==0)
	{
		Write429Int(Which429, IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
	}
	else 
	{
		Write429Int(Which429, IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
	}
	//限位不接的时候 高电平 未触发 未限位
	//Write429Int(Which429, IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_SDO_INT|IFCONF_INV_DIR | IFCONF_INV_REF|IFCONF_EN_REFR);

	Write429Datagram(Which429, IDX_SMGP, 0x00, 0x04, 0x02);

	if(Which429==SPI_DEV0_TMC429)
	{
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (motorSetting.PulseDiv[0]<<4)|(motorSetting.RampDiv[0] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (motorSetting.PulseDiv[1]<<4)|(motorSetting.RampDiv[1] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (motorSetting.PulseDiv[2]<<4)|(motorSetting.RampDiv[2] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
	}
	else if(Which429==SPI_DEV1_TMC429)
	{
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (motorSetting.PulseDiv[3]<<4)|(motorSetting.RampDiv[3] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (motorSetting.PulseDiv[4]<<4)|(motorSetting.RampDiv[4] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (motorSetting.PulseDiv[5]<<4)|(motorSetting.RampDiv[5] & 0x0f), 0x04);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
		Write429Datagram(Which429, IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
	}
  Write429Short(Which429, IDX_VMIN|MOTOR0, 1);
  Write429Short(Which429, IDX_VMIN|MOTOR1, 1);
  Write429Short(Which429, IDX_VMIN|MOTOR2, 1);

	if(Which429==SPI_DEV0_TMC429)
	{
		Write429Int(Which429, IDX_VMAX|MOTOR0, motorSetting.VMax[0]);
		SetAMax(Which429, 0, motorSetting.AMax[0]);
		Write429Int(Which429, IDX_VMAX|MOTOR1, motorSetting.VMax[1]);
		SetAMax(Which429, 1, motorSetting.AMax[1]);
		Write429Int(Which429, IDX_VMAX|MOTOR2, motorSetting.VMax[2]);
		SetAMax(Which429, 2, motorSetting.AMax[2]);
	}
	else if(Which429==SPI_DEV1_TMC429)
	{
		Write429Int(Which429, IDX_VMAX|MOTOR0, motorSetting.VMax[3]);
		SetAMax(Which429, 0, motorSetting.AMax[3]);
		Write429Int(Which429, IDX_VMAX|MOTOR1, motorSetting.VMax[4]);
		SetAMax(Which429, 1, motorSetting.AMax[4]);
		Write429Int(Which429, IDX_VMAX|MOTOR2, motorSetting.VMax[5]);
		SetAMax(Which429, 2, motorSetting.AMax[5]);
	}
}
void tmc429_hw_init(void)
{	
	TMC429_DefaulSetting();

	pinMode(SPI_DEV0_CSPin,PIN_MODE_OUTPUT);
	pinSet(SPI_DEV0_CSPin);	
	pinMode(SPI_DEV1_CSPin,PIN_MODE_OUTPUT);
	pinSet(SPI_DEV1_CSPin);	

	tmc429_clk_init();
	
	bsp_spi_hw_init();

	Init429(SPI_DEV0_TMC429);
	Init429(SPI_DEV1_TMC429);
	
	if((Read429SingleByte(SPI_DEV0_TMC429,IDX_PULSEDIV_RAMPDIV|(0<<5), 2) & 0x0f )== (motorSetting.RampDiv[0]))
	{
		rt_kprintf("spi dev0 tmc429 [found]\n");
	}
	else DBG_TRACE("spi dev0 tmc429 [no found]\n");
	
	if((Read429SingleByte(SPI_DEV1_TMC429,IDX_PULSEDIV_RAMPDIV|(0<<5), 2) & 0x0f )== (motorSetting.RampDiv[3]))
	{
		rt_kprintf("spi dev1 tmc429 [found]\n");
	}
	else DBG_TRACE("spi dev1 tmc429 [no found]\n");

	MotorSensor_thread_init();
}
#endif

/*************************************     tmc429_hw_init	    ************************************************/
/*************************************************************************************************************/
/***********************************Speed and acceleration configuration**************************************/
#if 1
enum speedLevel{
/*
ustep=1 200steps per turn
之分低速  中速 高速三种  不搞太多详细档位

	实际应用使用MaxRPM_585 高速档位 需要低速时使用驱动器微步调节
*/
MaxRPM_60,	 // !<= 60/60  1R/S= 200 pulse/s
MaxRPM_300,	 //	!<= 300/60 5R/S= 1000Pulse/S
MaxRPM_878,	 // !<= 585/60 9.75R/S=1950PULSE/S
	
};

uint8_t MaxRPM_Cs=MaxRPM_878;
uint8_t MaxRPM_Changed=0;
uint8_t AMaxLevel=255;
void motorSetVel(uint8_t which_motor, double velocity)	//设置目标速度。 单位： pulse/ms pulse/rev
{
	/*button: Vneed=0.5mm/s  
	driver TR22G-D: 1.on-one pulse  2.on  3.on 3 --39% 4.on  5.off  6.off ---8ustep
	step motor : 200steps per turn		2mm/rev   Vneed=0.5mm/s=0.5/2(rev_s= *60=15r/min) *1600=400pulse/s
	tmc429 set: Vmax≈2047=10*Vneed=146.41 PULS_DEV=7 RAMP_DIV=7
	VMax=210=15.02RPM=400.54PULSE≈400PULSE 所有速度以此为参考	VMaxUser≈140PRM=140/15*400
	VMaxSet=velocity/400*210
	AMaxSet=SelectAMaxBySpeed(VMaxSet);
	*/
	//速度会有误差 位移运动准确便好
}

/*TMC429_ramp_setup.xls

TMC429_CLK=12MHZ |	200steps_per_turn	|	1usteps 							Vmax_factor=pulsePerSecond/VmaxREG
PULS_DIV	VMAX		RPM					RPS								VMAX=1	        1PULSE					AMAX
7					2047		878.4			878.4/60				 	 0.429153				2.330168*1
8					2047		878.4/2		878.4/60/2				 0.429153/2			2.330168*2
9					210			22.530555										 0.429153/4			2.330168*4
10				210			22.530555/2									 0.429153/8			2.330168*8
-----------------------------------------------------------------------------
(1ustep PULS_DIV_7) = (8ustep PULS_DIV_10) 
Vmax_factor:	(1ustep PULS_DIV_8) = 2*(8ustep PULS_DIV_7)  
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
8usteps  1600steps per turn
PULS_DIV	VMAX		RPM				RPS					PPS							VMAX=210	      	1PULSE
7					2047		146.41		146.41/60		146.41/60*1600	15.02RPM=400.54P  210/400.54

PULS_DIV=8
RAMP_DIV=7
VMAX	800  700	600  500	400 300	 200  100
AMX		2000 1520	1120 780	500	280	 125	30

AMax1=VMax1*VMax1*AMax0/(VMax0*VMax0)	//忘记怎么计算得到的 以前缺少注释
-----------------------------------------------------------------------------
*/
int32_t ChangeSpeedPerSecondToVMax(int32_t pulsePerSecond)  
{
	float VmaxREG;
	
	VmaxREG= 1.0*pulsePerSecond/Vmax_factor;

	if(VmaxREG>2000)		VmaxREG=2000;
	if(VmaxREG<(-2000))	VmaxREG=(-2000);
//	if(0<VmaxREG && VmaxREG<5)				VmaxREG=5;
	
	DBG_TRACE("VmaxREG=%d\n",(int32_t)VmaxREG);
	return (int32_t)VmaxREG;
}
int32_t ChangeVMaxToSpeed_ms(int32_t VMaxReg)  						//注意正负负号的处理
{		
	float Speed_temp;
	
	Speed_temp=1.0*VMaxReg*Vmax_factor;
	return	(int32_t)Speed_temp;
	//(VMaxReg) *(40054/21)
}
//速度配置一次PULS_DIV就行，加速度配置需要根据速度进行配置适当的RAMP_DIV
//放大PULS_DIV  速度越慢  但是每个寄存器数字量对应的脉冲越精确 
void SelectAMaxByVMax(uint8_t which_motor, uint32_t speed_Vmax)  //TMC429 12MHZ
{
	float AMaxTemp;
//	speed=speed*210/400000;	//命令输入的是脉冲数 不再是TMC429内部寄存器值	

	if(speed_Vmax<100)
	{
		if(AMaxLevel!=0)
		{	
			AMaxLevel=0;
			TMC429_SetAxisParameter(which_motor, ramp_divisor, 12);
		}		
		AMaxTemp=1.0*speed_Vmax*speed_Vmax*Amax_factor_slow;	
		if(AMaxTemp<8) AMaxTemp=8;
	}	
	else if(speed_Vmax<=2047)
	{
		if(AMaxLevel!=1)
		{	
			AMaxLevel=1;
			TMC429_SetAxisParameter(which_motor, ramp_divisor, 7);
		}	
		AMaxTemp=1.0*speed_Vmax*speed_Vmax*Amax_factor_fast;			
	}	
	if(AMaxTemp>2047) AMaxTemp=2047;

//AMaxTemp=(MotorConfig[which_motor].VMax*MotorConfig[which_motor].VMax*240)/(210*210);		 
		motorSetting.AMax[which_motor]=(uint16_t)AMaxTemp;		
}
static void SetAmaxBySpeed(u8 mode, u8 motor_number , int32_t speed)	//命令输入的是脉冲数 不再是TMC429内部寄存器值
{		
	if(mode)
	{
		SelectAMaxByVMax(motor_number,ChangeSpeedPerSecondToVMax(speed));
	}
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	SetAMax(TMC429_DEV,TMC429_motor,  motorSetting.AMax[motor_number]);
}
//
#endif
/***********************************Speed and acceleration configuration**************************************/
/*************************************************************************************************************/
/*******************************************TMC429 motion RWAxisParameter*******************************
Rotate |	MoveToPosition	|	StopMotor	|	SetAmaxBySpeed	| 	GetAxisParameter	SetAxisParameter
********************************************TMC429 motion*****************************/
#if 1
uint8_t motionLocked[N_O_MOTORS]={0,0,0,0,0,0};

static void TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
static void TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);
static void StopMotorByHW(uint8_t motor_number);
static void StopMotorByLimit(uint8_t motor_number);
//void SetAmaxBySpeed(u8 motor_number , int speed);	//输入的速度为寄存器值
/*********************************************************************************************************************/
static uint8_t getMotor0_2(uint8_t motor_number)
{
	return (motor_number<3 ? motor_number:(motor_number-3)) ;	
}
uint8_t getTMC429_DEV(uint8_t motor_number)
{
	if(motor_number<3)
	{	
		return SPI_DEV0_TMC429;	
	}
	return SPI_DEV1_TMC429; 
}

static void TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	motorSetting.SpeedChangedFlag[motor_number]=TRUE;
	
	Set429RampMode(TMC429_DEV,	TMC429_motor, RM_VELOCITY);

	//需要重新配加速度
	SetAmaxBySpeed(1,motor_number,abs(motor_speed));	
	
	Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), 2047);
	
	Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), ChangeSpeedPerSecondToVMax(motor_speed));

	//rt_kprintf("motor%d is rotate at vmax= %d\n",motor_number,motor_speed);
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
static void StopMotorByHW(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	HardStop(TMC429_DEV,TMC429_motor);
	//StopMotorByRamp(motor_number);
	/* 回原点过程中响应停止命令的变量置位 */
	motorHoming.GoHome[motor_number]=0;
	motorHoming.GoLimit[motor_number]=0;
	/* 电机停止会自动返回当前位置 */
	rt_kprintf("motor[%d] is stop and P[%d]=%d\n",motor_number,motor_number,Read429Int(TMC429_DEV,IDX_XACTUAL|(TMC429_motor<<5)));
}
static void StopMotorByRamp(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	motorHoming.GoHome[motor_number]=0;
	motorHoming.GoLimit[motor_number]=0;
	
  Set429RampMode(TMC429_DEV,MOTOR_NUMBER(TMC429_motor), RM_VELOCITY);
  Write429Zero(TMC429_DEV,(MOTOR_NUMBER(TMC429_motor)<<5)|IDX_VTARGET);
}
static void StopMotorByLimit(uint8_t motor_number)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
		
  Set429RampMode(TMC429_DEV,MOTOR_NUMBER(TMC429_motor), RM_VELOCITY);
  Write429Zero(TMC429_DEV,(MOTOR_NUMBER(TMC429_motor)<<5)|IDX_VTARGET);
}
/***************************************************************//**
  \fn MoveToPosition(void)
  \brief Command MVP

  MVP (Move To Position) command (see TMCL manual).

motor move X -Y 电机后端往前看 逆时针选择， 负限位
motor move X  Y 电机后端往前看 顺时针旋转， 右限位
********************************************************************/
static void TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);
	
	if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
	{		
		if(motorSetting.SpeedChangedFlag[motor_number])
		{
			Write429Short(TMC429_DEV, IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[motor_number]);
			
			SetAmaxBySpeed(0,motor_number, motorSetting.VMax[motor_number]);
			
			motorSetting.SpeedChangedFlag[motor_number]=FALSE;
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

		case speed_pulse:
			TMC429_ParameterRW.Value.Int32=Read429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5));
			rt_kprintf("motor[%d] speed_pulse=%d\n",motor_number,ChangeVMaxToSpeed_ms(TMC429_ParameterRW.Value.Int32));
			break;

		case speed_max:
			TMC429_ParameterRW.Value.Int32=motorSetting.VMax[motor_number];
			rt_kprintf("motor[%d] max positioning speed=%d\n",motor_number,ChangeVMaxToSpeed_ms(TMC429_ParameterRW.Value.Int32));
			break;

		case v_max:
			TMC429_ParameterRW.Value.Int32=Read429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5));
			rt_kprintf("motor[%d] IDX_VMAX=%d\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;
				
		case acc_max:
			TMC429_ParameterRW.Value.Int32=motorSetting.AMax[motor_number];
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
					uint8_t limit = Read429SingleByte(SPI_DEV1_TMC429,IDX_REF_SWITCHES, 3);
					for(i=0;i<6;i++)
					{
						limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
					}
					limit = Read429SingleByte(SPI_DEV0_TMC429,IDX_REF_SWITCHES, 3);
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
			rt_kprintf("motor[%d] homed=%d\n",motor_number,motorHoming.Homed[motor_number]);
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
				Write429Short(TMC429_DEV,IDX_VTARGET|(TMC429_motor<<5), ChangeSpeedPerSecondToVMax(TMC429_ParameterRW.Value.Int32));
				
				rt_kprintf("set motor[%d] next speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			}
			break;

		case speed_actual:
			{			
				Write429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5), ChangeSpeedPerSecondToVMax(TMC429_ParameterRW.Value.Int32));
			
				rt_kprintf("set motor[%d] actual speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);	
			}
			break;

		case speed_max:
			{			
				motorSetting.VMax[motor_number]=ChangeSpeedPerSecondToVMax(TMC429_ParameterRW.Value.Int32);
				Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[motor_number]);
				SetAmaxBySpeed(0,motor_number,motorSetting.VMax[motor_number]);
				rt_kprintf("set motor[%d] speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);	
			}			
			break;
		case v_max:
			{			
				motorSetting.VMax[motor_number]=TMC429_ParameterRW.Value.Int32;
				Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[motor_number]);
				SetAmaxBySpeed(0,motor_number,motorSetting.VMax[motor_number]);
				rt_kprintf("set motor[%d] IDX_VMAX=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);	
			}			
			break;			
			

		case acc_max:
			motorSetting.AMax[motor_number]=TMC429_ParameterRW.Value.Int32;
			SetAMax(TMC429_DEV,TMC429_motor, motorSetting.AMax[motor_number]);
			rt_kprintf("set motor[%d] max acceleration speed=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);		
			break;

		case rightLimit_disable:	//12
			Write[0]=IDX_REFCONF_RM|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			if(TMC429_ParameterRW.Value.Byte[0]!=0)
				Write[2]=Read[2]|0x02;
			else
				Write[2]=Read[2]&  ~0x02;
			Write[3]=Read[3];
			Write[0]=IDX_REFCONF_RM|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			if(TMC429_ParameterRW.Value.Byte[0]==0)
				rt_kprintf("motor[%d] right limit switch is disable\n",motor_number);	
			else 
				rt_kprintf("motor[%d] right limit switch is enable\n",motor_number);	
			break;

		case leftLimit_disable: //13
			Write[0]=IDX_REFCONF_RM|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			if(TMC429_ParameterRW.Value.Byte[0]!=0)
				Write[2]=Read[2]|0x01;
			else
				Write[2]=Read[2]&  ~0x01;
			Write[3]=Read[3];
			Write[0]=IDX_REFCONF_RM|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			if(TMC429_ParameterRW.Value.Byte[0]==0)
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
			Write[2]=(Read[2] & 0xf0) | (TMC429_ParameterRW.Value.Byte[0] & 0x0f);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			motorSetting.RampDiv[motor_number]=TMC429_ParameterRW.Value.Byte[0] & 0x0f;
			rt_kprintf("set motor[%d] ramp divisor=%d ok\n",motor_number,TMC429_ParameterRW.Value.Int32);
			break;

		case pulse_divisor:
			Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			Write[1]=Read[1];
			Write[2]=(Read[2] & 0x0f) | (TMC429_ParameterRW.Value.Byte[0] << 4);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(TMC429_motor<<5);
			ReadWrite429(TMC429_DEV,Read, Write);
			motorSetting.PulseDiv[motor_number]=TMC429_ParameterRW.Value.Byte[0]& 0x0f;
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


//
#endif
/*******************************************TMC429 motion RWAxisParameter*************************************/
/*************************************************************************************************************/
/*****************************************motor commands list*********************************************************
																						motor commands
********************************************motor commands******************************/
#if 1
/*********************************************************************************************************************/

/*********************************************************************************************************************/
static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);
/*********************************************************************************************************************/
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
#if	DBG_ENABLE

	rt_kprintf("\n");
	rt_kprintf("motor usage: \n");
	rt_kprintf("\n");
	rt_kprintf("motor stop <var>\n");		
	rt_kprintf("    var:0~5\n");
	rt_kprintf("\n");
	rt_kprintf("motor rotate <var1> <var2>\n");	
	rt_kprintf("    var1:0~5 var2:pulse per sec\n");
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
	rt_kprintf("    var1:0~5 var2:pulse per sec\n");
	rt_kprintf("\n");
//		rt_kprintf("motor reset                           - motor reset\n");	
//		rt_kprintf("motor stop reset                      - motor reset\n");	
//		rt_kprintf("motor reset?                          - is axis reset or not\n");		
	rt_kprintf("\n");
#else

	rt_kprintf("\tCommands  Parameter1\tParameter2\t\tEscription\n");
	rt_kprintf("motor\t rotate  <motor number> <velocity>             ---rotate with specified velocity\n");	
	rt_kprintf("motot\t move    <motor number> <position>             ---move to position(relative)\n");	
	rt_kprintf("motor\t moveto  <motor number> <position>             ---move to position(absolute)\n");
	rt_kprintf("motor\t stop    <motor number>                        ---stop motor movement\n");
	rt_kprintf("motor\t get     <parameter>    <motor number>         ---get axis parameter\n");
	rt_kprintf("motor\t set     <parameter>    <motor number> <value> ---set axis parameter\n");
	rt_kprintf("motor\t gohome  <motor number> <velocity>             ---motor search limit then search the origin sensor\n");        
	rt_kprintf("motor\t golimit <motor number> <velocity>             ---motor search limit sensor\n");	
	rt_kprintf("\nmotor number:0~5\n");
#endif
}
static void printf_cmdList_motor_set(void)
{
		rt_kprintf("\nmotor set usage: \n");
		//rt_kprintf("Commonly used commands: \n");
	
		rt_kprintf("motor set speed <axis> <value>              -set the rotate speed \n");	
		rt_kprintf("motor set rightLimit <axis> <value>         -set right limit switch <1:enable>|<0:disable>\n");
		rt_kprintf("motor set leftLimit <axis> <value>          -set left limit switch <1:enable>|<0:disable>\n");
		rt_kprintf("motor set limitSignal <axis> <value>        -set the limit effective signal reverse<1> or not<0>\n");
	
		rt_kprintf("\nThe following commands are used for debugging:\n");	
		rt_kprintf("motor set position_next <axis> <value>      -set the target position to move\n");
		rt_kprintf("motor set position_actual <axis> <value>    -set the current position\n");				
		rt_kprintf("motor set speed_actual <axis> <value>       -set the current speed \n");
		rt_kprintf("motor set VMAX <axis> <value>               -set max positioning speed\n");
		rt_kprintf("motor set AMAX <axis> <value>               -set max acceleration\n");
		rt_kprintf("motor set ramp_div <axis> <value>           -set ramp divisor value\n");
		rt_kprintf("motor set pulse_div <axis> <value>          -set pulse divisor value\n");	
		
}
static void printf_cmdList_motor_get(void)
{
	rt_kprintf("\nmotor get usage: \n");
	//用户接口
	rt_kprintf("motor get speed <axis>             -get the current speed \n");
	rt_kprintf("motor get position <axis>          -get the current position\n");
	 
	rt_kprintf("motor get limit all                -get all limit switch status\n");	
	rt_kprintf("motor get rightLimit <axis>        -get right switch status\n");
	rt_kprintf("motor get leftLimit  <axis>        -get left switch status\n");	
	
	rt_kprintf("motor get is_homed <axis>          -is axis homed or not\n");	
	rt_kprintf("motor get is_stop <axis>           -is axis stop or not\n");	
	rt_kprintf("motor get is_reach <axis>          -is axis reach the position\n");	
	rt_kprintf("motor get position_reached <axis>  -is reach position or not\n");
	rt_kprintf("motor get homeSensor <axis>        -get home sensor status\n");	
	
	//调试接口
	rt_kprintf("\nThe following commands are used for debugging:\n");	
	
	rt_kprintf("motor get next_speed <axis>        -get the target speed \n");
	rt_kprintf("motor get next_position <axis>     -get the target position to move\n");
	
	rt_kprintf("motor get VMAX <axis>              -get max positioning speed\n");
	rt_kprintf("motor get AMAX <axis>              -get max acceleration\n");

	rt_kprintf("motor get ramp_div <axis>          -get ramp divisor value\n");
	rt_kprintf("motor get pulse_div <axis>         -get pulse divisor value\n");			
	
}

void Motor_help(void)
{
	printf_cmdList_motor();
	printf_cmdList_motor_set();
	printf_cmdList_motor_get();
}
#endif



/*****************************************motor commands list****************************************************/
/*************************************************************************************************************/
/*****************************************go home | go limit*******************************************************
//寻找原点的过程被PC停止或者运动命令干涉的话，回原动作会出现异常
//所以回原点未完成的状态需要屏蔽其他动作命令,且增加自锁不要自己反复复位
******************************************************************************************************************/
/***************************************************Origin*********************************************************************

default OriginSensor --> IN GPIO

回原点动作中,需要及时采集原点信号,进行电机停止并设立当前位置为0,这个动作如果delay太久会影响其他轴的回0动作！！！

SO: 
-->设置单轴回原点-->设置原点传感器为外部中断（触发方式串口传感器型号)-->采集到信号，判断回零方向正确的话，设立当前位置为0，并关闭此外部中断。(加调试LOG)
-->回0动作中尽量不要使用串口采集信息，外部中断优先级大于串口,串口数据会出现异常

多轴自动回原点在单轴回原点基础上进行motorHoming.GoHome和motorHoming.Homed 进行管理

取消多轴封装的复位动作，增加通用性	取消USER:	motorsResetInOrder=TRUE 触发复位动作

*********************************************************************************************************************/
/***************************************************Limit*********************************************************************
回原点动作中会触发限位信号，对检测时间要求不高，可计数检测到50ms后进行反转再寻原点信号
*********************************************************************************************************************/


/**************************************************************************************************************/
#if 1
/***************************************************************************/
static rt_uint8_t MotorSensorStack[ 512 ];
static struct	rt_thread MotorSensorThread;
/***************************************************************************/
//void motor_gohome_config(uint8_t motor_number, int home_speed);
//void motor_golimit_config(uint8_t motor_number, int home_speed);

static void InitOriginSensorAsEXTI(void);
static void OriginSensorIRQEnable(uint8_t motor_number);
static void OriginSensorIRQDisable(uint8_t motor_number);
static void OriginSensorIRQCall(void *args);
static void setPositionAsOrigin(uint8_t motor_number);
static void LimitSensorProcess(uint8_t motor_number);
static void MotorSensor_Thread_entry(void *parameter);
// //原点过程触发限位		//正常运动中触发限位,超时停止
static void LimitSensorProcess(uint8_t motor_number)
{
	static uint16_t motorlimitedCNT[6];	 
	static uint16_t limitedCNT_NoHome[6];
	
//	static uint8_t time_delay;
	
	TMC429_motor=getMotor0_2(motor_number);
	TMC429_DEV=getTMC429_DEV(motor_number);	
	
//	if(motorHoming.GoHome[motor_number] && Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0	)
	if(motorHoming.GoHome[motor_number] )
	{
		
		if((OriginSensorON[motor_number]==HIGH && pinRead(OriginSensorPin[motor_number])==HIGH )||
			 (OriginSensorON[motor_number]==LOW && pinRead(OriginSensorPin[motor_number])==LOW	 ))
		{
			
			//rt_kprintf("触发原点信号\n");
			
			if((motorHoming.HomeSpeed[motor_number] >0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))<0 ) || 
				 (motorHoming.HomeSpeed[motor_number] <0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))>0 )    )
			{	
				//触发原点的状态满足回原方式,停止电机把当前位置寄存器写0,并关闭原点中断功能			
				setPositionAsOrigin(motor_number);
				rt_kprintf("motor[%d] gohome ok\n",motor_number);	
				//OriginSensorIRQDisable(motor_number);	
			}
		}
		//防止回原点结束后还执行后面的动作
		if(	motorHoming.GoHome[motor_number] )
		{
			//确定停止是由于限位触发的原因后，电机反转
			u8 SwitchStatus=Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3);
			
			if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && ( (SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0))
			{
				//两个限位都触发，不正常
				//Do nothing
			}
			else if((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0)																	//触发左限位
			{		
				//TMC429_MotorRotate(motor_number,	(abs(motorHoming.HomeSpeed[motor_number])/4));	//向右转
				//if(motorlimitedCNT[motor_number]==0) { StopMotorByLimit(motor_number);motorlimitedCNT[motor_number]++;}
				if(motorlimitedCNT[motor_number]++>=500)
				{
					motorlimitedCNT[motor_number]=0;
					TMC429_MotorRotate(motor_number,	(abs(motorHoming.HomeSpeed[motor_number])/4));			//向右转	
				}
			}
			else if((SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0)													//触发右限位
			{		
				//if(motorlimitedCNT[motor_number]==0) 	{ StopMotorByLimit(motor_number);motorlimitedCNT[motor_number]++;}
				if(motorlimitedCNT[motor_number]++>=500)
				{
					motorlimitedCNT[motor_number]=0;
					TMC429_MotorRotate(motor_number,-abs(motorHoming.HomeSpeed[motor_number]/4));				//左转
				}
			}
		}
	}
	//电机回限位过程中停止了
	else if(motorHoming.GoLimit[motor_number] && Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5))==0	)
	{
		//确定停止是由于限位触发的原因后，电机复位OK
		uint8_t SwitchStatus=Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3);	
		
		//rt_kprintf("home_speed=%d\n",motorHoming.HomeSpeed[motor_number]);
		
		if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && ( (SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0))
		{
			//两个限位都触发，不正常
			//Do nothing
			//L TMC429_ParameterRW.Value.Int32=(Read429SingleByte(TMC429_DEV,IDX_REF_SWITCHES, 3) & (0x02<<TMC429_motor*2)) ? 1:0;
		}
		
		else if(((SwitchStatus& (0x02<<TMC429_motor*2)) ? 1:0) && (motorHoming.HomeSpeed[motor_number]<0)) 								
		{																													 
			setPositionAsOrigin(motor_number);	
			rt_kprintf("motor[%d] golimit ok\n",motor_number);				
		}
		else if(((SwitchStatus& (0x01<<TMC429_motor*2)) ? 1:0) && (motorHoming.HomeSpeed[motor_number]>0)) 
		{																													
			setPositionAsOrigin(motor_number);	
			rt_kprintf("motor[%d] golimit ok\n",motor_number);				
		}
	}	
	//limitedCNT_NoHome[motor_number] 需要一一对应 不要错位
#if 0
	else if( limitedCNT_NoHome[motor_number]++>20 && motorHoming.GoHome[motor_number]==FALSE && motorHoming.GoLimit[motor_number]==FALSE )	//间隔一段时间检查是否触发限位	
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
#endif
//	rt_kprintf("motor_number befor=%d\n",motor_number);

//	if( ( time_delay++>8) && (motorHoming.GoHome[motor_number]==FALSE) && (motorHoming.GoLimit[motor_number]==FALSE )	)//间隔一段时间检查是否触发限位	
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
	//InitOriginSensorAsEXTI();
	while (1)
	{  
		
		if(motorHoming.GoHome[0] || motorHoming.GoLimit[0] 	|| motorHoming.GoHome[1] || motorHoming.GoLimit[1]	||\
			 motorHoming.GoHome[2] || motorHoming.GoLimit[2]	|| motorHoming.GoHome[3] || motorHoming.GoLimit[3]	||\
			 motorHoming.GoHome[4] || motorHoming.GoLimit[4]	|| motorHoming.GoHome[5] || motorHoming.GoLimit[5]	)
		{
			for(uint8_t i=0;i<N_O_MOTORS;i++)
			{		
				rt_enter_critical();		
			
				LimitSensorProcess(i);
			
				rt_exit_critical();
			}	
			rt_hw_us_delay(10);
		}
		else rt_thread_mdelay(100);		
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
	
	motorHoming.Homed[motor_number]=TRUE;
	motorHoming.GoHome[motor_number]=FALSE;	
	motorHoming.GoLimit[motor_number]=FALSE;	
	
//	rt_kprintf("motor[%d] go home ok\n",motor_number);	
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
			
			if((motorHoming.HomeSpeed[motor_number] >0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))<0 ) || 
				 (motorHoming.HomeSpeed[motor_number] <0 && (Read429Short(TMC429_DEV,IDX_VACTUAL|(TMC429_motor<<5)))>0 )    )
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
	motorHoming.GoHome[motor_number]		=	TRUE;
	motorHoming.GoLimit[motor_number]		=	FALSE;
	motorHoming.Homed[motor_number]			=	FALSE;
	motorHoming.HomeSpeed[motor_number]	=	home_speed;	
		
	//OriginSensorIRQEnable(motor_number);
	TMC429_MotorRotate(motor_number,home_speed);
}
void motor_golimit_config(uint8_t motor_number, int home_speed)
{
	motorHoming.GoHome[motor_number]		=	FALSE;
	motorHoming.GoLimit[motor_number]		=	TRUE;
	motorHoming.Homed[motor_number]			=	FALSE;
	motorHoming.HomeSpeed[motor_number]=	home_speed;	
	
//	SetAmaxBySpeed(motor_number, home_speed);
//	rt_kprintf("home_speed=%d\n",home_speed);
	TMC429_MotorRotate(motor_number,home_speed);	
}

#endif


/*****************************************go home | go limit*******************************************************/
/*************************************************************************************************************/

/*************************************************************************************************************/
/****************************************FBTMC429_Motor_msh_cmd****************************************************/
#if 1

uint8_t Command_analysis_motor(char *string);
uint8_t Command_analysis_motor_get(char *string);
uint8_t Command_analysis_motor_set(char *string);

/*****************************************************************************************/
int motor(int argc, char **argv)
{
	int result = REPLY_OK;
	
	static uint8_t tmc429_inited=0;
	
	if(tmc429_inited==0)
	{
		tmc429_inited=1;
		//tmc429_hw_init();		
	}
	CMDGetFromUart.Type=Type_none;
	/*
	motor
	*/
	if		  (argc == 1 )  									{	printf_cmdList_motor();	}
	/*
	motor set
	motor get
	*/
	else if (argc == 2 )					
	{
		if 			 (!strcmp(argv[1], "set"))		{ printf_cmdList_motor_set();}
		else	if (!strcmp(argv[1], "get"))		{	printf_cmdList_motor_get();}
		else																	{ result = REPLY_INVALID_CMD;}
	}
	/*
	motor stop
	*/
	else if (argc == 3 )	
	{
		CMDGetFromUart.Motor=atoi(argv[2]);
		
		if (CMDGetFromUart.Motor > N_O_MOTORS) result = REPLY_INVALID_VALUE;
			
		else if(!strcmp(argv[1], "stop"))
		{
			StopMotorByHW(CMDGetFromUart.Motor);
		}
		else result = REPLY_INVALID_CMD;
	}
	/*
	motor get <type>	<axis:0~5>
							|
						VMAX		|	speed			|	position			|	is_homed	| is_stop 	|	is_reach	|	position_reached
						AMAX		|next_speed	|	next_position	|	limit			| leftLimit	|	homeSensor|	rightLimit
						ramp_div|pulse_div	|														|leftLimit?	|						|	rightLimit?

		20200619:	可以多个命令同一个功能
		add:
	
	motor rotate <axis:0~5>	<speed>
	motor move	 <axis:0~5>	<pulse position>  --- Relative motion
	motor moveto <axis:0~5>	<pulse position>  --- Absolute movement
	motor gohome <axis:0~5>	<speed>
	motor golimit<axis:0~5>	<speed>
	
	*/
	else if (argc == 4)
	{	
		if (!strcmp(argv[1], "get"))
		{
			if      (!strcmp(argv[2], "speed")) 	 			CMDGetFromUart.Type=speed_pulse;
			else if (!strcmp(argv[2], "position")) 				CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart.Type=is_homed;
			else if (!strcmp(argv[2], "is_stop")) 				CMDGetFromUart.Type=is_stop;
			else if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart.Type=is_reach;

			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;		
			else if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart.Type=v_max;
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
			
			//命令正确
			if(CMDGetFromUart.Type!=Type_none)
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				//参数错误
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				//参数正确
				else TMC429_GetAxisParameter(CMDGetFromUart.Motor,CMDGetFromUart.Type);	
			}
			//命令错误
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
								
			//先判断指定的电机编号不溢出
			if (CMDGetFromUart.Motor >=N_O_MOTORS) result = REPLY_INVALID_VALUE;	
			else 
			{
				if (!strcmp(argv[1], "gohome"))
				{
					rt_kprintf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);		
					motor_gohome_config(CMDGetFromUart.Motor, ChangeSpeedPerSecondToVMax(CMDGetFromUart.Value.Int32));		
				}
				else if (!strcmp(argv[1], "golimit"))
				{	
					rt_kprintf("motor[%d] is start go home by searching limit sensor\n",CMDGetFromUart.Motor);	
					motor_golimit_config(CMDGetFromUart.Motor,ChangeSpeedPerSecondToVMax(CMDGetFromUart.Value.Int32));							
				}				
				else if(motionLocked[CMDGetFromUart.Motor])
				{					
					rt_kprintf("motor%d is locked and is not allowed to rotate|move|moveto\n",CMDGetFromUart.Motor);	
				}
				else 
				{
					if (!strcmp(argv[1], "rotate"))
					{						
						//TMC429_MotorRotate(CMDGetFromUart.Motor,ChangeSpeedPerSecondToVMax(CMDGetFromUart.Value.Int32))	;

						TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32)	;			
						rt_kprintf("motor%d is rotate at %d pulse per sec\n",CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);	

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
					else result = REPLY_INVALID_CMD;
				}				
			}
		}
	}
	/*
	motor set <type>	<axis:0~5> <value>
							|
						VMAX|speed		|	speed_actual			|	actual_position	|		ramp_div	| leftLimit		| limitSignal		
						AMAX					|	next_speed				|	next_position		|		pulse_div	| rightLimit	
																			

		20200619:	可以多个命令同一个功能
		add:
	*/	
	else if (argc == 5)
	{
		//命令正确
		if (!strcmp(argv[1], "set"))
		{
			//user
			if (!strcmp(argv[2], "speed")) 					 		  CMDGetFromUart.Type=speed_max;
			//debug
			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;		
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;
			else if (!strcmp(argv[2], "actual_position")) CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "speed_actual")) 	 	CMDGetFromUart.Type=speed_actual;
			else if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart.Type=v_max;
			else if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=acc_max;
			
			else if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart.Type=rightLimit_disable;
			else if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_disable;	  		
			else if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart.Type=limitSignal;		
			else if (!strcmp(argv[2], "ramp_div")) 			  	CMDGetFromUart.Type=ramp_divisor;
			else if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;
			
			//类型正确
			if(CMDGetFromUart.Type!=Type_none)	
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				CMDGetFromUart.Value.Int32=atoi(argv[4]);
				//参数错误
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				//参数正确
				else TMC429_SetAxisParameter(CMDGetFromUart.Motor, CMDGetFromUart.Type, CMDGetFromUart.Value.Int32);
			}
			//类型错误
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
	#if 0    //commands strings user
	uint8_t FBTMC429_Motor_msh(char *string)
	{
		uint8_t result = REPLY_OK;
		
		if(!Command_analysis_motor(string))								{;}
		else if(!Command_analysis_motor_get(string))			{;}
		else if(!Command_analysis_motor_set(string))			{;}
		else result = REPLY_INVALID_CMD;
		return result;
	}
	static int8_t  CommmandLine_speed(char *Commands)
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
				motorSetting.VMax[CMDGetFromUart.Motor]=strtol(s, &p, 10);	
				len=p-s;				
				if(0<len&&len<=4)			
				{
					Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[CMDGetFromUart.Motor]);
					//SetAMaxBySpeed by project
					rt_kprintf("\nVset[%d-%d]<OK>\n",CMDGetFromUart.Motor,motorSetting.VMax[CMDGetFromUart.Motor]);

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
				rt_kprintf("\nVset[%d]<%d>,V[%d]<%d>\n",CMDGetFromUart.Motor,motorSetting.VMax[CMDGetFromUart.Motor],CMDGetFromUart.Motor,TMC429_ParameterRW.Value.Int32);
				return REPLY_OK;
			}	
			else return REPLY_INVALID_VALUE;
		}
		else return REPLY_INVALID_CMD;	
	}
	//
	static uint8_t  CommmandLine_acc(char *Commands)
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
				motorSetting.AMax[CMDGetFromUart.Motor]=strtol(s, &p, 10);	
				len=p-s;				
				if(0<len&&len<=4)				
				{
					SetAMax(TMC429_DEV,TMC429_motor, motorSetting.AMax[CMDGetFromUart.Motor]);
					rt_kprintf("A[%d-%d]<OK>\n",CMDGetFromUart.Motor,motorSetting.AMax[CMDGetFromUart.Motor]);				
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
			rt_kprintf("\nAcc<%d,%d,%d,%d,%d,%d>\n",motorSetting.AMax[0],motorSetting.AMax[1],motorSetting.AMax[2],motorSetting.AMax[3],motorSetting.AMax[4],motorSetting.AMax[5]);
			return REPLY_OK;
		}
		return REPLY_INVALID_VALUE;
	}
	//
	static uint8_t  CommmandLine_rotate(char *Commands)
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
						motorSetting.SpeedChangedFlag[CMDGetFromUart.Motor]=TRUE;
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
	static uint8_t  CommmandLine_move(char *Commands)
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
						if(motorSetting.SpeedChangedFlag[CMDGetFromUart.Motor])
						{
							Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[CMDGetFromUart.Motor]);
							SetAMax(TMC429_DEV,TMC429_motor, motorSetting.AMax[CMDGetFromUart.Motor]);
							motorSetting.SpeedChangedFlag[CMDGetFromUart.Motor]=FALSE;
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
						if(motorSetting.SpeedChangedFlag[CMDGetFromUart.Motor])
						{
							Write429Short(TMC429_DEV,IDX_VMAX|(TMC429_motor<<5), motorSetting.VMax[CMDGetFromUart.Motor]);
							SetAMax(TMC429_DEV, TMC429_motor, motorSetting.AMax[CMDGetFromUart.Motor]);
							motorSetting.SpeedChangedFlag[CMDGetFromUart.Motor]=FALSE;
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
	static uint8_t  CommmandLine_pos(char *Commands)
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
	static uint8_t  CommmandLine_limit(char *Commands)
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
	static uint8_t  CommmandLine_reach(char *Commands)
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
	static uint8_t  CommmandLine_home(char *Commands)
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
					motorHoming.HomeSpeed[CMDGetFromUart.Motor]=strtol(s, &p, 10);	
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
				

	//	else if( !strcmp(string,"motor reset")) 				{motorsResetInOrder=TRUE;}//MotorAutoReset_preset();
		else if( !strcmp(string,"motor stop reset"))		 		
		{
			motorHoming.GoHome[0]=0;
			motorHoming.GoHome[1]=0;
			motorHoming.GoHome[2]=0;
			StopMotorByHW(0);StopMotorByHW(1);StopMotorByHW(2);	
			printf("motor stop reset and all motor is stop\r\n") ;		
		}
		else if( !strncmp(string,"motor stop ",11)) 		
		{

			char *p = NULL;
			char *s = &string[11];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);	
			if(CMDGetFromUart.Motor<N_O_MOTORS)
			{
				StopMotorByHW(CMDGetFromUart.Motor);
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
						motorHoming.GoHome[CMDGetFromUart.Motor] = TRUE;
						motorHoming.GoLimit[CMDGetFromUart.Motor]= FALSE;
						motorHoming.Homed[CMDGetFromUart.Motor]	= FALSE;

						motorHoming.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
					
						SetAmaxBySpeed(1,CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
			
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
						motorHoming.GoHome[CMDGetFromUart.Motor]	=FALSE;
						motorHoming.GoLimit[CMDGetFromUart.Motor]=TRUE;
						motorHoming.Homed[CMDGetFromUart.Motor]	=FALSE;
						motorHoming.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
						SetAmaxBySpeed(1,CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));

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
				if(*p=='\0')	
				{
					rt_kprintf("\n");
					TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_actual);
				}
			}
			else if( !strncmp(string1,"position ",9)) 		
			{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	
				{
					rt_kprintf("\n");
					TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_actual);
				}
			}
			else if( !strncmp(string1,"is_homed ",9)) 		
			{
					char *p = NULL;
					char *s = &string1[9];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_homed);
					}
			}	
			else if( !strncmp(string1,"is_stop ",8)) 		
			{
					char *p = NULL;
					char *s = &string1[8];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_stop);
					}
			}		
			else if( !strncmp(string1,"is_reach ",9)) 		
			{
					char *p = NULL;
					char *s = &string1[9];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_reach);
					}
			}		
			else if( !strncmp(string1,"next_speed ",11)) 		
			{
					char *p = NULL;
					char *s = &string1[11];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_next);
					}
			}	
			else if( !strncmp(string1,"next_position ",14)) 		
			{
					char *p = NULL;
					char *s = &string1[14];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_next);
					}
			}	
			else if( !strncmp(string1,"VMAX ",5)) 		
			{
					char *p = NULL;
					char *s = &string1[5];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,speed_max);
					}
			}	
			else if( !strncmp(string1,"AMAX ",5)) 		
			{
					char *p = NULL;
					char *s = &string1[5];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,acc_max);
					}
			}	
			else if( !strncmp(string1,"position_reached ",17)) 		
			{
					char *p = NULL;
					char *s = &string1[17];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_reached);
					}
			}	
			else if( !strncmp(string1,"rightLimit ",11)) 		
			{
					char *p = NULL;
					char *s = &string1[11];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,rightLimit_SwS);
					}
			}
			else if( !strncmp(string1,"leftLimit ",10)) 		
			{
					char *p = NULL;
					char *s = &string1[10];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,leftLimit_SwS);
					}
			}
			else if( !strcmp(string1,"limit all")) 		
			{
				rt_kprintf("\n");
				TMC429_GetAxisParameter(CMDGetFromUart.Motor,allLimit_SwS);
			}
			else if( !strncmp(string1,"homeSensor ",11)) 		
			{
					char *p = NULL;
					char *s = &string1[11];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,home_SenS);
					}
			}
			else if( !strncmp(string1,"ramp_div ",9)) 		
			{
					char *p = NULL;
					char *s = &string1[9];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,ramp_divisor);
					}
			}
			else if( !strncmp(string1,"pulse_div ",10)) 		
			{
					char *p = NULL;
					char *s = &string1[10];	
					CMDGetFromUart.Motor =strtol(s, &p, 10);
					if(*p=='\0')	
					{
						rt_kprintf("\n");
						TMC429_GetAxisParameter(CMDGetFromUart.Motor,pulse_divisor);
					}
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
					{
						rt_kprintf("\n");
						TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_max,CMDGetFromUart.Value.Int32);
					}
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
					{
						rt_kprintf("\n");
						TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_next,CMDGetFromUart.Value.Int32);
					}
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
					{
						rt_kprintf("\n");
						TMC429_SetAxisParameter(CMDGetFromUart.Motor,position_next,CMDGetFromUart.Value.Int32);
					}
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
					{
						rt_kprintf("\n");
						TMC429_SetAxisParameter(CMDGetFromUart.Motor,position_actual,CMDGetFromUart.Value.Int32);
					}
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
						if(*p1=='\0')	
						{
							rt_kprintf("\n");
							TMC429_SetAxisParameter(CMDGetFromUart.Motor,speed_max,CMDGetFromUart.Value.Int32);
						}
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
						if(*p1=='\0')	
						{
							rt_kprintf("\n");
							TMC429_SetAxisParameter(CMDGetFromUart.Motor,acc_max,CMDGetFromUart.Value.Int32);
						}
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
						if(*p1=='\0')	
						{
							rt_kprintf("\n");
							TMC429_SetAxisParameter(CMDGetFromUart.Motor,ramp_divisor,CMDGetFromUart.Value.Int32);
						}
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
						if(*p1=='\0')	
						{
							rt_kprintf("\n");
							TMC429_SetAxisParameter(CMDGetFromUart.Motor,pulse_divisor,CMDGetFromUart.Value.Int32);
						}
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
						if(*p1=='\0')	
						{
							rt_kprintf("\n");
							TMC429_SetAxisParameter(CMDGetFromUart.Motor,limitSignal,CMDGetFromUart.Value.Int32);	
						}
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
	#endif
#endif
/****************************************FBTMC429_Motor_msh_cmd****************************************************/
/*************************************************************************************************************/
MSH_CMD_EXPORT(motor, control motor motion by commands);

/**********************************motor autoResetByProID with button & cmd***************************************
																						motion control
********************************************motion control******************************/

#if 0

uint8_t motorResetStep=no_need_home;
uint8_t motorsResetOK=0;
uint8_t motorsResetInOrder=0; 
uint8_t motorIsResetting=0;

void ButtonReset_process(void)
{
	motorsResetInOrder=TRUE;
	buttonRESETpressed=TRUE;
}

void motorResetByProID( void )
{			
	if(motorsResetInOrder==TRUE)	//只配置一次，使能相关
	{
		motorsResetInOrder=FALSE;		//outputSet(1,0);			outputSet(2,1);	 
		
		motorsResetOK=FALSE;
		
		for(uint8_t i=0;i<N_O_MOTORS;i++)
		{
			StopMotorByRamp(i);
			motorHoming[i].Homed=FALSE;
			motorHoming[i].GoHome=FALSE;
			motorHoming[i].GoLimit=FALSE;
		}	
		delay_ms(20);	
		if(motorSetting.Project_ID==OQC_FLEX || motorSetting.Project_ID==BUTTON_ROAD )
		{
				motorResetStep=z_go_home;		
		}
	}
	switch(motorResetStep)
	{
		case x_go_home:		motor_gohome_config(0,motorSetting.speed_home[0]);  motorResetStep=x_is_ok;					
			break;
		case y_go_home:		motor_gohome_config(1,-motorSetting.speed_home[1]);	motorResetStep=y_is_ok;		
			break;		
		case z_go_home:		motor_gohome_config(2,-motorSetting.speed_home[2]);	motorResetStep=z_is_ok;	
			break;
		
		case x_is_ok:		
			
				if(motorHoming[0].Homed==TRUE)
				{
					if(motorSetting.Project_ID==BUTTON_ROAD)	motorResetStep=all_is_ok;				
				}				
			break;
		
		case y_is_ok:		
			   
				if(motorHoming[1].Homed==TRUE) 	motorResetStep=all_is_ok;
			break;		
		
		case z_is_ok:	
			
				if(motorHoming[2].Homed==TRUE)
				{
					if(motorSetting.Project_ID==OQC_FLEX)						  motorResetStep=all_is_ok;
					else if(motorSetting.Project_ID==BUTTON_ROAD)			motorResetStep=x_go_home;										
				}
			break;	
		
		case all_is_ok:	
						
						//buttonRESETpressed=FALSE;
						//outputSet(2,0);	
						rt_kprintf("motor auto reset ok\n");
						
						motorResetStep=no_need_home;	
						
						buttonRESETpressed=FALSE;
		
						motorsResetOK=TRUE;
			break;
		
		case no_need_home:	
			break;
		default:
			break;	
	}
}	












#endif

/**********************************motor autoResetByProID with button & cmd****************************************/
/*************************************************************************************************************/
/**********************************TMC429 vmax amax pulse_div ramp_div CONFIG*******************************
																						TMC429 CONFIG
********************************************TMC429 CONFIG*****************************/
#if 0
/****TMC429 : Setting up the Ramp Generator ：TMC429.xls base on ustep=1 usteps=steps by ex drivers The motor and driver are compatible***********************************************************/
/* 	ustep	 steps	Vmax RPS RPM			ustep	 steps	Vmax RPS RPM	ustep	 steps	Vmax RPS RPM
		1				200		19.52 1171.3			16		 3200		1.22	73.21			64	 12800	0.31	18.3
		2				400		9.76	585.65			20		 4000		0.98	58.57			100  10000	0.39	23.43
		4				800		4.88	292.83			25	   5000		0.78	46.85			125	 20000  0.2		11.71
		5				1000	3.9		234.26			32	   6400		0.61	36.6
		8				1600	2.44	146.41			40	   6000   0.65	39.04
		10			2000	1.95	117.13			50	   8000		0.49	29.28
*/
//all motors load same set
void SetMotorsteps(void)
{
	motor_usteps=8000;
}
void TMC429_RampGeneratorByUsteps(int usteps)	//CLK=16MHZ
{
  uint8_t i;
		
	for(i=0; i<N_O_MOTORS; i++) 					
	{				
		switch(usteps)
		{
			case 8000:	
				
				MotorConfig[i].VMax 		= 840;   				
				MotorConfig[i].AMax 		=	2000;
				MotorConfig[i].PulseDiv	=	7;	
				MotorConfig[i].RampDiv	=	7;					
				motorHoming[i].HomeSpeed		=	2000;
				break;

			default:
				break;
		}		
	}
}
/**************************************************************************/
void motorSetVel(uint8_t which_motor, double velocity)	//设置目标速度。 单位： pulse/ms pulse/rev
{
	/*button: Vneed=0.5mm/s  
	driver TR22G-D: 1.on-one pulse  2.on  3.on 3 --39% 4.on  5.off  6.off ---8ustep
	step motor : 200steps per turn		2mm/rev   Vneed=0.5mm/s=0.5/2(rev_s= *60=15r/min) *1600=400pulse/s
	tmc429 set: Vmax≈2047=10*Vneed=146.41 PULS_DEV=7 RAMP_DIV=7
	VMax=210=15.02RPM=400.54PULSE≈400PULSE 所有速度以此为参考
	VMaxUser≈140PRM=140/15*400
	VMaxSet=velocity/400*210
	AMaxSet=SelectAMaxBySpeed(VMaxSet);
	*/
}
void SelectAMaxByVMax(uint8_t which_motor, uint32_t speed_Vmax)  //TMC429 16MHZ
{
	uint32_t AMaxTemp;
	
//	speed=speed*210/400000;	//命令输入的是脉冲数 不再是TMC429内部寄存器值
	
	switch(motorSetting.Project_ID)
	{		  
		case Sec_QT:				
			 AMaxTemp=(speed_Vmax*speed_Vmax*240)/(210*210);		 
			 if(AMaxTemp>2000) AMaxTemp=2000;
		   if(AMaxTemp<5) AMaxTemp=5;
//				 AMaxTemp=(MotorConfig[which_motor].VMax*MotorConfig[which_motor].VMax*240)/(210*210);		 
				 motorSetting.AMax[which_motor]=AMaxTemp;		
			break;				 

		default:
			break;	
	}
}
/**************************************************************************/
#endif








/**********************************TMC429 vmax amax pulse_div ramp_div CONFIG****************************************/
/*************************************************************************************************************/

//MotorSensor_thread_init();
//	MX_TIM3_Init();	
//	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
//	TMC429_CLK=16MHZ	
