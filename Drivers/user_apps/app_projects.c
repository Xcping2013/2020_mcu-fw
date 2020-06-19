#include <rtthread.h>
#include <rtdef.h>
#include "inc_controller.h"	
#include "motion_control.h"
#include "app_gpio_dido.h"
#include "app_projects.h"

static void buttonPressOverhigh_thread_entry(void *parameter)
{
	static uint8_t getAlarmCNT=0;	
	
	static uint8_t alarmChannel=0;
	
	static uint8_t axisChannle=0;
	static uint8_t axisCh='z';
	
	switch(g_tParam.Project_ID)
	{
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 alarmChannel=8;	axisChannle=AXIS_Z;axisCh='y';
			break;
		case EOK_POWE_2AXIS: alarmChannel=8;	axisChannle=AXIS_Y;axisCh='z';
			break;		
		default:
			break;	
	}	
	
	while (1)
	{
		rt_enter_critical();
		if( inputGet(alarmChannel)==0 && Read429Short(SPI_DEV0_TMC429,IDX_VACTUAL|(axisChannle<<5)) > 0 )
		{
			getAlarmCNT++; 	
			if(getAlarmCNT>=2) 
			{
				HardStop(SPI_DEV0_TMC429,axisChannle);
				getAlarmCNT=0;
				rt_kprintf("stop motor %c due to pressure overhigh!!!\n",axisCh);	
			}
		}
//		if( inputGet(alarmChannel)==0 && Read429Short(SPI_DEV1_TMC429,IDX_VACTUAL|(axisChannle<<5)) > 0 )
//		{
//			getAlarmCNT++; 	
//			if(getAlarmCNT>=2) 
//			{
//				HardStop(SPI_DEV0_TMC429,axisChannle);
//				getAlarmCNT=0;
//				rt_kprintf("stop motor %c due to pressure overhigh!!!\n",axisCh);	
//			}
//		}
		else getAlarmCNT=0;
		rt_exit_critical();		
		rt_thread_mdelay(2); 
	}
}
int buttonPressOverhigh_thread_init(void)
{
	rt_err_t ret = RT_EOK;
	rt_thread_t thread;
	
	switch(g_tParam.Project_ID)
	{
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 
		case EOK_POWE_2AXIS: 
						
				thread = rt_thread_create("alarm_scan", buttonPressOverhigh_thread_entry, RT_NULL, 1024, 30, 20);
		
				if (thread != RT_NULL)	{rt_thread_startup(thread);}
				else										{	ret = RT_ERROR;	}	
			break;				 

		default:
			break;	
	}
	return ret;	
}
void projectApp_init(void)
{
	buttonPressOverhigh_thread_init();	
}

//设置目标速度。 单位： pulse/ms pulse/rev
void motorSetVel(uint8_t which_motor, double velocity)
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

//注意正负负号的处理
long ChangeVMaxToSpeed_ms(uint16_t VMaxReg)  
{
	long speedMS;
	switch(g_tParam.Project_ID)
	{
		case BUTTON_ONLINE:	
		case BUTTON_OFFLINE:	
		case BUTTON_ROAD:		 
		case COMMON:	
			
			speedMS=VMaxReg*(40054/21);
			break;

		default:
			break;
	}
	return speedMS;				//(VMaxReg) *(40054/21);

}
int16_t ChangeSpeed_msToVMax(long speed_ms)  
{
	int32_t VmaxREG;
	
	switch(g_tParam.Project_ID)
	{
		case BUTTON_ONLINE:	
		case BUTTON_OFFLINE:	
		case BUTTON_ROAD:		 
		case COMMON:	
			
			VmaxREG=speed_ms*21/40054;
			break;

		default:
			break;
	}
	//rt_kprintf("speed_ms=%ld,VmaxREG=%ld\n",speed_ms,VmaxREG);
	
	if(VmaxREG>2000)		VmaxREG=2000;
	if(VmaxREG<(-2000))	VmaxREG=(-2000);
	if(0<VmaxREG && VmaxREG<5)				VmaxREG=5;
	
	
	return VmaxREG;
	
}
void SelectAMaxByVMax(uint8_t which_motor, uint32_t speed_Vmax)  //TMC429 16MHZ
{
	uint32_t AMaxTemp;
	
//	speed=speed*210/400000;	//命令输入的是脉冲数 不再是TMC429内部寄存器值
	
	switch(g_tParam.Project_ID)
	{		 
		case BUTTON_ONLINE:	
		case BUTTON_OFFLINE:	
		case BUTTON_ROAD:		 
		case COMMON:				
			 AMaxTemp=(speed_Vmax*speed_Vmax*240)/(210*210);		 
			 if(AMaxTemp>2000) AMaxTemp=2000;
		   if(AMaxTemp<5) AMaxTemp=5;
//				 AMaxTemp=(MotorConfig[which_motor].VMax*MotorConfig[which_motor].VMax*240)/(210*210);		 
				 MotorConfig[which_motor].AMax=AMaxTemp;		
			break;				 

		default:
			break;	
	}
}























































