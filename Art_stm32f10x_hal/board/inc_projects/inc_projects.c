
#include "inc_projects.h"

#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"

#include "inc_dido.h"

#include "fbio0808.h"	
#include "FBJFFB1615CN1.h"	
#include "fbholyrelay.h"

#include "app_eeprom_24xx.h"
#include "app_tmc429.h"	

#define LED_RED_ON() 							GPIOD->BSRR=BIT6;	GPIOD->BRR=BIT5|BIT4
#define LED_GREEN_ON() 						GPIOD->BSRR=BIT5;	GPIOD->BRR=BIT4|BIT6
#define LED_BLUE_ON() 						GPIOD->BSRR=BIT4;	GPIOD->BRR=BIT5|BIT6
#define LEDS_OFF() 								GPIOD->BRR=BIT4|BIT5|BIT6


#define PROJECT_USED 'Y'
/* -----------------------------------------------------------------------*/
uint8_t ProjectUsed='N';
uint8_t WhichProject=COMMON_USE;

uint8_t IDC_DEV[6]={FBIO0808,FBTMC429,FB29125,FB29125,NONE,NONE};

/* -----------------------------------------------------------------------*/
int ProjectAppThreadInit(void);

void NMEI_IN3COLA_init(void );
void NMEI_IN3VEGE_init(void );

void NMEI_IN3COLA_AppRun(void);
/* -----------------------------------------------------------------------*/

u8 	KEY_START_pressed=0;
u8 	KEY_RESET_pressed=0;

/* -----------------------------------------------------------------------*/
/*	NMEI-IN3COLA			MP Coil LCR

IO 	Config

IN1 双启动									OUT1	双启灯
IN2	Y轴报警									OUT2	Z轴抱闸打开
IN4 安全光栅-NC							OUT3	Y轴使能
IN6 Y轴原点									OUT4	Y轴报警清除
IN7 Z轴原点
IN8 Y轴电机负方向运动限制
*/
#if	1

#define AXIS_Y_limitLCh	8				//输入口8为Y轴的软限位开关 非回原点动作下 触发时 光耦不导通 GPIO=HIGH    其他普通输入 触发时GPIO=L

void NMEI_IN3COLA_init(void )
{

}

void NMEI_IN3COLA_AppRun(void)	//Y轴增加软限位功能
{
		uint8_t	in8_LimitL_detected_cnt=0;
		
    while (1)
    {
				if(inputGet(AXIS_Y_limitLCh)==1	) 	//limitLeft
				{
					in8_LimitL_detected_cnt++;
					if(  in8_LimitL_detected_cnt>3 )
					{
						if(motorHoming.GoHome[1]==TRUE	||	 motorHoming.GoLimit[1]==TRUE) 
						{
							
						}						
						else if (Read429Short(getTMC429_DEV(1),IDX_VACTUAL|(1<<5))<0)
						{
							HardStop(getTMC429_DEV(1),1);
						}
					}
				}
				else 
				{
					in8_LimitL_detected_cnt=0;
				}
				rt_thread_delay(5);
    }		
}
#else
void NMEI_IN3COLA_init(void )
{

}
#endif
/* -----------------------------------------------------------------------*/


/* -----------------------------------------------------------------------*/


/* -------------------BUTTON-ONLINE-----------------------------------------

***************************************************************************/
#if 1
void BUTTON_ONLINE_init(void )
{

}
//inputGet(2) - RESET
void BUTTON_ONLINE_AppRun(void )
{
		uint8_t	in2_RESET_detected_cnt=0;
		
    while (1)
    {
				if(inputGet(2)==0	&& KEY_RESET_pressed==0 ) 	
				{
					in2_RESET_detected_cnt++;
					if(  in2_RESET_detected_cnt>3 )
					{
							/*RL Y	Y轴先复位		Y轴先后后前
								RL Z	Z轴再复位		Z轴先上后下
								RR X 	X轴再复位		X轴先左后右
							*/
							KEY_RESET_pressed=1; outputSet(2,1); LED_BLUE_ON();
							motor_gohome_config(1, ChangeSpeedPerSecondToVMax(-1600));							
					}
				}
				else 
				{
					in2_RESET_detected_cnt=0;
				}
				if(motorHoming.Homed[0]	==	TRUE && KEY_RESET_pressed==1)
				{
					motor_gohome_config(2, ChangeSpeedPerSecondToVMax(-1600));		
					motor_gohome_config(0, ChangeSpeedPerSecondToVMax(1600));							
				}
				if(KEY_RESET_pressed==1 && motorHoming.Homed[0]	==	TRUE && motorHoming.Homed[1]	==	TRUE && motorHoming.Homed[2]	==	TRUE)
				{
					outputSet(2,0);	LED_GREEN_ON(); 	KEY_RESET_pressed=0;
					rt_kprintf("fixture reset ok\n");
				}
				rt_thread_delay(5);
    }		
	
}
#endif
/* -----------------------------------------------------------------------*/
/* -------------------BUTTON-OFFLINE-----------------------------------------

***************************************************************************/
#if 1

//20180118
void Cylinder_Reset_check(void)
{
	U32_T DebugDelay;
	DebugDelay=HAL_GetTick();
	rt_kprintf("\nCylinder resetting...\n");
	outputSet(5,0);	outputSet(6,1);
	delay_ms(100);
	while(	(inputGet(8)==1 && inputGet(9)==0) ? 0:1 )	
	{
		if(abs((int)(HAL_GetTick()-DebugDelay))>1000) 
		{
			rt_kprintf("Please check pogo pin cylinder is >>BACK<< :  OUT6=1,OUT5=0;    IN8=0,IN9=1\n");
			LED_RED_ON();
			DebugDelay=HAL_GetTick();
		}
	}
	outputSet(8,0);	outputSet(7,1);	delay_ms(100);
	while((inputGet(11)==1 && inputGet(10)==0)  	? 0:1 )	
	{
		if(abs((int)(HAL_GetTick()-DebugDelay))>1000) 
		{
			rt_kprintf("Please check pogo pin cylinder is >>UPSIDE<< : OUT7=1,OUT8=0;   IN11=0,IN10=1\n");
			LED_RED_ON();
			DebugDelay=HAL_GetTick();
		}
	}
	outputSet(3,0);	outputSet(4,1);	delay_ms(100);
	while((inputGet(3)==1 && inputGet(4)==0) 		? 0:1 )	
	{
		if(abs((int)(HAL_GetTick()-DebugDelay))>1000) 
		{
			rt_kprintf("Please check dut cylinder is >>OUTSIDE<< : OUT4=1,OUT3=0;    IN3=0,IN4=1\n");
			LED_RED_ON();
			DebugDelay=HAL_GetTick();
		}
	}
	LED_BLUE_ON();
}
//

void BUTTON_OFFLINE_init(void )
{
	motorSetting.limit_level_valid=1;          
	motorSetting.orgin_level_valid=0;  
	OriginSensorON[0]=LOW;	OriginSensorON[1]=LOW;	OriginSensorON[2]=LOW;
	OriginSensorON[3]=LOW;	OriginSensorON[4]=LOW;	OriginSensorON[5]=LOW;			

}
void BUTTON_OFFLINE_AppRun(void )
{
	
}
void BUTTON_OFFLINE_AppRun1(void )
{
		uint8_t	in2_RESET_detected_cnt=0;
		
    while (1)
    {
				if(inputGet(2)==0	&& KEY_RESET_pressed==0 ) 	
				{
					in2_RESET_detected_cnt++;
					if(  in2_RESET_detected_cnt>3 )
					{
							/*RL Y	Y轴先复位		Y轴先后后前
								RL Z	Z轴再复位		Z轴先上后下
								RR X 	X轴再复位		X轴先左后右
							*/
							KEY_RESET_pressed=1; outputSet(2,1); LED_BLUE_ON();
						
							Cylinder_Reset_check();
							motor_gohome_config(1, ChangeSpeedPerSecondToVMax(-1600));							
					}
				}
				else 
				{
					in2_RESET_detected_cnt=0;
				}
				if(motorHoming.Homed[0]	==	TRUE && KEY_RESET_pressed==1)
				{
					motor_gohome_config(2, ChangeSpeedPerSecondToVMax(-1600));		
					motor_gohome_config(0, ChangeSpeedPerSecondToVMax(1600));							
				}
				if(KEY_RESET_pressed==1 && motorHoming.Homed[0]	==	TRUE && motorHoming.Homed[1]	==	TRUE && motorHoming.Homed[2]	==	TRUE)
				{
					outputSet(2,0);	LED_GREEN_ON(); 	KEY_RESET_pressed=0;
					rt_kprintf("fixture reset ok\n");
				}
				rt_thread_delay(5);
    }				
}
#endif


/* -----------------------------------------------------------------------*/
/* -------------------BUTTON-LIDOPEN-----------------------------------------

***************************************************************************/
#if 1

#define DutGoInside()		  outputSet(3,0);outputSet(4,1)
#define DutGoOutside()		outputSet(3,1);outputSet(4,0)
#define CloseTheDoor()		outputSet(1,1);outputSet(2,0)
#define OpenTheDoor()			outputSet(1,0);outputSet(2,1)

#define PB_START				inputGet(1)
#define PRESS						0

#define STANDBY 				0
#define DUT_GO_INSIDE 	1
#define CLOSE_DOOR 			2
#define OPEN_DOOR 			3
#define DUT_GO_OUTSIDE 	4

#define	INSIDE					0
#define	OUTSIDE					1
#define	UNDEFINE				3
#define	UPSIDE					0
#define DOWNSIDE				1

uint8_t	OpStep=STANDBY;
uint8_t	KeyS_actOK_LidOpen=0;
uint8_t KeyS_actEN_LidOpen=0;

static UCHAR DutPosition(void)
{
	if		 (inputGet(2)==1 && inputGet(3)==0) return INSIDE;
	else if(inputGet(2)==0 && inputGet(3)==1) return OUTSIDE;	
	else {return UNDEFINE;}
}

static UCHAR DoorPosition(void)
{
	if		 (inputGet(9)==0 && inputGet(10)==1) return UPSIDE;
	else if(inputGet(9)==1 && inputGet(10)==0) return DOWNSIDE;
	else {return UNDEFINE;}	
}

void KeyS_act_LIDOPEN(void)												
{
	switch(OpStep)						
	{	
		case STANDBY:
			if(DutPosition()==OUTSIDE && DoorPosition()==UPSIDE )					
			{
				if(PB_START==PRESS) 																								
				{	
					delay_ms(20);
					if(PB_START==PRESS)	OpStep=DUT_GO_INSIDE;					
				}			
			}	
			break;
			
		case DUT_GO_INSIDE:	
			if(PB_START==PRESS)	
			{	
				 outputSet(7,1);DutGoInside();delay_ms(100);
				 if(DutPosition()==INSIDE )	OpStep=CLOSE_DOOR;	
			}
			else	OpStep=DUT_GO_OUTSIDE;
			break;	
			
		case CLOSE_DOOR:
			if(PB_START==PRESS)	
			{	
				 outputSet(7,1);CloseTheDoor();delay_ms(100);
				 if(DoorPosition()==DOWNSIDE )	
				 {
					 KeyS_actEN_LidOpen=0;KeyS_actOK_LidOpen=1;OpStep=STANDBY;
					 rt_kprintf("ready to test\n>>");
				 }
			}
			else	OpStep=OPEN_DOOR;
			break;
			
		case OPEN_DOOR:
			if(PB_START!=PRESS)	
			{
				 outputSet(7,0);OpenTheDoor();delay_ms(100);
				 if(DoorPosition()==UPSIDE) OpStep=DUT_GO_OUTSIDE;
				//else OpenTheDoor();	
			}
			else OpStep=CLOSE_DOOR;
			break;
			
		case DUT_GO_OUTSIDE:		
			if(PB_START!=PRESS)	
			{	
				 outputSet(7,0);DutGoOutside();delay_ms(100);
				 if(DutPosition()==OUTSIDE) OpStep=STANDBY;
				 else DutGoOutside();
			}
			else	OpStep=DUT_GO_INSIDE;
			break;
			
		default:
			break;		
	}
}



void LID_OPEN_init(void )
{
	

}
void LID_OPEN_AppRun(void )
{
		uint8_t	in2_RESET_detected_cnt=0;
		
    while (1)
    {
			
				KeyS_act_LIDOPEN();
			
				if(inputGet(2)==0	&& KEY_RESET_pressed==0 ) 	
				{
					in2_RESET_detected_cnt++;
					if(  in2_RESET_detected_cnt>3 )
					{
							/*RL Y	Y轴先复位		Y轴先后后前
								RL Z	Z轴再复位		Z轴先上后下
								RR X 	X轴再复位		X轴先左后右
							*/
							KEY_RESET_pressed=1; outputSet(2,1); LED_BLUE_ON();
						
							Cylinder_Reset_check();
							motor_gohome_config(1, ChangeSpeedPerSecondToVMax(-1600));							
					}
				}
				else 
				{
					in2_RESET_detected_cnt=0;
				}
				if(motorHoming.Homed[0]	==	TRUE && KEY_RESET_pressed==1)
				{
					motor_gohome_config(2, ChangeSpeedPerSecondToVMax(-1600));		
					motor_gohome_config(0, ChangeSpeedPerSecondToVMax(1600));							
				}
				if(KEY_RESET_pressed==1 && motorHoming.Homed[0]	==	TRUE && motorHoming.Homed[1]	==	TRUE && motorHoming.Homed[2]	==	TRUE)
				{
					outputSet(2,0);	LED_GREEN_ON(); 	KEY_RESET_pressed=0;
					rt_kprintf("fixture reset ok\n");
				}
				rt_thread_delay(5);
    }				
}
#endif

/* -----------------------------------------------------------------------*/
/* -------------------ATLAS-QT1-----------------------------------------

***************************************************************************/
#if 1

//IN1 上料正面传感器			OUT7 	P1-DOWN气缸伸出		OUT15 X气缸伸出
//IN2 上料侧面传感器			OUT8 	P1-DOWN气缸缩回	  OUT16 X气缸缩回
//IN3 下料正面传感器			OUT9 	P2-DOWN气缸伸出	
//IN4 下料侧面传感器			OUT10 P2-DOWN气缸缩回
//												OUT11 P3-DOWN气缸伸出
//                        OUT12 P3-DOWN气缸缩回
//IN12 出料光纤检测
//上面5个sensor，只要一个感应到了，就停止X轴的输出
//第2点： 检测盖子sensor（IN5）感应到了，OUT7~OUT12 停止输出。 恢复到未检测状态时，OUT7~OUT12 恢复之前的输出状态
//这些都加上使能开关
#define CylinderX_GoRight()			outputSet(15,1);	outputSet(16,0)
#define CylinderX_Stop()				outputSet(15,0);	outputSet(16,0)
#define CylinderX_GoLeft()			outputSet(15,0);	outputSet(16,1)

#define CylinderP1_GoUp()				outputSet(7,1);	outputSet(8,0)
#define CylinderP1_Stop()     	outputSet(7,0);	outputSet(8,0)
#define CylinderP1_GoDown()     outputSet(7,0);	outputSet(8,1)

#define CylinderP2_GoUp()				outputSet(9,1);	outputSet(10,0)
#define CylinderP2_Stop()    		outputSet(9,0);	outputSet(10,1)
#define CylinderP2_GoDown()     outputSet(9,0);	outputSet(10,1)

#define CylinderP3_GoUp()				outputSet(11,1);	outputSet(12,0)
#define CylinderP3_Stop()				outputSet(11,0);	outputSet(12,0)
#define CylinderP3_GoDown()     outputSet(11,0);	outputSet(12,1)

uint8_t OutStaus[8]={0,0,0,0,0,0,0,0};

uint8_t CylinderX_App_EN=0;
uint8_t LidDet_App_EN=0;

void GetOutputs_Status(void)
{
	OutStaus[0]=outputGet(7);		OutStaus[1]=outputGet(8);		OutStaus[2]=outputGet(9);		OutStaus[3]=outputGet(10);
	OutStaus[4]=outputGet(11);	OutStaus[5]=outputGet(12);	OutStaus[6]=outputGet(15);	OutStaus[7]=outputGet(16);
	
}
void CylinderX_App(void)
{
	GetOutputs_Status();
	
}
void ATLAS_QT1_init(void )
{
	IDC_DEV_Init(0,FBJFFB1615CN1);	

}
int appRun(int argc, char **argv)
{
	//appRun safe_sensor en|disable
	//appRun lid_sensor en|disable
}

void ATLAS_QT1_AppRun(void )
{		
    while (1)
    {
			static	uint8_t	Safe_detected_cnt=0;
			
			static	uint8_t	Lid_detected_cnt=0;
			
			static uint8_t CylinderX_AppLock=0;
			
			static uint8_t LidDet_AppLock=0;
			
			if(CylinderX_App_EN==1)
			{
				GetOutputs_Status();
				if( (inputGet(1)==0	|| inputGet(2)==0 || inputGet(3)==0 || inputGet(4)==0 || inputGet(12)==0)	)
				{
					if(CylinderX_AppLock==0)
					{
						Safe_detected_cnt++;
						if( Safe_detected_cnt>3 )
						{				
							CylinderX_Stop();
							CylinderX_AppLock=1;
						}
					}
				}
				else 					
				{
					if(CylinderX_AppLock==1)
					{
						Safe_detected_cnt++;
						if(Safe_detected_cnt>20)	
						{
							Safe_detected_cnt=0;
							CylinderX_AppLock=0;
							outputSet(15,OutStaus[6]);	outputSet(16,OutStaus[7]);					
						}
					}
					else 
					{
						Safe_detected_cnt=0;
					}
				}
				rt_thread_delay(5);
			}				
			else rt_thread_delay(100);

			if(LidDet_App_EN==1)
			{
				GetOutputs_Status();
				if( inputGet(5)==0)
				{
					if(LidDet_AppLock==0)
					{
						Lid_detected_cnt++;
						if( Lid_detected_cnt>3 )
						{				
							CylinderP1_Stop();CylinderP2_Stop();CylinderP3_Stop();
							LidDet_AppLock=1;
						}
					}
				}
				else 					
				{
					if(LidDet_AppLock==1)
					{
						Lid_detected_cnt++;
						if(Lid_detected_cnt>20)	
						{
							Lid_detected_cnt=0;
							LidDet_AppLock=0;
							outputSet(7,OutStaus[0]);	outputSet(8,OutStaus[1]);			
							outputSet(9,OutStaus[2]);	outputSet(10,OutStaus[3]);		
							outputSet(11,OutStaus[4]);	outputSet(12,OutStaus[5]);		
						}
					}
					else 
					{
						Lid_detected_cnt=0;
					}
				}
				rt_thread_delay(5);
			}				
			else rt_thread_delay(100);			
	}
}
#endif


/* -----------------------------------------------------------------------*/
/* -------------------B495-QT1-----------------------------------------

***************************************************************************/
#if 1

void B495_QT1_init(void)
{
	IDC_DEV_Init(0,FBIO0808);
	IDC_DEV_Init(1,FBHOLYRELAY);	
}
void B494_B495_Click_ratio_init(void)
{
	
	motorSetting.limit_level_valid=1;          
	motorSetting.orgin_level_valid=0; 
	
	OriginSensorON[0]=HIGH;	OriginSensorON[1]=HIGH;	OriginSensorON[2]=HIGH;
	OriginSensorON[3]=LOW;	OriginSensorON[4]=LOW;	OriginSensorON[5]=LOW;	
	
	IDC_DEV_Init(0,FBTMC429);
	
}
//
#endif

/* -------------------COMMON_USE-----------------------------------------

***************************************************************************/
#if 1

void COMMON_USE_init(void)
{
//	IDC_DEV_Init(0,FBTMC429);
//	IDC_DEV_Init(1,FBTMC429);	
}
//
#endif

/* -----------------------------------------------------------------------*/

void IDC_DEV_Init(uint8_t whichDev, uint8_t devType)
{
	if(	whichDev < 6)
	{
		IDC_DEV[whichDev] = devType;
	}
	switch(devType)
	{
		case FBIO0808:					FBIO0808_Init();
			break;
		case FBJFFB1615CN1:			FBJFFB1615CN1_Init();
			break;
		case FBHOLYRELAY:				FBHOLYRELAY_Init();
			break;	
		case FBTMC429:			 		tmc429_hw_init();
			break;			
		default:
			break;
	}			
}



void printProjectID(void)
{
	switch(WhichProject)
	{		
		case BUTTON_ONLINE: 	rt_kprintf("ProjectID [BUTTON-ONLINE]\n");	
			break;

		case BUTTON_OFFLINE: 	rt_kprintf("ProjectID [BUTTON-OFFLINE]\n");	
			break;

		case LID_OPEN: 				rt_kprintf("ProjectID [LID-OPEN]\n");	
			break;

		case COMMON_USE: 			rt_kprintf("project select : COMMON-USE\n");	
			break;
		
		case ATLAS_QT1: 			rt_kprintf("project select : ATLAS-QT1\n");	
			break;
		case B495_QT1: 				rt_kprintf("project select : B495-QT1\n");	
			break;
		case CLICK_RATIO: 		rt_kprintf("project select : CLICK-RATIO\n");	
			break;				
		default: 
			break;
	}	
	
}
void ProjectAppInit(void)
{
	at24cxx.read(at24c256 , EEPROM_APP_EN_ADDR, &ProjectUsed, 1);
	at24cxx.read(at24c256 , EEPROM_APP_CS_ADDR, &WhichProject, 1);
	
	if(ProjectUsed==PROJECT_USED)
	{
		if(WhichProject==ATLAS_QT1)
		{
			IDC_DEV_Init(0,FBJFFB1615CN1);			
		}
		else IDC_DEV_Init(0,FBIO0808);
		{
//			OriginSensorON[0]=LOW;	OriginSensorON[1]=LOW;	OriginSensorON[2]=LOW;
//			OriginSensorON[3]=LOW;	OriginSensorON[4]=LOW;	OriginSensorON[5]=LOW;	
//	
//			//IDC_DEV_Init(0,FBTMC429);		
		}

#if 1		
		switch(WhichProject)
		{
			case COMMON_USE: COMMON_USE_init(); 	
				break;
			
			case BUTTON_ONLINE: BUTTON_ONLINE_init();		
				break;

			case CLICK_RATIO: B494_B495_Click_ratio_init();		
				break;

			case LID_OPEN: LID_OPEN_init();		
				break;

			case ATLAS_QT1: ATLAS_QT1_init();		
				break;
			
			case B495_QT1: B495_QT1_init();		
				break;
			
			default: 
				break;
		}
#endif

		ProjectAppThreadInit();
	}
	else
	{
		ProjectUsed=PROJECT_USED;	
		at24cxx.write(at24c256,EEPROM_APP_EN_ADDR,&ProjectUsed,1);
		WhichProject=	COMMON_USE; 
		at24cxx.write(at24c256,EEPROM_APP_CS_ADDR,&WhichProject,1);
	}
	rt_kprintf("\n");
	printProjectID();
}
static struct rt_thread ProjectAppThread;

static rt_uint8_t ProjectAppStack[ 512 ];

static struct rt_thread ProjectAppThread;

static void ProjectAppThread_entry(void *parameter)
{
	switch(WhichProject)
	{
		
		case BUTTON_ONLINE: BUTTON_ONLINE_AppRun(); 
			break;
		
		case BUTTON_OFFLINE: BUTTON_OFFLINE_AppRun(); 
			break;
		
		case LID_OPEN: LID_OPEN_AppRun(); 
			break;
		
		case NMEI_IN3COLA: NMEI_IN3COLA_AppRun(); 
			break;

		case ATLAS_QT1: //ATLAS_QT1_AppRun(); 
			break;
		
		default: rt_thread_delay(100);
			break;
	}
}
int ProjectAppThreadInit(void)
{
    rt_err_t result;

    result = rt_thread_init(&ProjectAppThread,
                            "ProjectApp",
                            ProjectAppThread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&ProjectAppStack[0],
                            sizeof(ProjectAppStack),
                            15,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&ProjectAppThread);
    }
    return 0;
}
/* CMDS:	ProjectID [ProjectID]
*/
int project(int argc, char **argv)
{
	int result = RT_ERROR;
	
	if (argc == 1 )	
	{
		rt_kprintf("usage: \n");
		rt_kprintf("project get       ---Get current project name\n");
		rt_kprintf("project select    ---Select to run related project applications\n");
		
		rt_kprintf("\nThe following are the available project name:\n");
		rt_kprintf("-----------------------------------------------------------------\n");		
//		rt_kprintf("NMEI-IN3COLA\n");
//		rt_kprintf("NMEI-IN3VEGE\n");
		rt_kprintf("COMMON-USE\n");
		rt_kprintf("ATLAS-QT1\n");
		rt_kprintf("B495-QT1\n");
		rt_kprintf("CLICK-RATIO\n");
		

		rt_kprintf("-----------------------------------------------------------------\n");	
	}
	else if (argc == 2 )	
	{
		if (!strcmp(argv[1], "get"))	
		{
			printProjectID();result=RT_EOK;
		}
	}
	else if (argc == 3 )	
	{
		
		if (!strcmp(argv[1], "select"))	
		{
			if (!strcmp(argv[2], "NMEI-IN3COLA"))				{		WhichProject=NMEI_IN3COLA;	result=RT_EOK;}
			else if (!strcmp(argv[2], "NMEI-IN3VEGE"))	{		WhichProject=NMEI_IN3VEGE;	result=RT_EOK;}
			else if (!strcmp(argv[2], "COMMON-USE"))		{		WhichProject=COMMON_USE;		result=RT_EOK;}
			
			else if (!strcmp(argv[2], "LID-OPEN"))			{		WhichProject=LID_OPEN;			result=RT_EOK;}
			else if (!strcmp(argv[2], "BUTTON-ONLINE"))	{		WhichProject=BUTTON_ONLINE;	result=RT_EOK;}
			else if (!strcmp(argv[2], "BUTTON-OFFLINE")){		WhichProject=BUTTON_OFFLINE;result=RT_EOK;}
			else if (!strcmp(argv[2], "ATLAS-QT1"))			{		WhichProject=ATLAS_QT1;result=RT_EOK;}
			
			else if (!strcmp(argv[2], "B495-QT1"))			{		WhichProject=B495_QT1;result=RT_EOK;}
			else if (!strcmp(argv[2], "CLICK-RATIO"))		{		WhichProject=CLICK_RATIO;result=RT_EOK;}
			
			if(result==RT_EOK)
			{
				at24cxx.write(at24c256 , EEPROM_APP_CS_ADDR, &WhichProject, 1);					
				rt_kprintf("control board is resetting to load and run project app......\n");
				cpu_reset();
			}
		}
	}
	return result;
}
MSH_CMD_EXPORT(project, Get or Select to run related project applications);
/* -----------------------------------------------------------------------
motion reset
 -----------------------------------------------------------------------*/

int motion(int argc, char **argv)
{
	
	
}



//不发项目编号的话 发送对应的命令去加载初始化配置也OK 关于电机配置直接把通用的写在默认配置里面
//后续可以使用板卡对应的特殊命令来配置 并把Dev所接设备保持入EEPROM




















