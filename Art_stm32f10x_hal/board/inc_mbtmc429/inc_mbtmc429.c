/*			控制卡基本功能

HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);

1. DIDO
2. EEPROM
3. Motor control,需要接驱动器

main_thread_entry						系统运行指示灯
finsh_thread_entry					串口字符串交互
motion_thread_entry					电机回原点
board_debug_thread_entry		控制板硬件自检


change log:

20200630_ver1.0.0:	基本功能的整理完善



*/
/*			road:

HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);

1.	IN8接压力里模块的报警输出： 

		采集到报警信号时,电机停止且屏蔽向下运动或者不响应运动命令，需要复位动作去清除
		
2.	串口3打印压力值和电机位置 

3.	优先级别	Z轴电机回原 > 电机停止 > 报警信号	> 运动命令

#define RT_USING_UART3

serial_thread_entry					串口打印压力值和电机位置

ver1.04---中断采集过压报警信号;立即停止电机;循环发送报警字符串

*/
#include "bsp_mcu_delay.h"	
#include "inc_dido.h"	
#include "app_tmc429.h"	
#include "app_eeprom_24xx.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

#define EEPROM_TEST_PAGE_ADDR			 255

char version_string[]="ver1.0.0";

uint8_t LED_PIN=PD_7;

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SDA SCL
	0xA0,
};

int main(void)
{
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_write(LED_PIN, PIN_HIGH);
    while (1)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
    return RT_EOK;
}

int mbtmc429_hw_init(void)
{			
	//board.c	rcc_set
	
	rt_kprintf("\nFW-MBTMC429-RTT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
	
	delay_ms(10);
	
	at24cxx_hw_init();
	
	dido_hw_init();

	tmc429_hw_init();
	
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("-------------------------------\n");
	rt_kprintf("you can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n");
	
  return 0;
}

INIT_APP_EXPORT(mbtmc429_hw_init);

#if 	( DBG_ENABLE ) 

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
static void input_trigger_output(void)
{
	for(u8 channel=1;channel<5;channel++)
	{
		outputSet(channel,(inputGet(channel)? 0:1) | (inputGet(channel+8)? 0:1) );
	}
	for(u8 channel=5;channel<9;channel++)
	{
		outputSet(channel,inputGet(channel)? 0:1);
	}

}
static void hw_eeprom_debug(void)
{
	char *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,EEPROM_TEST_PAGE_ADDR*EEPROM_PAGE_BYTES, save_info,len);
	delay_ms(10);
	at24cxx.read(at24c256 , EEPROM_TEST_PAGE_ADDR*EEPROM_PAGE_BYTES, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\n");
  }	
	else rt_kprintf("ok\n");
	
}

static void hw_motor_debug(void)
{
//	//读取某一个电机限位状态，再设置取反后读取判断
//	rt_kprintf("tmc429 test..................");
//	if(Read429SingleByte(IDX_REF_SWITCHES, 3)==0) 
//		rt_kprintf("err");
//	else rt_kprintf("ok");

}
static void board_debug_thread_entry(void *parameter)
{
		hw_eeprom_debug();
		//hw_motor_debug();
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

MSH_CMD_EXPORT(board_debug, list device in system);

#endif








