/****************************************************************************************/
#include "app_button.h"
#include "app_gpio_dido.h"
#include "app_eeprom_24xx.h"

#ifdef 	BUTTON_DEBUG
#define BUTTON_TRACE         rt_kprintf
#else
#define BUTTON_TRACE(...)  				     
#endif
/****************************************************************************************/
uint8_t buttonSTARTpressed=FALSE;
uint8_t buttonRESETpressed=FALSE;

uint8_t buttonRESETDisnable=FALSE;

static uint8_t buttonSTARTCNT=0;
static uint8_t buttonRESETCNT=0;
/****************************************************************************************/
static uint8_t GetButtonStart(void);
static uint8_t GetButtonReset(void);

/****************************************************************************************/
static uint8_t GetButtonStart(void)
{
	uint8_t buttonValue=0;
	
	switch(g_tParam.Project_ID)
	{
		case EOK_POWE_2AXIS: if(inputGet(1)==IN_ON && inputGet(2)==IN_ON) buttonValue=1;
			break;
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 if(inputGet(1)==IN_ON ) buttonValue=1;
			break;				 
		default:
			break;	
	}
	return buttonValue;
}
static uint8_t GetButtonReset(void)
{
	uint8_t buttonValue=0;
	
	switch(g_tParam.Project_ID)
	{
		case EOK_POWE_2AXIS: if(inputGet(3)==IN_ON ) buttonValue=1;
			break;
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 if(inputGet(2)==IN_ON ) buttonValue=1;
			break;				 
		default:
			break;	
	}
	return buttonValue;
}
void SetButtonStartLed(uint8_t on_off)
{
	switch(g_tParam.Project_ID)
	{
		case EOK_POWE_2AXIS: outputSet(1,on_off);	outputSet(2,on_off);
			break;
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 outputSet(1,on_off);
			break;				 
		default:
			break;	
	}
}
void SetButtonResetLed(uint8_t on_off)
{
	switch(g_tParam.Project_ID)
	{
		case EOK_POWE_2AXIS: outputSet(3,on_off);	
			break;
		case OQC_FLEX: 			 
		case BUTTON_ROAD:		 outputSet(2,on_off);	
			break;				 
		default:
			break;	
	}
}
void ButtonProcess(void)
{
	if(buttonRESETpressed==FALSE)																									  
	{
		if( buttonSTARTpressed==FALSE && ( GetButtonStart()==TRUE ) )					
		{
			buttonSTARTCNT++;
			if( buttonSTARTCNT >= 2 )											 															
			{
				 SetButtonStartLed(1); SetButtonResetLed(0);				 			
				 buttonSTARTpressed=TRUE;  		
				 rt_kprintf("buttonSTART=1\n");
			}
		}
		else if( GetButtonStart()==FALSE)
		{
			buttonSTARTpressed=FALSE; //由命令下发解锁 改为自动解锁
			buttonSTARTCNT=0;	
		}
	}
	//
	if(buttonRESETDisnable==FALSE)
	{
		if( buttonRESETpressed==FALSE && GetButtonReset()==TRUE  )							
		{
			buttonRESETCNT++;
			if( buttonRESETCNT>=2)			
			{		
				ButtonReset_process();	
			}
		}
		else if( GetButtonReset()==FALSE)
		{
			buttonRESETCNT=0;	
		}	
	}
}
//
__weak void ButtonReset_process(void)
{
	
	
	
}

int buttonSTART(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))	rt_kprintf("buttonSTART=%d\n",buttonSTARTpressed);
		if (!strcmp(argv[1], "enable")) 
		{
			buttonSTARTpressed=FALSE;
			SetButtonStartLed(0);	SetButtonResetLed(0);
			rt_kprintf("enable button START for next test\n",buttonSTARTpressed);		
		}
	}
	else
	{
		rt_kprintf("Usage: \n");
		//用户接口
		rt_kprintf("buttonSTART status          -button START is pressed or not\n");
		rt_kprintf("buttonSTART enable          -enable button START for next test\n");
	}
	return 0;
}
int buttonRESET(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))						rt_kprintf("buttonRESET=%d\n",buttonRESETpressed);
		else if (!strcmp(argv[1], "enable")) 	{		buttonRESETDisnable=FALSE;	}
		else if (!strcmp(argv[1], "disable")) {		buttonRESETDisnable=TRUE;		}
	}
	else
	{
		rt_kprintf("Usage: \n");
		//用户接口
		rt_kprintf("buttonRESET status          -button RESET is pressed or not\n");
		rt_kprintf("buttonRESET enable          -enable button RESET\n");
		rt_kprintf("buttonRESET disable         -disable button RESET\n");
	}
	return 0;
}	
//MSH_CMD_EXPORT(buttonSTART, status and enable );
//MSH_CMD_EXPORT(buttonRESET, status and enable );
/****************************************************************************************/
static rt_uint8_t button_stack[ 256 ];
static struct rt_thread button_thread;

static void rt_hw_button_init(void)
{

}
static void button_thread_entry(void *parameter)
{
    rt_hw_button_init();

    while (1)
    {
        ButtonProcess();

        rt_thread_delay(10); /* sleep 0.5 second and switch to other thread */
    }
}

int button_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&button_thread,
                            "button",
                            button_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&button_stack[0],
                            sizeof(button_stack),
                            21,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&button_thread);
    }
    return 0;
}

static void printf_cmdList_button(void)
{
	printf("button Usage: \r\n");
	printf("buttonSTART status          -button START is pressed or not\r\n");
	printf("buttonSTART enable          -enable button START for next test\r\n");	
}

uint8_t Command_analysis_button(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"buttonSTART status") )		 		 printf("buttonSTART=%d\r\n",buttonSTARTpressed);
	else if( !strcmp(string,"buttonSTART enable") )
	{
			buttonSTARTpressed=FALSE;
			outputSet(1,0);	
			printf("enable button START for next test\r\n");	
	}
	
	else if( !strcmp(string,"button help") )			printf_cmdList_button();
	else result = REPLY_INVALID_CMD;
	return result;
}


