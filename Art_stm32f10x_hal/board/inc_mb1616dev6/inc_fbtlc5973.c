
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"
#include "inc_fbtlc5973.h"

#define EEPROM_LED_PAR_ADDR				 114 
#define EEPROM_LED_PAR_LEN				 1

/*					current set
命令1 led 0~100.0:25ma(红灯) 0~100.0:190mA(白灯)
命令2 led save 保存当前亮度值下次开机固定的亮度
命令3 软件可以通过savedata和readdata进行其他亮度的保存和读取
1.74K Imax=30mA

static uint16_t LedWhiteValue=0;
static uint16_t LedRedValue=0;
static float LedWhiteValue=0;
static float LedRedValue=0;
*/

typedef struct	
{
	float LedRedValue;
	float LedWhiteValue;
}
LED_Brightness_T;															

LED_Brightness_T LEDBrightness;	

void setLED(float whiteval,float redval);
	
void app_tlc5973_init(void) 
{
	static uint8_t tlc5973_inited=0;
	if(tlc5973_inited==0)
	{
		tlc5973_inited=1;
		
		bsp_tlc5973_init();
		
//		at24cxx.read(at24c256 , (EEPROM_LED_PAR_ADDR)*EEPROM_PAGE_BYTES, (uint8_t *)&LEDBrightness, sizeof(LED_Brightness_T));	//98 Byte		2页
//		setLED(LEDBrightness.LedWhiteValue,LEDBrightness.LedRedValue);	
//		delay_us(100);
//		setLED(LEDBrightness.LedWhiteValue,LEDBrightness.LedRedValue);
		
		//rt_kprintf("LedRedValue=%f, LedWhiteValue=%f\n",LEDBrightness.LedRedValue,LEDBrightness.LedWhiteValue);
	}	
}

int setLEDBrightness(int argc, char **argv)
{
	//rt_enter_critical();		
	
	app_tlc5973_init();
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("led <var1> <var2>\n");
			  rt_kprintf("    var1:red 0~100.0\n");		//
			  rt_kprintf("    var2:white 0~100.0\n");	//0.01mA
			  rt_kprintf("led save\n");
			
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "save")) 
			{				
				at24cxx.write(at24c256 , (EEPROM_LED_PAR_ADDR)*EEPROM_PAGE_BYTES, (uint8_t *)&LEDBrightness, sizeof(LED_Brightness_T));				
			  rt_kprintf("save led value ok\n");		
				//rt_kprintf("LedRedValue=%f, LedWhiteValue=%f\n",LEDBrightness.LedRedValue,LEDBrightness.LedWhiteValue);		
        return RT_EOK;	
			}
    }
    if (argc == 3)
    {
			char CurrentR[10]="0.0";
			char CurrentW[10]="0.0";
			LEDBrightness.LedWhiteValue=atof(argv[2]);
			LEDBrightness.LedRedValue=atof(argv[1]);
			
			if(LEDBrightness.LedWhiteValue>100)	LEDBrightness.LedWhiteValue=100;
			if(LEDBrightness.LedRedValue>100)		LEDBrightness.LedRedValue=100;
			
			//uint16_t LedRedValue1=LEDBrightness.LedRedValue*(4096*25/30)/100;
			
			uint16_t LedWhiteValue1=(uint16_t)(LEDBrightness.LedWhiteValue*40.96);
			uint16_t LedRedValue1=(uint16_t)(LEDBrightness.LedRedValue*40.96);

			
			for(uint8_t i=0;i<3;i++)
			{
				writeLED(1,LedWhiteValue1,LedRedValue1,0);delay_us(50);
			}
			delay_us(200);
			
//			rt_exit_critical();
//			writeLED(1,LedWhiteValue1,LedRedValue1,0);
//			delay_us(200);
			rt_kprintf("set led ok\n");
			
			//sprintf(CurrentR,"%.3f",(float)(LEDBrightness.LedRedValue/4.0));
			sprintf(CurrentR,"%.3f",(float)(LEDBrightness.LedRedValue/10.0));
			sprintf(CurrentW,"%.3f",(float)(LEDBrightness.LedWhiteValue/10.0));

			rt_kprintf("Ired=%smA,Iwhite=%smA\n",CurrentR,CurrentW);
			
			return RT_EOK;	
    }
		return RT_ERROR;
}

void setLED(float whiteval,float redval)
{
	uint16_t whitetemp=0;
	uint16_t redtemp=0;
	
	if(whiteval>100)	whiteval=100;
	if(redval>100)		redval=100;
	
	whitetemp=whiteval*(4096*10/30)/100;
	redtemp=redval*(4096*25/30)/100;
	
	writeLED(1,whitetemp,redtemp,0);
}

void set12BitLED(uint16_t whiteval,uint16_t redval)
{
	app_tlc5973_init();
//	if(whiteval>3413)	whiteval=3413;
//	if(redval>1365)		redval=1365;
//	writeLED(1,whiteval,redval,0);

while(1)
{	
	writeLED(1,1000,1000,0);
	delay_ms(200);
	writeLED(1,0,0,0);
	delay_ms(200);
}
}


MSH_CMD_EXPORT_ALIAS(setLEDBrightness, led,...);

MSH_CMD_EXPORT_ALIAS(set12BitLED, led_set,...);

FINSH_FUNCTION_EXPORT_ALIAS(set12BitLED, led_set,...);
