/******************************************Origin*********************************************************************

default OriginSensor --> IN GPIO

回原点动作中,需要及时采集原点信号,进行电机停止并设立当前位置为0,这个动作如果delay太久会影响其他轴的回0动作！！！

SO: 
-->设置单轴回原点-->设置原点传感器为外部中断（触发方式串口传感器型号)-->采集到信号，判断回零方向正确的话，设立当前位置为0，并关闭此外部中断。(加调试LOG)
-->回0动作中尽量不要使用串口采集信息，外部中断优先级大于串口，串口数据会出现异常

多轴自动回原点在单轴回原点基础上进行homeInfo.GoHome和homeInfo.Homed 进行管理

*********************************************************************************************************************/
/******************************************Limit*********************************************************************

回原点动作中会触发限位信号，对检测时间要求不高，可计数检测到50ms后进行反转再寻原点信号

*********************************************************************************************************************/


/***********************************************************************/
#include "bsp_include.h"	
#include "app_include.h"
#include "motor_sensor_process.h"
/***********************************************************************/
#define MOTOR_SENSOR_THREAD_PRIORITY	18

static uint8_t OriginSensorPin[3]={	PB_9,	PE_0,	PE_1};
static uint8_t OriginSensorON[3]= {	HIGH,	LOW,	LOW};

static rt_uint8_t MotorSensorStack[ 512 ];

static struct	rt_thread MotorSensorThread;
/***************************************************************************/
static void InitOriginSensorAsEXTI(void);
static void OriginSensorIRQEnable(uint8_t actualMotor);
static void OriginSensorIRQDisable(uint8_t actualMotor);
static void OriginSensorIRQCall(void *args);
static void setPositionAsOrigin(uint8_t axisNum);

static void MOTOR_ResetInSequence(uint8_t actualProID);
static void LimitSensorProcess(uint8_t actualMotor);
static void MotorSensor_Thread_entry(void *parameter);

/***************************************************************************/
//void MOTOR_ResetPreset( void )
//{	
//	outputSet(2,1);	 outputSet(1,0);	
//	
//	motorsResetInOrder=TRUE;
//	
//	for(uint8_t i=0;i<3;i++)
//	{
//		StopMotorByRamp(i);
//		homeInfo.Homed[i]=FALSE;
//		homeInfo.GoHome[i]=FALSE;
//		homeInfo.GoLimit[i]=FALSE;
//	}	
//	delay_ms(20);	
//	switch(g_tParam.Project_ID)
//	{
//		case BUTTON_OFFLINE:				
//		case BUTTON_ONLINE:
//				 homeInfo.GoHome[AXIS_Y]=TRUE;
//				 TMC429_MotorRotate(AXIS_Y,-homeInfo.HomeSpeed[AXIS_Y]);			//逆时针旋转		L		Y轴先复位		Y轴先后后前	
//  
//				break;
//				 
//		case OQC_FLEX:
//    case BUTTON_ROAD:
//			
//				 homeInfo.GoHome[AXIS_Z]=TRUE;
//				 homeInfo.GoLimit[AXIS_Z]=FALSE;
//				 homeInfo.Homed[AXIS_Z]=FALSE;
//				 homeInfo.HomeSpeed[AXIS_Z]=-g_tParam.speed_home[AXIS_Z];
//		
//		     TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);			//逆时针旋转		L		Z轴先复位		Z轴先上后下
//	
//				 rt_kprintf("motor[%d] is start go home by searching home sensor\n",AXIS_Z);
//		
//				 start_timer_5ms();
//		
//		break;	
//		
//		default:
//			break;
//	}
//	buttonRESETpressed=TRUE;	
//}
////


/***************************************************************************/

/***************************************************************************/
static void LimitSensorProcess(uint8_t actualMotor)
{
	static uint8_t motorlimitedCNT[3]={0,0,0};
	
	//电机回原点过程中停止了
	if(homeInfo.GoHome[actualMotor] && Read429Short(IDX_VACTUAL|(actualMotor<<5))==0	)
	{
		motorlimitedCNT[actualMotor]++;
		if(motorlimitedCNT[actualMotor]>=3)
		{
			//确定停止是由于限位触发的原因后，电机反转
			u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<actualMotor*2)) ? 1:0)										//触发左限位
			{		
				TMC429_MotorRotate(actualMotor,	abs((int32_t)homeInfo.HomeSpeed));					//向右转			
			}
			if((SwitchStatus& (0x01<<actualMotor*2)) ? 1:0)										//触发右限位
			{																													 		
				TMC429_MotorRotate(actualMotor,-(abs((int32_t)homeInfo.HomeSpeed)));				//左转
			}
			motorlimitedCNT[actualMotor]=0;
		}
	}
	//电机回限位过程中停止了
	if(homeInfo.GoLimit[actualMotor] && Read429Short(IDX_VACTUAL|(actualMotor<<5))==0	)
	{
		//确定停止是由于限位触发的原因后，电机复位OK
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);	
		
		if(((SwitchStatus& (0x02<<actualMotor*2)) ? 1:0) && (homeInfo.HomeSpeed<0)) 								
		{																													 
			setPositionAsOrigin(actualMotor);							
		}
		if(((SwitchStatus& (0x01<<actualMotor*2)) ? 1:0) && (homeInfo.HomeSpeed>0)) 
		{																													
			setPositionAsOrigin(actualMotor);				
		}
	}
}
//
static void MotorSensor_Thread_entry(void *parameter)
{
	InitOriginSensorAsEXTI();
	while (1)
	{  
		rt_enter_critical();		
		
		for(uint8_t i=0;i<N_O_MOTORS;i++)
		{			
			LimitSensorProcess(i);
		}	
		
		rt_exit_critical();
		
		rt_thread_mdelay(30);		
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

/*******************************************************************************************************************/
static void MOTOR_ResetInSequence(uint8_t actualProID)
{
	switch(actualProID)
	{
		//Z轴左转往上找负限位,再正转找原点
		case OQC_FLEX:
		case BUTTON_ROAD:
				 //旋转运动
				 TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);
				 //原点中断使能
				 OriginSensorIRQEnable(AXIS_Z);
			break;
		
		default:
			break;
	}
}

static void setPositionAsOrigin(uint8_t actualMotor)
{
	HardStop(actualMotor); 
	delay_ms(20);
	Set429RampMode(MOTOR_NUMBER(actualMotor), RM_VELOCITY);
	Write429Int(IDX_XTARGET|(actualMotor<<5), 0);
	Write429Int(IDX_XACTUAL|(actualMotor<<5), 0);
	
	homeInfo.Homed[actualMotor]=TRUE;
	homeInfo.GoHome[actualMotor]=FALSE;	
	homeInfo.GoLimit[actualMotor]=FALSE;	
	
	rt_kprintf("motor[%d] go home ok\n",actualMotor);	
}

static void InitOriginSensorAsEXTI(void)
{
	uint8_t i;
	for(i=0l;i<3;i++)
	{
		if(OriginSensorON[i]==HIGH)
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_RISING,  OriginSensorIRQCall, (void*)i);	
		else
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_FALLING, OriginSensorIRQCall, (void*)i);	
	}
	rt_kprintf("InitOriginSensorAsEXTI()...ok\n");
}





static void OriginSensorIRQEnable(uint8_t actualMotor)
{
	rt_pin_irq_enable(OriginSensorPin[actualMotor], PIN_IRQ_ENABLE);
}
static void OriginSensorIRQDisable(uint8_t actualMotor)
{
	rt_pin_irq_enable(OriginSensorPin[actualMotor], PIN_IRQ_DISABLE);
}
static void OriginSensorIRQCall(void *args)
{
	rt_uint32_t actualMotor;
	actualMotor = (rt_uint32_t)args;
	
	if(actualMotor<3)
	{
		//确定触发原点信号
		if(OriginSensorON[actualMotor]==HIGH && pinRead(OriginSensorPin[actualMotor])==HIGH ||
			 OriginSensorON[actualMotor]==LOW && pinRead(OriginSensorPin[actualMotor])==LOW			 )
		{
			
			if((homeInfo.HomeSpeed[actualMotor] >0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))<0 ) || 
				 (homeInfo.HomeSpeed[actualMotor] <0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))>0 )    )
			{	
				//触发原点的状态满足回原方式,停止电机把当前位置寄存器写0,并关闭原点中断功能			
				setPositionAsOrigin(actualMotor);
				OriginSensorIRQDisable(actualMotor);	
			}
		}	
	}
}
//
void OriginSensorProcess(void)
{
	

	
}



//
/***************************************************************************/
//														CodeTest Area START
/***************************************************************************/
void motor_gohome(uint8_t actualMotor, int home_speed)
{
	if(actualMotor<3)
	{
		homeInfo.GoHome[actualMotor] = TRUE;
		homeInfo.GoLimit[actualMotor]= FALSE;
		homeInfo.Homed[actualMotor]	= FALSE;
		homeInfo.HomeSpeed[actualMotor]=home_speed;
		SetAmaxAutoByspeed(actualMotor,abs(home_speed));
		
		OriginSensorIRQEnable(actualMotor);
		
		TMC429_MotorRotate(actualMotor,home_speed);
		
		rt_kprintf("motor[%d] is start go home by searching home sensor\n",actualMotor);				
	}
}

INIT_APP_EXPORT(MotorSensor_thread_init);

//#define FINSH_USING_MSH_ONLY
FINSH_FUNCTION_EXPORT(motor_gohome,...);
/***************************************************************************/
//														CodeTest Area END
/***************************************************************************/
//









