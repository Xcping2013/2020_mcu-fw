#include "inc_projects.h"

#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"
#include "inc_dido.h"

#include "app_eeprom_24xx.h"
#include "app_tmc429.h"	

#include "mbtmc429_spi.h"
#include "app_motion.h"	
/*********************************************************************************************************************/
#define	CMD_RESPONSE_EN	1
#ifdef	CMD_RESPONSE_EN
#define CMD_TRACE         	rt_kprintf
#else
#define CMD_TRACE(...)   
#endif
#define	DEBUG_EN	1
#ifdef	DEBUG_EN
#define DEBUG_TRACE         rt_kprintf
#else
#define DEBUG_TRACE(...)   
#endif

	#define POSCMP1_PIN		PE_12
	#define	INTOUT1_PIN		PE_13	

PARAM_T g_tParam;

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

homeInfo_t homeInfo;

TMotorConfig MotorConfig[N_O_MOTORS]=
{
				//12MHZ 																			
				//3R/S																			
  839,  //!< VMax					839,  											
  1000, //!< AMax					1000, 												
  5,    //!< Pulsediv			5,    													
  8,    //!< Rampdiv			8,    													
};

typedef struct
{
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} TMC429_Command_0;

TMC429_Command_0 CMDGetFromUart_0;

typedef struct
{
  uint8_t Status;               
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;

uint8_t homeSensorPin[3]={PB_9,PE_0,PE_1};

TMC429_Parameter TMC429_ParameterGet;

TMC429_Parameter TMC429_ParameterSet;

static UCHAR SpeedChangedFlag[N_O_MOTORS];

static uint8_t MotorStopByLimit[N_O_MOTORS]={ENABLE,ENABLE,ENABLE};								//碰到限位处理一次
		
/*********************************************************************************************************************/
static void 		TMC429_ParameterPresetToRam(void);
static uint8_t 	TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);

static void 		TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void 		TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);
static void 		MotorKeepStop_removeLimitSigal(uint8_t motor_number);

static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);

/*********************************************************************************************************************/
//SPI_HandleTypeDef hspi1;
//SPI_HandleTypeDef hspi2;

///* SPI1 init function */
//void MX_SPI1_Init_0(void)
//{

//  hspi1.Instance = SPI1;
//  hspi1.Init.Mode = SPI_MODE_MASTER;
//  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
//  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
//  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
//  hspi1.Init.NSS = SPI_NSS_SOFT;
//  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
//  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//  hspi1.Init.CRCPolynomial = 10;
//  if (HAL_SPI_Init(&hspi1) != HAL_OK)
//  {
//    Error_Handler();
//  }

//}

//void MX_SPI2_Init_0(void)
//{

//  hspi2.Instance = SPI2;
//  hspi2.Init.Mode = SPI_MODE_MASTER;
//  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
//  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
//  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
//  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
//  hspi2.Init.NSS = SPI_NSS_SOFT;
//  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
//  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
//  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
//  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//  hspi2.Init.CRCPolynomial = 10;
//  if (HAL_SPI_Init(&hspi2) != HAL_OK)
//  {
//    Error_Handler();
//  }

//}
////
//void HAL_SPI_MspInit_0(SPI_HandleTypeDef* spiHandle)
//{

//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(spiHandle->Instance==SPI1)
//  {
//  /* USER CODE BEGIN SPI1_MspInit 0 */

//  /* USER CODE END SPI1_MspInit 0 */
//    /* SPI1 clock enable */
//    __HAL_RCC_SPI1_CLK_ENABLE();
//  
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    /**SPI1 GPIO Configuration    
//    PA5     ------> SPI1_SCK
//    PA6     ------> SPI1_MISO
//    PA7     ------> SPI1_MOSI 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = GPIO_PIN_6;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//  /* USER CODE BEGIN SPI1_MspInit 1 */
//		pinMode(PC_4,PIN_MODE_OUTPUT);
//		pinSet(PC_4);
//  /* USER CODE END SPI1_MspInit 1 */
//  }
//  else if(spiHandle->Instance==SPI2)
//  {
//  /* USER CODE BEGIN SPI2_MspInit 0 */

//  /* USER CODE END SPI2_MspInit 0 */
//    /* SPI2 clock enable */
//    __HAL_RCC_SPI2_CLK_ENABLE();
//  
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**SPI2 GPIO Configuration    
//    PB13     ------> SPI2_SCK
//    PB14     ------> SPI2_MISO
//    PB15     ------> SPI2_MOSI 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = GPIO_PIN_14;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//  /* USER CODE BEGIN SPI2_MspInit 1 */
////    HAL_NVIC_SetPriority(SPI2_IRQn, 4, 0);
////    HAL_NVIC_EnableIRQ(SPI2_IRQn);
//		pinMode(PB_12,PIN_MODE_OUTPUT);
//		pinSet(PB_12);
//  /* USER CODE END SPI2_MspInit 1 */
//  }
//}

//void HAL_SPI_MspDeInit_0(SPI_HandleTypeDef* spiHandle)
//{

//  if(spiHandle->Instance==SPI1)
//  {
//  /* USER CODE BEGIN SPI1_MspDeInit 0 */

//  /* USER CODE END SPI1_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_SPI1_CLK_DISABLE();
//  
//    /**SPI1 GPIO Configuration    
//    PA5     ------> SPI1_SCK
//    PA6     ------> SPI1_MISO
//    PA7     ------> SPI1_MOSI 
//    */
//    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

//  /* USER CODE BEGIN SPI1_MspDeInit 1 */

//  /* USER CODE END SPI1_MspDeInit 1 */
//  }
//  else if(spiHandle->Instance==SPI2)
//  {
//  /* USER CODE BEGIN SPI2_MspDeInit 0 */

//  /* USER CODE END SPI2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_SPI2_CLK_DISABLE();
//  
//    /**SPI2 GPIO Configuration    
//    PB13     ------> SPI2_SCK
//    PB14     ------> SPI2_MISO
//    PB15     ------> SPI2_MOSI 
//    */
//    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

//  /* USER CODE BEGIN SPI2_MspDeInit 1 */
//	 // HAL_NVIC_DisableIRQ(SPI2_IRQn);
//  /* USER CODE END SPI2_MspDeInit 1 */
//  }
//} 




void SetAmaxAutoByspeed(u8 axisNum,int speed)
{
	//16MHZ|1600steps PulseDiv=6 //speed=105->0.25/s  15.02/m 2047=4.88/S  292.83/m
	//16MHZ|1600steps PulseDiv=7 //speed=210->0.25/s  15.02/m 2047=2.44/S  146.41/m
	if(MotorConfig[axisNum].PulseDiv==6 &&	MotorConfig[axisNum].RampDiv==8)
	{
		//按项目进行速度设置 0.5mm/S为最小速度  其他速度都大于此设置值
//  if (speed==105)									MotorConfig[axisNum].AMax=550;	//0.05s-->210---0.5/S
		if (speed==105)										MotorConfig[axisNum].AMax=275;	//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=10;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed+10;	
		else if (105<speed && speed<210)	MotorConfig[axisNum].AMax=speed*2;	
		else if (209<speed && speed<400)	MotorConfig[axisNum].AMax=1000;	
		else 															MotorConfig[axisNum].AMax=2047;	
		SetAMax(getTMC429_DEV(1),axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed==70)								MotorConfig[axisNum].AMax=200;	//0.05s-->210---0.5/S
		
		else if (speed<50)								MotorConfig[axisNum].AMax=speed;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed*2;	
		else if (104<speed && speed<210)	MotorConfig[axisNum].AMax=speed*3;		
		else 															MotorConfig[axisNum].AMax=2000;	
		SetAMax(getTMC429_DEV(1),axisNum, MotorConfig[axisNum].AMax);
	}
//	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
//	{
//		if (speed==35)										MotorConfig[axisNum].AMax=50;	//0.05s-->210---0.5/S

//		if (speed==70)										MotorConfig[axisNum].AMax=200;	//0.05s-->210---0.5/S
//		else if (speed<50)								MotorConfig[axisNum].AMax=speed;	
//		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed*2;	
//		else if (104<speed && speed<210)	MotorConfig[axisNum].AMax=speed*3;		
//		else 															MotorConfig[axisNum].AMax=2000;	
//		SetAMax(axisNum, MotorConfig[axisNum].AMax);
//	}

	else if(MotorConfig[axisNum].PulseDiv==7&&	MotorConfig[axisNum].RampDiv==10)
	{
		if (speed<25)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}			
		else if (speed<100)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+10);	MotorConfig[axisNum].AMax=speed+10;}
		else if (speed<150)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+30);MotorConfig[axisNum].AMax=speed+30;}
		else if (speed<280)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+60);MotorConfig[axisNum].AMax=speed+60;}
		else if (speed<420)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+100);MotorConfig[axisNum].AMax=speed+100;}
		else if (speed<560)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+150);MotorConfig[axisNum].AMax=speed+150;}
		else if (speed<700)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+200);MotorConfig[axisNum].AMax=speed+200;}
		else if (speed<840)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+250);MotorConfig[axisNum].AMax=speed+250;}
		else if (speed<980)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+300);MotorConfig[axisNum].AMax=speed+300;}
		else if (speed<=1180)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+350);MotorConfig[axisNum].AMax=speed+350;}
		else if (speed<2047)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+600);MotorConfig[axisNum].AMax=speed+600;}	
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed<25)	//1~24
		{		SetAMax(getTMC429_DEV(1),axisNum, speed);	MotorConfig[axisNum].AMax=speed;}			
		else if (speed<50)
		{		SetAMax(getTMC429_DEV(1),axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}
		else if (speed<100)
		{		SetAMax(getTMC429_DEV(1),axisNum, 100);	MotorConfig[axisNum].AMax=100;}
		else if (speed<150)
		{		SetAMax(getTMC429_DEV(1),axisNum, 500);MotorConfig[axisNum].AMax=500;}
		else if (speed<250)
		{		SetAMax(getTMC429_DEV(1),axisNum, 1000);MotorConfig[axisNum].AMax=1000;}
		else if (speed<2047)
		{		SetAMax(getTMC429_DEV(1),axisNum, 2047);MotorConfig[axisNum].AMax=2047;}	
	}
}

static void TMC429_ParameterPresetToRam(void)
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
		
		//pinMode(homeSensorPin[i], PIN_MODE_INPUT_PULLUP);
	}
}

/***************************************************************//**
  \fn MoveToPosition(void)
  \brief Command MVP

  MVP (Move To Position) command (see TMCL manual).
********************************************************************/
static uint8_t TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
  if(motor_number<N_O_MOTORS)
  {
    if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
    {
			//MotorStopByLimit[motor_number]=1;
			
      if(SpeedChangedFlag[motor_number])
      {
        Write429Short(getTMC429_DEV(1),IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
			  SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
        SpeedChangedFlag[motor_number]=FALSE;
      }
      if(motion_mode==MVP_ABS)
	  {
				rt_kprintf("motor[%d] is start to make absolute motion\n",motor_number);
        Write429Int(getTMC429_DEV(1),IDX_XTARGET|(motor_number<<5), position);
	  }
      else
	 {
				rt_kprintf("motor[%d] is start to make relative motion\n",motor_number);
        Write429Int(getTMC429_DEV(1),IDX_XTARGET|(motor_number<<5), position + Read429Int(getTMC429_DEV(1),IDX_XACTUAL|(motor_number<<5)));
	 }
      Set429RampMode(getTMC429_DEV(1),motor_number, RM_RAMP);
	 return 0;
    }
    else 
	{
		return 1;
	}
  }
  else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
  return 1;
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
        TMC429_ParameterGet.Value.Int32=Read429Int(getTMC429_DEV(1),IDX_XTARGET|(motor_number<<5));				
				CMD_TRACE("motor[%d] next position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
				break;

      case actual_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(getTMC429_DEV(1),IDX_XACTUAL|(motor_number<<5));
				CMD_TRACE("motor[%d] actual position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case next_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(getTMC429_DEV(1),IDX_VTARGET|(motor_number<<5));
			  CMD_TRACE("motor[%d] next speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case actual_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(getTMC429_DEV(1),IDX_VACTUAL|(motor_number<<5));
			  CMD_TRACE("motor[%d] actual speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_v_positioning:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].VMax;
			  CMD_TRACE("motor[%d] max positioning speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_acc:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].AMax;
			  CMD_TRACE("motor[%d] max acceleration=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IRun;
				CMD_TRACE("motor[%d] max current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case standby_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IStandby;
				CMD_TRACE("motor[%d] standby current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case position_reached:
//        if(Read429Status() & 0x01) TMC429_ParameterGet.Value.Byte[0]=1;
//				CMD_TRACE("motor[%d] position reached=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
			
				CMD_TRACE("motor[%d] position reached=%d\n",motor_number,(Read429Status(getTMC429_DEV(1)) & (0x01<<motor_number*2)) ? 1:0);
        break;

      case leftLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(getTMC429_DEV(1),IDX_REF_SWITCHES, 3) & (0x02<<motor_number*2)) ? 1:0;
			  CMD_TRACE("motor[%d] left limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case rightLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(getTMC429_DEV(1),IDX_REF_SWITCHES, 3) & (0x01<<motor_number*2)) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;
			
      case allLimit_SwS:
				{
						uint8_t limit_char[6];
						uint8_t i;
						uint8_t limit = Read429SingleByte(getTMC429_DEV(1),IDX_REF_SWITCHES, 3);
						for(i=0;i<6;i++)
						{
							limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
						}
						CMD_TRACE("motor limit switch status L2R2L1R1L0R0=%.6s\n",limit_char);
				}
        break;

      case rightLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(getTMC429_DEV(1),IDX_REFCONF_RM|(motor_number<<5), 2) & 0x02) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				break;

      case leftLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(getTMC429_DEV(1),IDX_REFCONF_RM|(motor_number<<5), 2) & 0x01) ? 1:0;
				CMD_TRACE("motor[%d] left limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case min_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(getTMC429_DEV(1),IDX_VMIN|(motor_number<<5));
				CMD_TRACE("motor[%d] minimum speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case ramp_mode:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(getTMC429_DEV(1),IDX_REFCONF_RM|(motor_number<<5), 3);
			  CMD_TRACE("motor[%d] ramp mode=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        TMC429_ParameterGet.Value.Int32=Read429Short(getTMC429_DEV(1),MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE);
				CMD_TRACE("motor[%d] ref switch tolerance=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case softStop_flag:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, 0) & 0x04) ? 1:0;
				CMD_TRACE("motor[%d] soft stop status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
			  break;

      case ramp_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(getTMC429_DEV(1),IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) & 0x0f;
		  	CMD_TRACE("motor[%d] ramp divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case pulse_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(getTMC429_DEV(1),IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) >> 4;
				CMD_TRACE("motor[%d] pulse divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

			case is_homed:
				CMD_TRACE("motor[%d] homed=%d\n",motor_number,homeInfo.Homed[motor_number]);
				break;
			case is_stop:
				CMD_TRACE("motor[%d] stop=%d\n",motor_number,Read429Short(getTMC429_DEV(1),IDX_VACTUAL|(motor_number<<5))? 0:1) ;
				break;
			case is_reach:

				CMD_TRACE("motor[%d] reach=%d\n",motor_number,(Read429Status(getTMC429_DEV(1)) & (0x01<<motor_number*2)) ? 1:0);

				break;
			case home_SenS:
				CMD_TRACE("motor[%d] homeSensor=%d\n",motor_number, rt_pin_read(homeSensorPin[motor_number]) ? 0:1) ;
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
        Write429Int(getTMC429_DEV(1),IDX_XTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] next position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_position:
        Write429Int(getTMC429_DEV(1),IDX_XACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] actual position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case next_speed:
        Write429Short(getTMC429_DEV(1),IDX_VTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] next speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_speed:
        Write429Short(getTMC429_DEV(1),IDX_VACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] actual speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_v_positioning:
        MotorConfig[motor_number].VMax=TMC429_ParameterSet.Value.Int32;
        Write429Short(getTMC429_DEV(1),IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
				CMD_TRACE("set motor[%d] speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_acc:
        MotorConfig[motor_number].AMax=TMC429_ParameterSet.Value.Int32;
        SetAMax(getTMC429_DEV(1),motor_number, MotorConfig[motor_number].AMax);
				CMD_TRACE("set motor[%d] max acceleration speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case rightLimit_disable:	//12
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart_0.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x02;
        else
          Write[2]=Read[2]&  ~0x02;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
				if(CMDGetFromUart_0.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] right limit switch is disable\n",motor_number);	
				else 
					CMD_TRACE("motor[%d] right limit switch is enable\n",motor_number);	
        break;

      case leftLimit_disable: //13
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart_0.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x01;
        else
          Write[2]=Read[2]&  ~0x01;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
				if(CMDGetFromUart_0.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] left limit switch is disable\n",motor_number);	
				else 
					CMD_TRACE("motor[%d] left limit switch is enable\n",motor_number);	
        break;

      case min_speed:
        Write429Short(getTMC429_DEV(1),IDX_VMIN|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] minimum speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_mode:
        Set429RampMode(getTMC429_DEV(1),motor_number, TMC429_ParameterSet.Value.Byte[0]);
				CMD_TRACE("set motor[%d] ramp mode=%d ok\n",motor_number,TMC429_ParameterSet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        Write429Short(getTMC429_DEV(1),MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE, TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] ref switch tolerance=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
			  break;

      case softStop_flag:
        Read429Bytes(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
        if(TMC429_ParameterSet.Value.Int32!=0)
          Read[1]|=0x04;
        else
          Read[1]&= ~0x04;
        Write429Bytes(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
				CMD_TRACE("set motor[%d] soft stop status=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (CMDGetFromUart_0.Value.Byte[0] & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        MotorConfig[motor_number].RampDiv=CMDGetFromUart_0.Value.Byte[0] & 0x0f;
				CMD_TRACE("set motor[%d] ramp divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case pulse_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (CMDGetFromUart_0.Value.Byte[0] << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(getTMC429_DEV(1),Read, Write);
        MotorConfig[motor_number].PulseDiv=CMDGetFromUart_0.Value.Byte[0]& 0x0f;
				CMD_TRACE("set motor[%d] pulse divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;
			case limitSignal:
				if(TMC429_ParameterSet.Value.Int32!=0) 
				{
				  Write429Int(getTMC429_DEV(1),IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
				}
				else Write429Int(getTMC429_DEV(1),IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
				CMD_TRACE("set limit signal effective trigger level=%d ok\n",TMC429_ParameterSet.Value.Int32);
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
		rt_kprintf("motor stop reset                      - motor reset\n");	
		rt_kprintf("motor reset?                          - is axis reset or not\n");		
}
static void printf_cmdList_motor_set(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor set speed <axis> <value>             -set the rotate speed \n");				
		CMD_TRACE("motor set next_position <axis> <value>      -set the target position to move\n");
		CMD_TRACE("motor set actual_position <axis> <value>    -set the current position\n");				
		CMD_TRACE("motor set actual_speed <axis> <value>       -set the current speed \n");
		CMD_TRACE("motor set VMAX <axis> <value>               -set max positioning speed\n");
		CMD_TRACE("motor set AMAX <axis> <value>               -set max acceleration\n");
		CMD_TRACE("motor set ramp_div <axis> <value>           -set ramp divisor value\n");
		CMD_TRACE("motor set pulse_div <axis> <value>          -set pulse divisor value\n");	
		CMD_TRACE("motor set rightLimit <axis> <value>         -set right limit switch\n");
		CMD_TRACE("motor set leftLimit <axis> <value>          -set left limit switch\n");
		CMD_TRACE("motor set limitSignal <axis> <value>        -set limit signal effective trigger level\n");		
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
	CMD_TRACE("motor get next_speed <axis>        -get the target speed \n");
	CMD_TRACE("motor get next_position <axis>     -get the target position to move\n");
	
	CMD_TRACE("motor get VMAX <axis>              -get max positioning speed\n");
	CMD_TRACE("motor get AMAX <axis>              -get max acceleration\n");

	CMD_TRACE("motor get position_reached <axis>  -is reach positon or not\n");
	CMD_TRACE("motor get ramp_div <axis>          -get ramp divisor value\n");
	CMD_TRACE("motor get pulse_div <axis>         -get pulse divisor value\n");			
	
}
/*********************************************************************************************************************/
void Init429_0(void)
{
  UINT addr;
  UCHAR Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write429Zero(getTMC429_DEV(1),addr|(Motor<<5));
  }

	Write429Int(getTMC429_DEV(1),IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);

	Write429Datagram(getTMC429_DEV(1),IDX_SMGP, 0x00, 0x04, 0x02);

  Write429Datagram(getTMC429_DEV(1),IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x04);
  Write429Datagram(getTMC429_DEV(1),IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x04);
  Write429Datagram(getTMC429_DEV(1),IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x04);
  Write429Datagram(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
  Write429Datagram(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
  Write429Datagram(getTMC429_DEV(1),IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
  Write429Short(getTMC429_DEV(1),IDX_VMIN|MOTOR0, 1);
  Write429Short(getTMC429_DEV(1),IDX_VMIN|MOTOR1, 1);
  Write429Short(getTMC429_DEV(1),IDX_VMIN|MOTOR2, 1);

  Write429Int(getTMC429_DEV(1),IDX_VMAX|MOTOR0, MotorConfig[0].VMax);
  SetAMax(getTMC429_DEV(1),0, MotorConfig[0].AMax);
  Write429Int(getTMC429_DEV(1),IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(getTMC429_DEV(1),1, MotorConfig[1].AMax);
  Write429Int(getTMC429_DEV(1),IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(getTMC429_DEV(1),2, MotorConfig[2].AMax);
	
}
void  tmc429_hw_init_0(void)
{
	pinMode(POSCMP1_PIN, PIN_MODE_INPUT_PULLUP);
  pinMode(INTOUT1_PIN, PIN_MODE_INPUT_PULLUP);	
	
//	MX_SPI1_Init();
	
#if defined(USING_INC_MBTMC429) 
	MX_TIM3_Init();	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
#endif
	
	TMC429_ParameterPresetToRam();
	
	Init429_0();

//板卡上电后已当前位置为原点，正常应用需要回原点动作
	for(uint8_t i=0;i<3;i++)
	{
		HardStop(getTMC429_DEV(1),i);
		delay_ms(50);
		Write429Short(getTMC429_DEV(1),IDX_VMAX|(i<<5), 0);	
		delay_ms(50);
		Write429Int(getTMC429_DEV(1),IDX_XTARGET|(i<<5), 0);
		Write429Int(getTMC429_DEV(1),IDX_XACTUAL|(i<<5), 0);
	}
}

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
		
    Set429RampMode(getTMC429_DEV(1),motor_number, RM_VELOCITY);
		
		SetAmaxAutoByspeed(motor_number,abs(motor_speed));
	  
    Write429Short(getTMC429_DEV(1),IDX_VMAX|(motor_number<<5), 2047);
	  
    Write429Short(getTMC429_DEV(1),IDX_VTARGET|(motor_number<<5), motor_speed);
	

	rt_kprintf("motor%d is rotate at %d\n",motor_number,motor_speed);
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
		HardStop(getTMC429_DEV(1),motor_number);
		//StopMotorByRamp(motor_number);
		/* 回原点过程中响应停止命令的变量置位 */
		homeInfo.GoHome[motor_number]=0;
		homeInfo.GoLimit[motor_number]=0;
		/* 电机停止会自动返回当前位置 */
		rt_kprintf("motor[%d] is stop and P[%d]=%d\n",motor_number,motor_number,Read429Int(getTMC429_DEV(1),IDX_XACTUAL|(motor_number<<5)));
		return 0;
  }
	return 1;
}
void StopMotorByRamp(UINT Motor)
{
  Set429RampMode(getTMC429_DEV(1),MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero(getTMC429_DEV(1),(MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
}

uint8_t SPI_LockByCMD=0;

/***************************************************************//**
Load等项目一直使用的命令与参数：电机速度使用的是IC的寄存器值;命令中封装一些动作

motor reset											 		---	采集治具是否处于复位状态中
motor reset？											  ---	控制治具开始进行复位动作，复位时不响应电机其他动作命令
motor stop reset     							  ---	复位时只响应此命令

motor get <type> <axis>   			 		---	获取电机相关参数
		
	speed					|	position			|		limit all			| 	rightLimit				|		leftLimit
	is_homed			|	is_stop				|		is_reach			|		homeSensor				|		next_speed
	next_position	|	VMAX					|		AMAX					|		position_reached	|		ramp_div	|		pulse_div

motor set <type> <axis> <value>  		---	配置电机相关参数

	speed			| next_position	| actual_position	| actual_speed	|	VMAX				|				AMAX	
	ramp_div	|	pulse_div			|	rightLimit			|	leftLimit			|	limitSignal

motor stop <axisNum>              	--- 电机停止
motor rotate <axisNum> <speed>      --- 电机已设定的速度进行旋转运动	
motor moveto <axisNum> <position>   --- 电机绝对运动到位置
motor move <axisNum> <position>     --- 电机进行相对运动到


motor gohome <axisNum> <speed> 	 		--- 电机回原点点位
motor golimit <axisNum> <speed>	 		--- 电机回相位点位
********************************************************************/

int motor_0(int argc, char **argv)
{
	 CMDGetFromUart_0.Type=Type_none;
	
	 TMC429_ParameterGet.Status=REPLY_OK;
	
    int result = RT_ERROR;
		
		if (argc == 2 )
		{
			if (!strcmp(argv[1], "reset?"))
			{
				//CMD_TRACE("motor reset=%d\n",motorsReset_InOrder==OK_2 ? 1:0) ;
				return RT_EOK	; 				
			}
			else	if (!strcmp(argv[1], "reset"))
			{
				//MotorAutoReset_preset();
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
		if (argc == 3 )
		{
			CMDGetFromUart_0.Motor=atoi(argv[2]);
			if (!strcmp(argv[1], "stop"))
			{
				if (!strcmp(argv[2], "reset"))
				{
					//buttonRESETpressed=FALSE;
					homeInfo.GoHome[0]=0;
					homeInfo.GoHome[1]=0;
					homeInfo.GoHome[2]=0;
					StopMotorByRamp(0);StopMotorByRamp(1);StopMotorByRamp(2);return RT_EOK;//TMC429_MotorStop(2);
				}
				//if(buttonRESETpressed==TRUE || homeInfo.GoHome[2]==1) {CMD_TRACE("motor is reseting\n");return RT_EOK;}
				if(CMDGetFromUart_0.Motor==AXIS_Z) 
				{	
					//closeSerial();
				}	

				if(!TMC429_MotorStop(CMDGetFromUart_0.Motor))	//电机停止后自动会关闭串口打印压力
				{
//					homeInfo.GoHome[0]=FALSE;		homeInfo.GoHome[1]=FALSE;		homeInfo.GoHome[2]=FALSE;	
//					homeInfo.GoLimit[0]=FALSE;	homeInfo.GoLimit[1]=FALSE;	homeInfo.GoLimit[2]=FALSE;						
//					buttonRESETpressed=FALSE;								  //只要有轴停止啊，就可以开启下次复位功能，复位过去被停止的话									
					return RT_EOK	;
				}
				result = RT_EINVAL;
			}
		}
		else if (argc == 4)
		{	
			CMDGetFromUart_0.Motor=atoi(argv[2]);
			CMDGetFromUart_0.Value.Int32=atoi(argv[3]);
			if (!strcmp(argv[1], "get"))
			{
				if (!strcmp(argv[2], "speed")) 	 					CMDGetFromUart_0.Type=actual_speed;
				if (!strcmp(argv[2], "position")) 				CMDGetFromUart_0.Type=actual_position;
				if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart_0.Type=is_homed;
				if (!strcmp(argv[2], "is_stop")) 					CMDGetFromUart_0.Type=is_stop;
				if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart_0.Type=is_reach;
			
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart_0.Type=next_speed;
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart_0.Type=next_position;		
				if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart_0.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart_0.Type=max_acc;
				if (!strcmp(argv[2], "position_reached")) CMDGetFromUart_0.Type=position_reached;		
				
				if (!strcmp(argv[2], "rightLimit")) 			CMDGetFromUart_0.Type=rightLimit_SwS;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart_0.Type=leftLimit_SwS;
				if (!strcmp(argv[2], "limit")) 			      CMDGetFromUart_0.Type=allLimit_SwS;
				
				if (!strcmp(argv[2], "homeSensor")) 			CMDGetFromUart_0.Type=home_SenS;
				
				if (!strcmp(argv[2], "rightLimit?"))			CMDGetFromUart_0.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit?")) 			CMDGetFromUart_0.Type=leftLimit_disable;
				
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart_0.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart_0.Type=pulse_divisor;	
				
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart_0.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart_0.Type=standby_current;
//				if (!strcmp(argv[2], "Llimit_off")) 			CMDGetFromUart_0.Type=leftLimit_disable;
				
				if(CMDGetFromUart_0.Type!=Type_none)
				{
					CMDGetFromUart_0.Motor=atoi(argv[3]);
					TMC429_GetAxisParameter(CMDGetFromUart_0.Motor,CMDGetFromUart_0.Type);
					if(TMC429_ParameterGet.Status==REPLY_OK) return RT_EOK;
					result=RT_EINVAL;
				}
				else 
				{
					printf_cmdList_motor_get();
					result =REPLY_INVALID_CMD;
				}
			}		
			//报警后所有的正向运动都屏蔽
//			if(pressureAlarm == TRUE && CMDGetFromUart_0.Value.Int32>0 && CMDGetFromUart_0.Motor==2 )  
//			{
//				CMD_TRACE("pressure overhigh, motor z is forbidden to move downward\n");	
//				return RT_EOK;				
//			}
			//复位状态下屏蔽电机运行控制
//			if(buttonRESETpressed==TRUE )
//			{	
//				CMD_TRACE("motor is resetting\n");
//				return RT_EOK;
//			}
			else 
			{
				if (!strcmp(argv[1], "rotate"))
				{
					if(!(TMC429_MotorRotate(CMDGetFromUart_0.Motor,CMDGetFromUart_0.Value.Int32)))	return RT_EOK	;
					result = RT_EINVAL;
				}
				if (!strcmp(argv[1], "move") )
				{			
						if(!TMC429_MoveToPosition(CMDGetFromUart_0.Motor,  MVP_REL, CMDGetFromUart_0.Value.Int32))	return RT_EOK;
						result =RT_EINVAL;
				}
				if (!strcmp(argv[1], "moveto"))
				{
//					if(g_tParam.Project_ID==OQC_FLEX || g_tParam.Project_ID==BUTTON_ROAD)
//					{
//						if( (pressureAlarm == TRUE || INPUT8==0 )  &&  CMDGetFromUart_0.Motor==2 )  
//						{
//							if(CMDGetFromUart_0.Value.Int32 > Read429Int(IDX_XACTUAL|(CMDGetFromUart_0.Motor<<5)))
//							{
//								CMD_TRACE("pressure overhigh, motor z is forbidden to move downward\n");	
//								return RT_EOK;				
//							}
//						}
//					}
//					else if(g_tParam.Project_ID==EOK_POWE_2AXIS)
//					{
//						if( (pressureAlarm == TRUE || INPUT8==0 )  &&  CMDGetFromUart_0.Motor==1 )  
//						{
//							if(CMDGetFromUart_0.Value.Int32 > Read429Int(IDX_XACTUAL|(CMDGetFromUart_0.Motor<<5)))
//							{
//								CMD_TRACE("pressure overhigh, motor y is forbidden to move downward\n");	
//								return RT_EOK;				
//							}
//						}
//					}
//					closeSerial();
					if(!TMC429_MoveToPosition(CMDGetFromUart_0.Motor, MVP_ABS, CMDGetFromUart_0.Value.Int32))	return RT_EOK;
					result =RT_EINVAL;
				}
				if (!strcmp(argv[1], "gohome"))
				{
						//closeSerial();
						
						//Stop_HardTimer();		
			
						homeInfo.GoHome[CMDGetFromUart_0.Motor] = TRUE;
						homeInfo.GoLimit[CMDGetFromUart_0.Motor]= FALSE;
						homeInfo.Homed[CMDGetFromUart_0.Motor]	= FALSE;

						homeInfo.HomeSpeed[CMDGetFromUart_0.Motor]=CMDGetFromUart_0.Value.Int32;	
					
						SetAmaxAutoByspeed(CMDGetFromUart_0.Motor,abs(CMDGetFromUart_0.Value.Int32));
			
						TMC429_MotorRotate(CMDGetFromUart_0.Motor,CMDGetFromUart_0.Value.Int32);
						
						CMD_TRACE("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart_0.Motor);
										
						//Start_HardTimer();
					
						return RT_EOK	;
				}
				if (!strcmp(argv[1], "golimit"))
				{			
						//Stop_HardTimer();					
						homeInfo.GoHome[CMDGetFromUart_0.Motor]	=FALSE;
						homeInfo.GoLimit[CMDGetFromUart_0.Motor]=TRUE;
						homeInfo.Homed[CMDGetFromUart_0.Motor]	=FALSE;
						homeInfo.HomeSpeed[CMDGetFromUart_0.Motor]=CMDGetFromUart_0.Value.Int32;	
						SetAmaxAutoByspeed(CMDGetFromUart_0.Motor,abs(CMDGetFromUart_0.Value.Int32));
						TMC429_MotorRotate(CMDGetFromUart_0.Motor,CMDGetFromUart_0.Value.Int32);
						//Start_HardTimer();					
						return RT_EOK	;
				}
			}	

		}
		else if (argc == 5)
		{
			if (!strcmp(argv[1], "set"))
			{
				//user
				if (!strcmp(argv[2], "speed")) 					 	CMDGetFromUart_0.Type=max_v_positioning;
					
				//debug
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart_0.Type=next_speed;		
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart_0.Type=next_position;
				if (!strcmp(argv[2], "actual_position")) 	CMDGetFromUart_0.Type=actual_position;
				if (!strcmp(argv[2], "actual_speed")) 	 	CMDGetFromUart_0.Type=actual_speed;
				if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart_0.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart_0.Type=max_acc;
				//设置限位失能无效，需要调试原因
				if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart_0.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart_0.Type=leftLimit_disable;	  		
				if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart_0.Type=limitSignal;		
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart_0.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart_0.Type=pulse_divisor;
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart_0.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart_0.Type=standby_current;
				if(CMDGetFromUart_0.Type!=Type_none)	
				{
					CMDGetFromUart_0.Motor=atoi(argv[3]);
					CMDGetFromUart_0.Value.Int32=atoi(argv[4]);
					TMC429_SetAxisParameter(CMDGetFromUart_0.Motor, CMDGetFromUart_0.Type, CMDGetFromUart_0.Value.Int32);
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
			rt_kprintf("motor number must be 0~2\n",CMDGetFromUart_0.Motor);
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
			uint8_t SwitchStatus=Read429SingleByte(getTMC429_DEV(1),IDX_REF_SWITCHES, 3);				//限位状态读取
			//CMD_TRACE("%d GoHome=%d,GoLimit=%d,SwitchStatus=%d,V=%d\n",motor_number,homeInfo.GoHome[motor_number],homeInfo.GoLimit[motor_number],SwitchStatus,Read429Short(IDX_VACTUAL|(motor_number<<5)));

			//左				需要再判断速度													右              //碰到限位停止，需要再次发命令运动
			if(((((SwitchStatus& (0x02<<motor_number*2))?1:0) ==1)	||  (((SwitchStatus& (0x01<<motor_number*2)) ?1:0)==1))	\
				   && ( Read429Short(getTMC429_DEV(1),IDX_VACTUAL|(motor_number<<5))==0 )	)						 
			{					
					
					TMC429_MotorStop(motor_number);		MotorStopByLimit[motor_number]=0;
					//CMD_TRACE("MotorStop(%d) is act\n",axisNum);
			}		
		}
	}
}
//MSH_CMD_EXPORT(test,..);
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



