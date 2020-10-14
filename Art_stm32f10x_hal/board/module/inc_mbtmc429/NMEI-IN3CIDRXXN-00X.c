
#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"

#include "app_eeprom_24xx.h"

//				NMEI-IN3CIDR20N-001			M54 P1 COIL TEST	INC-HB54COILCN1-02		ID=COIL54
			
/*				项目相关功能
coil test 底层需求增加，配置需要保存：板卡上电真空输出默认控制控制；光栅触发后停止电机运行；reset按键进行电机复位动作（不相应命令）
*/

//1.板卡上的真空输出默认控制	Vacuum default control state

#define EE_RESET_ADDR 	(200+55)*64
#define EE_LIGHT_ADDR 	EE_RESET_ADDR+3
#define EE_VACUUM_ADDR 	EE_LIGHT_ADDR+2

static uint8_t COIL54_inited=0;

static uint8_t COIL54_RESET[3]={0};
static uint8_t COIL54_LIGHT[2]={0};
static uint8_t COIL54_VACUUM[2]={0};

static uint8_t COIL54_RESET_PRESS=0;
static uint8_t COIL54_RESET_CNT=0;

static uint8_t COIL54_LIGHT_PRESS=0;
static uint8_t COIL54_LIGHT_CNT=0;

int COIL54_thread_init(void);

void COIL54_RESET_exe(void)
{
	static uint8_t ResetOut_ON=0;
	if(COIL54_RESET[2]==8)
	{
		if(COIL54_RESET_PRESS==FALSE && buttonRESETpressed==FALSE && getChInput(COIL54_RESET[0])==IN_ON )							
		{
			COIL54_RESET_CNT++; 
			if( COIL54_RESET_CNT>=3 )			
			{		
				COIL54_RESET_PRESS=TRUE;
//			}
//			if(COIL54_RESET_PRESS==TRUE  && getChInput(COIL54_RESET[0])==IN_OFF)
//			{
				buttonRESETpressed=TRUE;	setChOutput(COIL54_RESET[1],1);
				
				setChOutput(5,1);
				
				ResetOut_ON=0;
				Stop_HardTimer();		
				homeInfo.GoHome[2]	=FALSE;
				homeInfo.GoLimit[2]=TRUE;
				
				homeInfo.Homed[2]	=FALSE;
				homeInfo.HomeSpeed[2]=-800;	
				SetAmaxAutoByspeed(2,800);
				TMC429_MotorRotate(2,-800);
				Start_HardTimer();		
			}

		}	

		else if( getChInput(COIL54_RESET[0])==IN_OFF)
		{
			COIL54_RESET_PRESS=FALSE;
			COIL54_RESET_CNT=0;	
			//COIL54_RESET_PRESS=0;
		}
				if (ResetOut_ON	==0 && homeInfo.Homed[2]	==TRUE)
		{
			ResetOut_ON=1;
			setChOutput(COIL54_RESET[1],0);
			
		}
	}
}
void COIL54_LIGHT_exe(void)
{
	if(COIL54_LIGHT[1]==8)
	{
		if( COIL54_LIGHT_PRESS==FALSE && getChInput(COIL54_LIGHT[0])==IN_OFF )							
		{
			COIL54_LIGHT_CNT++;
			if( COIL54_LIGHT_CNT>=3)			
			{		
				COIL54_LIGHT_PRESS=TRUE;
				buttonRESETpressed=FALSE;
				
				setChOutput(COIL54_RESET[1],0);
				
			  TMC429_MotorStop(2);
			}

		}	
				else if( getChInput(COIL54_LIGHT[0])==IN_ON)
			{
				COIL54_LIGHT_PRESS=FALSE;
				COIL54_LIGHT_CNT=0;	
			}
	}
	
	
}

void CoilTest_54_init(void)
{
	at24cxx.read(at24c256 , EE_RESET_ADDR, COIL54_RESET, 3);
	at24cxx.read(at24c256 , EE_LIGHT_ADDR, COIL54_LIGHT, 2);
	at24cxx.read(at24c256 , EE_VACUUM_ADDR,COIL54_VACUUM, 2);
	
	if(COIL54_VACUUM[1]==8)
	{
		setChOutput(COIL54_VACUUM[0],1);
	}
	
	if(COIL54_inited==0)
	{
		COIL54_inited=1;
		if(COIL54_RESET[2]==8 || COIL54_LIGHT[1]==8 || COIL54_VACUUM[1]==8)
		COIL54_thread_init();
	}
}
//IN_OUT_FUN(INPUTn,OUTPUTn,EN)
//COIL54 RESET  1 1 1   带灯的按键	COIL54 RESET  IN OUT ON
//COIL54 VACUUM 1 1									COIL54 VACUUM OUT ON					
//COIL54 LIGHT 1 1									COIL54 LIGHT IN ON
int COIL54(int argc, char **argv)
{
	uint8_t INn=0,OUTn=0,OnOff=0;
	
	int result = RT_ERROR;
	
//	if(COIL54_inited==0)
//	{
//		COIL54_inited=1;
//		
////		g_tParam.Project_ID  = ID_COIL54;
////		SaveParamToEeprom();	
//		COIL54_thread_init();
//	}
	
	if (argc == 5 )	
	{
		if (!strcmp(argv[1], "RESET"))
		{
			COIL54_RESET[0]=atoi(argv[2]);	COIL54_RESET[1]=atoi(argv[3]);
			COIL54_RESET[2]=atoi(argv[4])+7;
			at24cxx.write(at24c256,	EE_RESET_ADDR,COIL54_RESET,3);
			
			if(COIL54_RESET[2]==8)
			rt_kprintf("Enable RESET=IN%d,OUT%d\n",COIL54_RESET[0],COIL54_RESET[1]);
			else
			rt_kprintf("Disable RESET=IN%d,OUT%d\n",COIL54_RESET[0],COIL54_RESET[1]);


			result=RT_EOK;
		}	
	}
	else if (argc == 4 )	
	{
		if (!strcmp(argv[1], "VACUUM"))
		{
			COIL54_VACUUM[0]=atoi(argv[2]);	COIL54_VACUUM[1]=atoi(argv[3])+7;
			

			
			
			at24cxx.write(at24c256,	EE_VACUUM_ADDR,COIL54_VACUUM,2);

			if(COIL54_VACUUM[1]==8)
			{
				setChOutput(COIL54_VACUUM[0],1);
				rt_kprintf("Enable VACUUM=OUT%d\n",COIL54_VACUUM[0]);

			}
			else	
			{
				setChOutput(COIL54_VACUUM[0],0);
				rt_kprintf("Disable VACUUM=OUT%d\n",COIL54_VACUUM[0]);
			}

			result=RT_EOK;
		}	
		if (!strcmp(argv[1], "LIGHT"))
		{
			COIL54_LIGHT[0]=atoi(argv[2]);	COIL54_LIGHT[1]=atoi(argv[3])+7;
			at24cxx.write(at24c256,	EE_LIGHT_ADDR,COIL54_LIGHT,2);
			
			if(COIL54_LIGHT[1]==8)
			{
				rt_kprintf("Enable LIGHT=IN%d\n",COIL54_LIGHT[0]);

			}
			else	
			{
				rt_kprintf("Disable LIGHT=IN%d\n",COIL54_LIGHT[0]);
			}
			result=RT_EOK;
		}	
	}
	else
	{			
		rt_kprintf("Usage: \n");
		rt_kprintf("COIL54 RESET <var1> <var2> <var3>\n");
		rt_kprintf("   var1---input number:1~12\n");
		rt_kprintf("   var2---output number: 1~8\n");
		rt_kprintf("   var3---on|off: 0|1\n");

		rt_kprintf("COIL54 VACUUM <var1> <var2>\n");
		rt_kprintf("   var1---output number:1~8\n");
		rt_kprintf("   var2---on|off: 0|1\n");	

		rt_kprintf("COIL54 LIGHT <var1> <var2>\n");
		rt_kprintf("   var1---input number:1~12\n");
		rt_kprintf("   var2---on|off: 0|1\n");		
	}
	return result;
}
static rt_uint8_t COIL54_stack[ 1024 ];

static struct rt_thread COIL54_thread;

static void COIL54_thread_entry(void *parameter)
{
    //CoilTest_54_init();

    while (1)
    {
				COIL54_RESET_exe();
				COIL54_LIGHT_exe();
        rt_thread_delay(10);
    }
}

int COIL54_thread_init(void)
{
    rt_err_t result;

    /* init COIL54 thread */
    result = rt_thread_init(&COIL54_thread,
                            "COIL54",
                            COIL54_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&COIL54_stack[0],
                            sizeof(COIL54_stack),
                            21,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&COIL54_thread);
    }
    return 0;
}


MSH_CMD_EXPORT(COIL54, Commands for Coil 54 tester);


//
//







