
#include "app_tlc5973.h"
#include "bsp_mcu_delay.h"	
#include "app_eeprom_24xx.h"
#include "inc_controller.h"

//命令1 led 0~100(红灯) 0~100(白灯)
//命令2 led save 保存当前亮度值下次开机固定的亮度
//命令3 软件可以通过savedata和readdata进行其他亮度的保存和读取
//1.74K Imax=30mA
static uint16_t LedWhiteValue=0;
static uint16_t LedRedValue=0;



void setLED(uint8_t whiteval,uint8_t redval);
	
void app_tlc5973_init(void) 
{
	if(g_tParam.Project_ID==OKOA_LED)
	{
		bsp_tlc5973_init();
		
		at24cxx.read(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64, (uint8_t *)&LedRedValue, 1);
		at24cxx.read(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64+1, (uint8_t *)&LedWhiteValue, 1);
		
		setLED(LedWhiteValue,LedRedValue);
		
		//rt_kprintf("LedRedValue=%d, LedWhiteValue=%d\n",LedRedValue,LedWhiteValue);
	}
	
}

int setLEDBrightness(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("led <var1> <var2>\n");
			  rt_kprintf("    var1:red 0~100\n");
			  rt_kprintf("    var2:white 0~100\n");
			  rt_kprintf("led save\n");
			
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "save")) 
			{					
				at24cxx.write(at24c256,(EEPROM_VAR_PAGE_ADDR+1)*64,(uint8_t *)&LedRedValue,1);
				at24cxx.write(at24c256,(EEPROM_VAR_PAGE_ADDR+1)*64+1,(uint8_t *)&LedWhiteValue,1);
			  rt_kprintf("save led value ok\n");		

				//rt_kprintf("LedRedValue=%d, LedWhiteValue=%d\n",LedRedValue,LedWhiteValue);				
        return RT_EOK;	
			}
    }
    if (argc == 3)
    {
			LedWhiteValue=atoi(argv[2]);
			LedRedValue=atoi(argv[1]);
			if(LedWhiteValue>100)	LedWhiteValue=100;
			if(LedRedValue>100)		LedRedValue=100;
	
			uint16_t LedWhiteValue1=LedWhiteValue*(2047*10/30)/100;
			uint16_t LedRedValue1=LedRedValue*(2047*25/30)/100;
			writeLED(1,LedWhiteValue1,LedRedValue1,0);
			rt_kprintf("set led ok\n");
			return RT_EOK;	
    }
		return RT_ERROR;
}

void setLED(uint8_t whiteval,uint8_t redval)
{
	uint16_t whitetemp=0;
	uint16_t redtemp=0;
	
	if(whiteval>100)	whiteval=100;
	if(redval>100)		redval=100;
	
	whitetemp=whiteval*(2047*10/30)/100;
	redtemp=redval*(2047*25/30)/100;
	writeLED(1,whitetemp,redtemp,0);
}

void led_text(void) 
{
	
	
}

INIT_APP_EXPORT(app_tlc5973_init);

MSH_CMD_EXPORT_ALIAS(setLEDBrightness, led,...);

FINSH_FUNCTION_EXPORT(setLED, led_test);
