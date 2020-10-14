
/*			module Description

Benefits：每个模块自带MCU和存储IC,自带独立的程序,不同的模块进行分割设计，方便独立的软硬件开发

模块功能说明：

1.MCU：STM32F103C8 ROM=64KB RAM=20KB | STM32F103CB ROM=128KB RAM=20KB

	串口1接收PC命令，地址符合本机则直接执行命令，并返回数据
	串口1接收PC命令，地址不符合则从串口2转发485数据出去
	
	串口2接收到485数据，地址符合回复串口2的485数据并且从串口1也发送数据

2.SPI-TMC429 只进行单芯片3轴操作，可以调用旧项目的相关文件

  3GPIO=原点
	3GPIO=默认为对应电机停止运动信号，使能信号，具体项目可以更改为单方向软限位来使用;
				回原点命令按需可不受此信号限制
	3IN=HARD LIMIT
	
3. EEPROM 只存储电机相关配置参数，可以参考stepRocker

*/
/*			Ver1.0 20200917~	

*/
#include "tmc429.h"	
#include "bsp_eeprom_24xx.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

char version_string[]="module:inc-mbtmc429 FW-ver1.0.0";

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

void tmc429_spi_init(void)
{
	
	
}
void tmc429_clk_init(void)
{
	
	
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








