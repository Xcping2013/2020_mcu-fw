/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2019-10-19     xuzhuoyi     add stm32f429-st-disco bsp
 */

#include <mpm.h>
#include <main.h>

#include <finsh.h>
#include "msh.h"
#include <dfs_posix.h>

#include "mpm_display.h"

static uint16_t pre_dll_selftime=0;
static uint16_t temPress_selftime=0;
static uint16_t dis_gfx_selftime=0;

static uint8_t output_times_delay=0;

void output_times(void)
{
	if(output_times_delay++>=10)
	{
		pinToggle(GET_PIN(F, 6));	
		output_times_delay=0;
	}
	
	
}
void pre_dll_times(void)
{
	if(pre_dll_selftime<600)
	{
		//DLLR_readdata();
		pre_dll_selftime=pre_dll_selftime+100;
	}		
	
}
void dis_BGcolour_times(void)
{
	if(dis_gfx_selftime<2000)
	{
		gfx_BGcolour(dis_gfx_selftime);
		dis_gfx_selftime=dis_gfx_selftime+200;
	}	
	else dis_gfx_selftime=0;
}

void dis_tmPre_times(void)
{
	if(temPress_selftime<600)
	{
		//dis_bme280_times();
		temPress_selftime=temPress_selftime+100;
	}	
}

/**********************************************
reboot           - Reboot System
list_fd          - list file descriptor											MSH_CMD_EXPORT(list_fd, list file descriptor);
version          - show RT-Thread version information				rt_show_version	
list_thread      - list thread
list_sem         - list semaphore in system
list_event       - list event in system
list_mutex       - list mutex in system
list_mailbox     - list mail box in system
list_msgqueue    - list message queue in system
list_memheap     - list memory heap in system
list_mempool     - list memory pool in system
list_timer       - list timer in system
list_device      - list device in system
exit             - return to RT-Thread shell mode.
help             - RT-Thread shell help.
ps               - List threads in the system.
free             - Show the memory usage in the system.
ls               - List information about the FILEs.
cp               - Copy SOURCE to DEST.
mv               - Rename SOURCE to DEST.
cat              - Concatenate FILE(s)
rm               - Remove(unlink) the FILE(s).
cd               - Change the shell working directory.
pwd              - Print the name of the current working directory.	//FINSH_FUNCTION_EXPORT_ALIAS(cmd_mkdir, __cmd_mkdir, Create the DIRECTORY.);
mkdir            - Create the DIRECTORY.
mkfs             - format disk with file system
df               - disk free
echo             - echo string to file
dut              - ...
tsi              - ...
mfc              - ...
scale            - ...
display          - ...
mpm_test         - Board function test
led_rgb          - led test
output           - output set
readinput        - read input
savedata         - save data to page 1~200
readdata         - read data from page 1~200
eeprom           - ...
DLLR             - ...
bme280           - ...
sdcard_init      - Board function test
sdin 
************************************************/
void mpm_outputs_test(void)
{
	static uint8_t i=0;
	static uint8_t j=0;	
	static uint8_t z=0;	
	
	for(z=0;z<120;z++)
	{
		if(++i<=6)	
		{
			rt_pin_write(valve_pin[i], PIN_HIGH);
			
//			rt_pin_write(DUT_5V_EN_PIN, PIN_HIGH);
			
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
			
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			
			rt_thread_mdelay(100);
		}
		else if(++j<=6)	
		{
			rt_pin_write(valve_pin[j], PIN_LOW);
			
//			rt_pin_write(DUT_5V_EN_PIN, PIN_LOW);
			
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
			
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			
			rt_thread_mdelay(100);
		}	
		if(i>=6 && j>=6) {i=0;j=0;}
	}
}

/*		board seft_debug
*/
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
	uint8_t read_info[64];
	uint8_t len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,100,(uint8_t *)save_info,len);
	at24cxx.read(at24c256 , 100, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\n>>");
  }	
	else rt_kprintf("ok\n>>");
}




static void board_debug_thread_entry(void *parameter)
{
//	hw_eeprom_debug();
//	mpm_outputs_test();
	
//	sd_card_test();
	//app_bme280_active();
	
	while (1)
	{	
		
		//dis_BGcolour_times();	rt_thread_mdelay(20);
		dis_tmPre_times();		rt_thread_mdelay(20);
		output_times();				rt_thread_mdelay(20);
		//msTaskNoneRTOS_bme280(0);
		rt_thread_mdelay(300);
	}
}

static rt_thread_t tid1 = RT_NULL;

int board_debug_thread_init(void)
{
	tid1 = rt_thread_create("board_debug",
													board_debug_thread_entry,
													RT_NULL,
													1024,
													20,
													20);

	if (tid1 != RT_NULL)
	{
			rt_thread_startup(tid1);
	}
  return 0;
}



void mpm_test(void)
{
	static uint8_t debug_enabled=0;
	
	pre_dll_selftime=0;
	temPress_selftime=0;
	dis_gfx_selftime=0;
	
	if(debug_enabled==0)
	{
		debug_enabled=1;	
		
//		rt_pin_mode(DUT_5V_EN_PIN,PIN_MODE_OUTPUT);
//		rt_pin_write(DUT_5V_EN_PIN, PIN_LOW);
//		rt_pin_mode(DUT_UART_SEL_PIN,PIN_MODE_OUTPUT);
//		rt_pin_write(DUT_UART_SEL_PIN, PIN_LOW);
		led_rgb_hw_init();
		valve_hw_init();
		
		rt_pin_write(GET_PIN(C, 2), PIN_HIGH);	
		
		board_debug_thread_init();
	}
}

void mpm_init(void)
{
	pinMode(PB_6,PIN_MODE_INPUT_PULLUP);
	rt_kprintf("\nINC-FBNUCLEOCNT-2.7 init ok, you can type help to get command list\n");	
}

int UserMsh_Commands(char *cmd, rt_size_t length)
{	
	
//	if(!strncmp("read doorsensor",cmd,4))
//	{
//		rt_kprintf("string=%s\n",cmd);¡¤
//		return 0;
//	}	
	//if(MB1616_DIDO_msh(cmd)==REPLY_OK) return 0;
	
	msh_exec(cmd,length);
	
	return 0;
}	

INIT_APP_EXPORT(mpm_init);

//mpm eeprom test
//mpm test all
MSH_CMD_EXPORT(mpm_test, Board function test);

//MSH_CMD_EXPORT(mpm_loop, ...);


