/*             优化代码 不在需要保存项目信息 项目之间的应用与初始化使用命令来区别
	主要应用
		输入;输出;存储;光采集;电机运动
		
	主要完善: 

		速度档位的自动切换，pusle_div和ramp_div的自动跟随
		
	建议命令输入与输出全用小写
*/


#include "inc_mb1616dev6.h"

#include "inc_dido.h"

#include "bsp_mcp3421.h"

#include "app_eeprom_24xx.h"

#include "app_tmc429.h"		
//#include "inc_fbtmc429.h"	
#include "inc_projects.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

#define EEPROM_TEST_PAGE_ADDR			 254

uint8_t LED_PIN=PB_2;

at24cxx_t at24c256=
{
	{0},
	{PC_14,PC_15},	
	0xA2,	
};
/****************************************MAIN---LED Blink*******************************************************/
int main(void)
{

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_write(LED_PIN, PIN_HIGH);
    while (1)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}

char version_string[]="Ver01";

int mb1616dev6_hw_init(void)
{		
	//board.c	rcc_set
	
	rt_kprintf("\nFW-MB1616dDEV6 %s ROM[%dK] RAM[%dK] build at %s %s\n\n", version_string,STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE,  __TIME__, __DATE__);
	
	//cube_hal_inits();	
	
	delay_ms(10);
		
	at24cxx_hw_init();
	
	dido_hw_init();
		
	tmc429_hw_init();

	ProjectAppInit();
	
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("-------------------------------\n");
	rt_kprintf("you can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n>>");
  return 0;
}
//
INIT_APP_EXPORT(mb1616dev6_hw_init);	//电机TMC429必须在此进行初始化，不然板卡复位后电机还在继续运行,正常运行影响不大

/*******************************************board debug*******************************
																						board debug
********************************************board debug******************************/
#if 	( 1 ) 
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}
static void hw_eeprom_debug(void)
{
	char *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,EEPROM_TEST_PAGE_ADDR*EEPROM_PAGE_BYTES,(u8 *)save_info,len);
	at24cxx.read(at24c256 ,EEPROM_TEST_PAGE_ADDR*EEPROM_PAGE_BYTES, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\n>>");
  }	
	else rt_kprintf("ok\n>>");
}
static void input_trigger_output(void)
{
	for(u8 channel=1;channel<17;channel++)
	{
		outputSet(channel,inputGet(channel));
	}
	//if(pca9539a_is_inited>=3)
	{
		for(u8 channel=1;channel<17;channel++)
		{
			outputSet(channel,inputGet(channel+16));
		}		
	}
}
static void board_debug_thread_entry(void *parameter)
{
		hw_eeprom_debug();
    while (1)
    {
			input_trigger_output();	    
      rt_thread_delay(50);
    }
}
static rt_thread_t tid1 = RT_NULL;

static u8 debug_enabled=0;

int board_debug_thread_init(void)
{
	tid1 = rt_thread_create("board_debug",
													board_debug_thread_entry,
													RT_NULL,
													512,
													20,
													20);

	if (tid1 != RT_NULL)
	{
			rt_thread_startup(tid1);
	}
  return 0;
}

void board_debug(void)
{
	if(debug_enabled==0)
	{
		debug_enabled=1;	
		board_debug_thread_init();
	}
}
/****************************MSH_CMD_EXPORT*****************FINSH_FUNCTION_EXPORT*******************************************/
MSH_CMD_EXPORT(board_debug, Board function test);
FINSH_FUNCTION_EXPORT(board_debug,...);

#endif


//
#if 0
void MB1616_CMD_help(void)
{
	printf_cmdList_gpio();
	printf_cmdList_eeprom();
}
//
#endif


//








