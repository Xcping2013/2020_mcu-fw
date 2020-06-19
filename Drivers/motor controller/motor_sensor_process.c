/******************************************Origin*********************************************************************

default OriginSensor --> IN GPIO

��ԭ�㶯����,��Ҫ��ʱ�ɼ�ԭ���ź�,���е��ֹͣ��������ǰλ��Ϊ0,����������delay̫�û�Ӱ��������Ļ�0����������

SO: 
-->���õ����ԭ��-->����ԭ�㴫����Ϊ�ⲿ�жϣ�������ʽ���ڴ������ͺ�)-->�ɼ����źţ��жϻ��㷽����ȷ�Ļ���������ǰλ��Ϊ0�����رմ��ⲿ�жϡ�(�ӵ���LOG)
-->��0�����о�����Ҫʹ�ô��ڲɼ���Ϣ���ⲿ�ж����ȼ����ڴ��ڣ��������ݻ�����쳣

�����Զ���ԭ���ڵ����ԭ������Ͻ���homeInfo.GoHome��homeInfo.Homed ���й���

*********************************************************************************************************************/
/******************************************Limit*********************************************************************

��ԭ�㶯���лᴥ����λ�źţ��Լ��ʱ��Ҫ�󲻸ߣ��ɼ�����⵽50ms����з�ת��Ѱԭ���ź�

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
//				 TMC429_MotorRotate(AXIS_Y,-homeInfo.HomeSpeed[AXIS_Y]);			//��ʱ����ת		L		Y���ȸ�λ		Y���Ⱥ��ǰ	
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
//		     TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);			//��ʱ����ת		L		Z���ȸ�λ		Z�����Ϻ���
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
	
	//�����ԭ�������ֹͣ��
	if(homeInfo.GoHome[actualMotor] && Read429Short(IDX_VACTUAL|(actualMotor<<5))==0	)
	{
		motorlimitedCNT[actualMotor]++;
		if(motorlimitedCNT[actualMotor]>=3)
		{
			//ȷ��ֹͣ��������λ������ԭ��󣬵����ת
			u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<actualMotor*2)) ? 1:0)										//��������λ
			{		
				TMC429_MotorRotate(actualMotor,	abs((int32_t)homeInfo.HomeSpeed));					//����ת			
			}
			if((SwitchStatus& (0x01<<actualMotor*2)) ? 1:0)										//��������λ
			{																													 		
				TMC429_MotorRotate(actualMotor,-(abs((int32_t)homeInfo.HomeSpeed)));				//��ת
			}
			motorlimitedCNT[actualMotor]=0;
		}
	}
	//�������λ������ֹͣ��
	if(homeInfo.GoLimit[actualMotor] && Read429Short(IDX_VACTUAL|(actualMotor<<5))==0	)
	{
		//ȷ��ֹͣ��������λ������ԭ��󣬵����λOK
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
		//Z����ת�����Ҹ���λ,����ת��ԭ��
		case OQC_FLEX:
		case BUTTON_ROAD:
				 //��ת�˶�
				 TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);
				 //ԭ���ж�ʹ��
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
		//ȷ������ԭ���ź�
		if(OriginSensorON[actualMotor]==HIGH && pinRead(OriginSensorPin[actualMotor])==HIGH ||
			 OriginSensorON[actualMotor]==LOW && pinRead(OriginSensorPin[actualMotor])==LOW			 )
		{
			
			if((homeInfo.HomeSpeed[actualMotor] >0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))<0 ) || 
				 (homeInfo.HomeSpeed[actualMotor] <0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))>0 )    )
			{	
				//����ԭ���״̬�����ԭ��ʽ,ֹͣ����ѵ�ǰλ�üĴ���д0,���ر�ԭ���жϹ���			
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









