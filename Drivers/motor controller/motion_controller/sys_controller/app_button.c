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
	if(buttonRESETpressed==FALSE)																									  //����ڸ�λ�У�START��Ч
	{
		if( inputGet(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			buttonSTARTCNT++;	
			if( buttonSTARTCNT>=2)											 																//��ʼ����
			{
				 outputSet(1,1);	outputSet(2,0);							 									 			
				 buttonSTARTpressed=TRUE;  		
				 rt_kprintf("buttonSTART=1\n");
			}
		}
		else if( inputGet(inCh)==IN_OFF)
		{
			//buttonSTARTpressed=FALSE; //�������·����� ��Ϊ�Զ�����
			buttonSTARTCNT=0;	
		}
	}
}
/*
��ԭ���ٶȴ�EEPROM����
Bx35_button: Y�������� ---> Z�����Ϻ��� ---> X���������

B445_button: Z�����Ϻ��»�ԭ�������������һ��λ�÷��ô�����ײ��Holder---> Y��������---> X���������

ROAD			 ��Z�����Ϻ���				
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
		//buttonRESETpressed=FALSE; ��λ����Զ�����
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


