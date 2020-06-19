/*             �Ż����� ������Ҫ������Ŀ��Ϣ ��Ŀ֮���Ӧ�����ʼ��ʹ������������
	��ҪӦ��
		����;���;�洢;��ɼ�;����˶�
		
	��Ҫ����: 

		�ٶȵ�λ���Զ��л���pusle_div��ramp_div���Զ�����
		
	�����������������ȫ��Сд
*/


#include "inc_mb1616dev6.h"

#include "inc_dido.h"

#include "bsp_mcp3421.h"

#include "app_eeprom_24xx.h"

//#include "inc_fbtmc429.h"	

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DEG_TRACE		rt_kprintf
	#else
	#define DEG_TRACE(...)		
#endif

#endif

//

char version_string[]="ver3.0.0";

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
    int count = 1;

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_write(LED_PIN, PIN_HIGH);
    while (count++)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}


int mb1616dev6_hw_init(void)
{		
	//board.c	rcc_set
	
	rt_kprintf("\nFW-MB1616dDEV6-RTT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
	
	//cube_hal_inits();	
	
	delay_ms(100);
		
	at24cxx_hw_init();
	
	dido_hw_init();
		
//	tmc429_hw_init();
//	projectApp_init();
			
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("-------------------------------\n");
	rt_kprintf("you can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n");
  return 0;
}
//
INIT_APP_EXPORT(mb1616dev6_hw_init);	//���TMC429�����ڴ˽��г�ʼ������Ȼ�忨��λ�������ڼ�������,��������Ӱ�첻��

/*******************************************board debug*******************************
																						board debug
********************************************board debug******************************/
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
static void hw_eeprom_debug(void)
{
	char *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+EEPROM_INFO_PAGE_LEN-1)*EEPROM_PAGE_BYTES,(u8 *)save_info,len);
	at24cxx.read(at24c256 ,(EEPROM_INFO_PAGE_ADDR+EEPROM_INFO_PAGE_LEN-1)*EEPROM_PAGE_BYTES, read_info, len);
	
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
	if(pca9539a_is_inited>=3)
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







