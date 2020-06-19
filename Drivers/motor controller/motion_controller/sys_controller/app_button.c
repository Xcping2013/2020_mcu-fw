/*********************************************************************************************************************/
#include "app_gpio_dido.h"	
#include "app_motor_home.h"
/*********************************************************************************************************************/
uint8_t buttonSTARTpressed=FALSE;
uint8_t buttonRESETpressed=FALSE;

static uint8_t buttonSTARTCNT=0;
static uint8_t buttonRESETCNT=0;

/*********************************************************************************************************************/
void buttonSTART_process(uint8_t inCh, uint8_t outCh)
{
	if(buttonRESETpressed==FALSE)																									  //电机在复位中，START无效
	{
		if( inputGet(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			buttonSTARTCNT++;	
			if( buttonSTARTCNT>=2)											 																//开始按键
			{
				 outputSet(1,1);	outputSet(2,0);							 									 			
				 buttonSTARTpressed=TRUE;  		
				 rt_kprintf("buttonSTART=1\n");
			}
		}
		else if( inputGet(inCh)==IN_OFF)
		{
			//buttonSTARTpressed=FALSE; //由命令下发解锁 改为自动解锁
			buttonSTARTCNT=0;	
		}
	}
}
/*
回原点速度从EEPROM读出
Bx35_button: Y轴先里后出 ---> Z轴先上后下 ---> X轴先左后右

B445_button: Z轴先上后下回原点后再往上运行一段位置放置传感器撞到Holder---> Y轴先里后出---> X轴先左后右

ROAD			 ：Z轴先上后下				
*/
void buttonRESET_process(uint8_t inCh, uint8_t outCh)
{
	if( inputGet(inCh)==IN_ON && buttonRESETpressed==FALSE )							
	{
		buttonRESETCNT++;		
		if( buttonRESETCNT>=2)			
		{		
			MotorAutoReset_preset();	
		}
	}
	else if( inputGet(inCh)==IN_OFF)
	{
		//buttonRESETpressed=FALSE; 复位完成自动解锁
		buttonRESETCNT=0;	
	}
}

int buttonSTART(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))	rt_kprintf("buttonSTART=%d\n",buttonSTARTpressed);
		if (!strcmp(argv[1], "enable")) 
		{
			buttonSTARTpressed=FALSE;
			outputSet(1,0);	
			rt_kprintf("enable button START for next test\n",buttonSTARTpressed);		
		}
	}
	else
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("buttonSTART status          -button START is pressed or not\n");
		rt_kprintf("buttonSTART enable          -enable button START for next test\n");
	}
	return 0;
}
	

/******************************************finsh***********************************************************************/
MSH_CMD_EXPORT(buttonSTART, status and enable );
/*********************************************************************************************************************/


