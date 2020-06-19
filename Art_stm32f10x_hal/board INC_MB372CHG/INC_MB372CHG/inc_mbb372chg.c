/*
	主要应用
		输入;输出;存储;光采集;电机运动
		
	主要完善: 

		速度档位的自动切换，pusle_div和ramp_div的自动跟随
		
*/
#include "inc_mbb372chg.h"	
#include "eeprom_24xx.h"	
#include "main.h"
#include "adc.h"
#include "leds.h"

#define SAMPLING_RES       0.51
#define GAIN_ADC           21

char version_string[]="ver1.0.0";

uint8_t LED_GREEN_PIN=PB_6;
uint8_t LED_BLUE_PIN=PB_7;

at24cxx_t at24c256=
{
	{0},
	{PB_8,PB_9},	
	0xA0,	
};

PARAM_T g_tParam;	

static void MX_GPIO_Init(void);

int main(void)
{
    /* set LED0 pin mode to output */
		rt_pin_mode(LED_GREEN_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_BLUE_PIN, PIN_MODE_OUTPUT);


    while (1)
    {
//				rt_pin_write(LED_BLUE_PIN, PIN_HIGH);	
//				rt_pin_write(LED_GREEN_PIN, PIN_HIGH);
			
//				rt_pin_write(LED_GREEN_PIN, PIN_LOW);
        rt_pin_write(LED_BLUE_PIN, PIN_HIGH);	
        rt_thread_mdelay(500);
//        rt_pin_write(LED_GREEN_PIN, PIN_HIGH);
				rt_pin_write(LED_BLUE_PIN, PIN_LOW);	
        rt_thread_mdelay(500);
    }
    return RT_EOK;
}

int mbb372chg_hw_init(void)
{		
	//board.c	rcc_set
	MX_GPIO_Init();
	
	at24cxx_hw_init();
	
	User_ADC_Init();
	
	MBI5024_Init();
	
	rt_kprintf("\nFW-MBB372CHG-RTT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
			
//	__HAL_AFIO_REMAP_SWJ_NOJTAG();
		
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("\n-------------------------------\n");
	rt_kprintf("project name: ");
	switch(g_tParam.Project_ID)
	{
		case B372_CHG: 
			rt_kprintf("B372 CHG\n");
			break;

		default:		
			break;
	}

	rt_kprintf("\nCommands mode--msh:command and parameters are separated by spaces\n");
	rt_kprintf("-------------------------------\n");
	rt_kprintf("\nyou can type help to list commands and all commands should end with \\r\\n\n");
	
  return 0;
}

/*#################################################################################################
*/

//
uint8_t CHG_EN_Pin[8]={PB_5,PB_4,PB_3,PA_15,PA_12,PA_11,PA_8,PB_14};
uint8_t CHG_CS_Pin[8]={PC_13,PC_14,PC_15,PD_0,PD_1,PB_2,PB_10,PB_11};


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SEL0_Pin|SEL1_Pin|SEL2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SEL3_Pin|SEL4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEL5_Pin|SEL6_Pin|SEL7_Pin|CHG_EN7_Pin 
                          |CHG_EN2_Pin|CHG_EN1_Pin|CHG_EN0_Pin|LED1_Pin 
                          |LED0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CHG_EN6_Pin|CHG_EN5_Pin|CHG_EN4_Pin|CHG_EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SEL0_Pin SEL1_Pin SEL2_Pin */
  GPIO_InitStruct.Pin = SEL0_Pin|SEL1_Pin|SEL2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SEL3_Pin SEL4_Pin */
  GPIO_InitStruct.Pin = SEL3_Pin|SEL4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : SEL5_Pin SEL6_Pin SEL7_Pin CS_Pin 
                           CHG_EN7_Pin CHG_EN2_Pin CHG_EN1_Pin CHG_EN0_Pin 
                           LED1_Pin LED0_Pin */
  GPIO_InitStruct.Pin = SEL5_Pin|SEL6_Pin|SEL7_Pin|CS_Pin 
                          |CHG_EN7_Pin|CHG_EN2_Pin|CHG_EN1_Pin|CHG_EN0_Pin 
                          |LED1_Pin|LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CHG_EN6_Pin CHG_EN5_Pin CHG_EN4_Pin CHG_EN3_Pin */
  GPIO_InitStruct.Pin = CHG_EN6_Pin|CHG_EN5_Pin|CHG_EN4_Pin|CHG_EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure peripheral I/O remapping */
  __HAL_AFIO_REMAP_PD01_ENABLE();

}
//
void usb_power_switch(uint8_t ch, uint8_t on_off)
{
	if(ch>0 && ch<9)
	{
		if(on_off)
		{
			pinSet(CHG_EN_Pin[ch-1]);	
		}
		else pinReset(CHG_EN_Pin[ch-1]);	
	}
	
}
//
void usb_device_select(uint8_t ch)
{
	if(ch>0 && ch<9)
	{
		for(uint8_t i=0;i<8;i++)
		{
			pinReset(CHG_CS_Pin[i]);			
		}
		pinSet(CHG_CS_Pin[ch-1]);	
	}	
}
//
int usb(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			
			  rt_kprintf("usb current <ch>\n");
			  rt_kprintf("    ch:1~8 or null\n");	
			
			  rt_kprintf("usb on|off <ch>\n");
			  rt_kprintf("    ch:1~8\n");	
			
			  rt_kprintf("usb select <ch>\n");
			  rt_kprintf("    ch:1~8\n");	
			
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "current")) 
			{	
				char StrAdc[8]="";	
				for(uint8_t i=0;i<8;i++)
				{
//					usb_power_switch(i+1,1);
					
//					float volatgeValue=GetVol_Average(i+1,1)*(1000/0.51/101)-Voltage_KB.B[i];
//					if(volatgeValue<0) volatgeValue=0;
//					sprintf(StrAdc,"%.0f",volatgeValue);
					
					sprintf(StrAdc,"%.0f",GetVol_Average(i+1,1)*(1000/GAIN_ADC/SAMPLING_RES));
					
//					if(i==7)
//					{
//						rt_kprintf("I[8]=%smA\n",StrAdc);
//					}
//					else rt_kprintf("I[%d]=%smA,",i+1,StrAdc);
					
					rt_kprintf("I[%d]=%smA\n",i+1,StrAdc);
				}				
        return RT_EOK;	
			}
    }
    if (argc == 3)
    {	
			if (!strcmp(argv[1], "current")) 
			{	
				uint8_t ch=atoi(argv[2]);
				if(ch>0 && ch<9)
				{
					char StrAdc[8]="";	
					usb_power_switch(ch,1);
//					float volatgeValue=GetVol_Average(ch,1)*(1000/0.51/101)-Voltage_KB.B[ch-1];
//					if(volatgeValue<0) volatgeValue=0;
//					sprintf(StrAdc,"%.0f",volatgeValue);
					
					sprintf(StrAdc,"%.0f",GetVol_Average(ch,1)*(1000/GAIN_ADC/SAMPLING_RES));
					rt_kprintf("I[%d]=%smA\n",ch,StrAdc);
				}
        return RT_EOK;	
			}
			if (!strcmp(argv[1], "on")) 
			{	
				uint8_t ch=atoi(argv[2]);
				if(ch>0 && ch<9)
				{
					usb_power_switch(ch,1);
					rt_kprintf("usb[%d] is power on\n",ch);
				}
        return RT_EOK;	
			}
			if (!strcmp(argv[1], "off")) 
			{	
				uint8_t ch=atoi(argv[2]);
				if(ch>0 && ch<9)
				{
					usb_power_switch(ch,0);
					rt_kprintf("usb[%d] is power off\n",ch);
				}
        return RT_EOK;	
			}
			if (!strcmp(argv[1], "select")) 
			{	
				uint8_t ch=atoi(argv[2]);
				if(ch>0 && ch<9)
				{
					usb_device_select(ch);
					rt_kprintf("usb[%d] is connect to pc\n",ch);
				}
        return RT_EOK;	
			}
    }
		return RT_ERROR;
}
//



INIT_APP_EXPORT(mbb372chg_hw_init);

MSH_CMD_EXPORT(savedata, Save data to page 1~200);
MSH_CMD_EXPORT(readdata, Read data from page 1~200);
//MSH_CMD_EXPORT(ParamSave, Save speical parameters);

MSH_CMD_EXPORT(usb,...);

/*	other cmd used
MSH_CMD_EXPORT(motor, control motor motion by commands);
MSH_CMD_EXPORT(rgb, set the rgb led);
MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(FunctionTest, Check whether the control is normal)
MSH_CMD_EXPORT(printdata, auto print position and press);;
*/

/*	MSH_CMD_EXPORT SYS CMD 									SYS CMD 只使用finsh方式
MSH_CMD_EXPORT(memcheck, check memory data);
MSH_CMD_EXPORT(memtrace, dump memory trace information);
MSH_CMD_EXPORT(version, show RT-Thread version information);
MSH_CMD_EXPORT(list_thread, list thread);
MSH_CMD_EXPORT(list_sem, list semaphore in system);
MSH_CMD_EXPORT(list_event, list event in system);
MSH_CMD_EXPORT(list_mutex, list mutex in system);
MSH_CMD_EXPORT(list_mailbox, list mail box in system);
MSH_CMD_EXPORT(list_msgqueue, list message queue in system);
MSH_CMD_EXPORT(list_memheap, list memory heap in system);
MSH_CMD_EXPORT(list_mempool, list memory pool in system);
MSH_CMD_EXPORT(list_timer, list timer in system);
MSH_CMD_EXPORT(list_device, list device in system);
MSH_CMD_EXPORT(reboot, cpu reset);
*/

