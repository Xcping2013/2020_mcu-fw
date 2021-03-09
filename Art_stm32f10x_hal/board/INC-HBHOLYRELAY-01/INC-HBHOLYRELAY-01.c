
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

char version_string[]="ver01";
/*

*/
at24cxx_t at24c256=
{
	{0},
	{PB_1,PB_0},	
	0xA0,	
};

uint8_t LED_PIN=PB_14;

uint8_t RelayCon_Vin2BatPin[6]={PA_0, PA_1,PA_2, PA_3, PA_4, PA_5 };				//(1~6)VBATn+ -> DC3V3 ON|OFF
uint8_t RelayCon_Hi2VddPin[6]={PB_3, PA_15,PA_14, PA_13, PA_12, PA_11 };		//HI	---> (1~6)VDDn  ON|OFF

uint8_t RelayCon_Ai2DC3V3Pin=PA_8;																					//AI	---> DC3V3
uint8_t RelayCon_Lo2BatPin[6]={PB_4, PB_5,PB_6, PB_7, PB_8, PB_9 };					//LO	---> VBAT(1~6)
uint8_t RelayCon_Lo2GndPin=PB_15;																						//LO	---> GND

uint8_t Dut_isPowerON[6]={0,0,0,0,0,0,};

uint8_t DutSelectedPre=0;
uint8_t DutSelectedCur=0;
uint8_t RelayConPins[20]={
	PA_0, PA_1,	PA_2, PA_3, PA_4, PA_5,
	PB_4, PB_5,	PB_6, PB_7, PB_8, PB_9,
	PB_3, PA_15,PA_14, PA_13, PA_12, PA_11,
	PA_8,
	PB_15};
/*************************************************************************************************************/
uint8_t at24cxx_msh(char *string)
{
	
	
}
	
void relayGPIOinit(void)
{
	for(uint8_t i=0;i<20;i++)
	{
		rt_pin_mode(RelayConPins[i], PIN_MODE_OUTPUT);
		rt_pin_write(RelayConPins[i], PIN_LOW);
	}
}
/****************************************MAIN---LED Blink*******************************************************/
int main(void)
{
	rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED_PIN, PIN_HIGH);
	while (1)
	{
			rt_pin_write(LED_PIN, PIN_HIGH);
			rt_thread_mdelay(800);
			rt_pin_write(LED_PIN, PIN_LOW);
			rt_thread_mdelay(800);
	}
	return RT_EOK;
}

/****************************************mbrelayadp_hw_init**********************************************************/
int mbrelayadp_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\nFW:PCBA-NFC-BFT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
			
	__HAL_AFIO_REMAP_SWJ_DISABLE();
	
	relayGPIOinit();
	
	at24cxx_hw_init();
		
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("\n-------------------------------\n");
	rt_kprintf("\nyou can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n");
	
  return 0;
}
void RelayCon_Hi2Vdd_CS(uint8_t dutChannel)
{
	if(0<dutChannel && dutChannel<7)
	{
		for(uint8_t i=0;i<6;i++)
		{
			rt_pin_write(RelayCon_Hi2VddPin[i], PIN_LOW);
		}			
		rt_pin_write(RelayCon_Hi2VddPin[dutChannel-1], PIN_HIGH);

	}
}
void RelayCon_Lo2Bat_CS(uint8_t dutChannel)
{
	if(0<dutChannel && dutChannel<7)
	{
		for(uint8_t i=0;i<6;i++)
		{
			rt_pin_write(RelayCon_Lo2BatPin[i], PIN_LOW);
		}				
		rt_pin_write(RelayCon_Lo2BatPin[dutChannel-1], PIN_HIGH);
	}
}
/****************************************power
上电的时候需要把AI或者LO断开;或者LO-BAT+
**********************************************************/

void DUT_PowerON(uint8_t dutChannel)
{
	if(0<dutChannel && dutChannel<7)
	{
		//LO GND OFF
		//AI 3V	 OFF 	防止VCC-AI-LO-GND 烧毁仪表保险丝
		rt_pin_write(RelayCon_Ai2DC3V3Pin, PIN_LOW);
		//VBAT+ -> 3V
		rt_pin_write(RelayCon_Vin2BatPin[dutChannel-1], PIN_HIGH);
		
		Dut_isPowerON[dutChannel-1]=1;
	}
}
void DUT_PowerOFF(uint8_t dutChannel)
{
	if(0<dutChannel && dutChannel<7)
	{
		//LO GND OFF
		//AI 3V	 OFF 	防止VCC-AI-LO-GND 烧毁仪表保险丝
		rt_pin_write(RelayCon_Ai2DC3V3Pin, PIN_LOW);
		//VBAT+ -> 3V
		rt_pin_write(RelayCon_Vin2BatPin[dutChannel-1], PIN_LOW);
		
		Dut_isPowerON[dutChannel-1]=0;
	}
}
void SwitchToMeasureVoltage(uint8_t dutChannel)
{
	if(0<dutChannel && dutChannel<7)
	{
		rt_pin_write(RelayCon_Ai2DC3V3Pin, PIN_LOW);
		//LO-BAT OFF
		for(uint8_t i=0;i<6;i++)
		{
			rt_pin_write(RelayCon_Lo2BatPin[i], PIN_LOW);
			
			rt_pin_write(RelayCon_Hi2VddPin[i], PIN_LOW);
		}
		//LO-GND ON
		rt_pin_write(RelayCon_Lo2GndPin, PIN_HIGH);
		//HI-VDD CS ON
		rt_pin_write(RelayCon_Hi2VddPin[dutChannel-1], PIN_HIGH);
	}
}
//需要注意切换采集电流的时候，DUT有可能先断电，如果对测试有影响，需要进行优化
void SwitchToMeasureCurrent(uint8_t dutChannel)
{
	//LO-GND OFF
	rt_pin_write(RelayCon_Lo2GndPin, PIN_LOW);
	
	//DUT_PowerON(DutSelectedPre);
	
	if(0<dutChannel && dutChannel<7)
	{
		//LO-BAT OFF
		for(uint8_t i=0;i<6;i++)
		{
			rt_pin_write(RelayCon_Lo2BatPin[i], PIN_LOW);
		}
		//AI-3V CS ON
		rt_pin_write(RelayCon_Ai2DC3V3Pin, PIN_HIGH);
		delay_ms(5);
		rt_pin_write(RelayCon_Lo2BatPin[dutChannel-1], PIN_HIGH);
		
		rt_pin_write(RelayCon_Vin2BatPin[dutChannel-1], PIN_LOW);
		//DUT_PowerOFF(dutChannel);
	}
}
int relay(int argc, char **argv)
{
	uint8_t result = REPLY_OK;
	
	uint8_t relayCh=0;
	uint8_t relayStatus=0;
	
	if (argc ==3 )
	{
		relayCh=atoi(argv[1]);
		relayStatus=atoi(argv[2]);
	
		if(0<relayCh && relayCh<21)
		{
			if(relayStatus==0)
			{
				rt_pin_write(RelayConPins[relayCh-1],PIN_LOW);
			}
			else rt_pin_write(RelayConPins[relayCh-1],PIN_HIGH);
						
			result = REPLY_OK;
		}
		else result = REPLY_INVALID_CMD;
	}
	if (argc ==4 )
	{
		if (!strcmp(argv[1], "switch") ) 
		{
			if (!strcmp(argv[2], "voltage") ) 
			{
				SwitchToMeasureVoltage(atoi(argv[3]));
				
				result = REPLY_OK;
				rt_kprintf("ready to measure voltage of dut%d\n",atoi(argv[3]));
				
			}			
			else if(!strcmp(argv[2], "current") ) 
			{		
				DutSelectedCur=atoi(argv[3]);
				
				//if(DutSelectedCur!=DutSelectedPre)  //切换到电压采集再切回电流 会有不需要的问题
				{
					if(Dut_isPowerON[DutSelectedPre-1])
					{
						DUT_PowerON(DutSelectedPre);
					}
					DutSelectedPre=DutSelectedCur;
					SwitchToMeasureCurrent(DutSelectedCur);		
					
				}
				result = REPLY_OK;
				rt_kprintf("ready to measure current of dut%d\n",atoi(argv[3]));

			}	
			
		}
		else result = REPLY_INVALID_CMD;
	}
	else result = REPLY_INVALID_CMD;
	
	
	if(result == REPLY_INVALID_CMD)
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("relay <channel> <1|0>\n");
		rt_kprintf("    channel:1~20\n");			
		
		rt_kprintf("relay switch voltage <channel>\n");
		rt_kprintf("relay switch current <channel>\n");
		rt_kprintf("    channel:1~6\n");		
		
	}	
	return result;
}
int power(int argc, char **argv)
{
	uint8_t result = REPLY_OK;
	
	uint8_t DutCh=0;
	uint8_t DutStatus=0;
	
	if (argc ==3 )
	{
		DutCh=atoi(argv[2]);
		if (!strcmp(argv[1], "on")) 
		{
			DUT_PowerON(DutCh);		

			rt_kprintf("dut%d is power on\n",DutCh);
		}
		else if (!strcmp(argv[1], "off")) 
		{
			DUT_PowerOFF(DutCh);	
			rt_kprintf("dut%d is power off\n",DutCh);
		}
		else result = REPLY_INVALID_CMD;
	}
	
	else result =REPLY_INVALID_CMD;
	
	if(result == REPLY_INVALID_CMD)
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("power on|off <channel>\n");
		rt_kprintf("    channel:1~6\n");			
		result = REPLY_INVALID_CMD;
	}	
	return result;
}

INIT_APP_EXPORT(mbrelayadp_hw_init);

MSH_CMD_EXPORT(relay, ......);
MSH_CMD_EXPORT(power, ......);




