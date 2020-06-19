
#include "app_mcp3421.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DEG_TRACE		rt_kprintf
	#else
	#define DEG_TRACE(...)		
#endif

#endif

#define EEPROM_MCP3421_CAL_PAGE_ADDR		112 
#define CAL_DATA_SAVED	30

//Rmeasure=0.2Ω
//Rlimit=5mΩ
#define SAMPLE_RES 0.2

typedef struct	
{
	uint8_t saved;
	float gain;
	float offset;
}
MCP3421_CAL_T;	

MCP3421_CAL_T USB_CurrentCal=
{
	CAL_DATA_SAVED,
	1.0,
	0.0,
};

mcp3421_t VBUS_MCP3421=
{
	{PC_14,PC_15},
	MCP3421_SAMPLE_RATE_15_SPS,
	MCP3421_GAIN_8_VALUE,	
	//0x9b,
	0x1b,
};
//
void LoadMCP3421_CAL_FromEeprom(void)
{
	
	MCP3421_CAL_T Current_CAL;
	
	at24cxx.read(at24c256 , (EEPROM_MCP3421_CAL_PAGE_ADDR)*64, (uint8_t *)&Current_CAL, sizeof(MCP3421_CAL_T));		
	if(Current_CAL.saved==CAL_DATA_SAVED)
	{
		USB_CurrentCal.gain=Current_CAL.gain;
		USB_CurrentCal.offset=Current_CAL.offset;
	}	
	else 
	{
		at24cxx.write(at24c256 , (EEPROM_MCP3421_CAL_PAGE_ADDR)*64, (uint8_t *)&USB_CurrentCal, sizeof(MCP3421_CAL_T));			
	}
}


float ConvertADC_to_mA(float adcVal)
{
	float ret;
		
	ret=(( adcVal - USB_CurrentCal.offset)/SAMPLE_RES )*USB_CurrentCal.gain;	
	
	return ret;
}

// 1 0 0 1 1 0 1 1 开始转换 连续	16bit PGA=8
void mcp3421_hw_init(void)
{
	uint8_t data=0x9b;
	
	SoftI2c.pin_init(VBUS_MCP3421.pin);
  
//	SoftI2c.writes(VBUS_MCP3421.pin,0,0xd0,0x00,&data,1); 
		
	MCP3421SetConvertionRate(&VBUS_MCP3421);
	
	MCP3421SetPGAGain(&VBUS_MCP3421);
	
	MCP3421SetConvertionModeOneShot(&VBUS_MCP3421);
	
//	MCP3421StartConvertion(&VBUS_MCP3421);
	
	delay_us(10);
		
	LoadMCP3421_CAL_FromEeprom();
}
//4.6mA  73mA
int mcp3421(int argc, char **argv)
{	
	static uint8_t mcp3421_inited=0;
	
	if(mcp3421_inited==0)
	{
		mcp3421_inited=1;
		mcp3421_hw_init();
	}
	
	if (argc == 1)
	{	
			rt_kprintf("Usage: \n");
			rt_kprintf("mcp3421 getCurrent\n");
		
			rt_kprintf("\nuse calibration commands if you know what you are doing:\n\n");		
			rt_kprintf("mcp3421 offsetCal\n");		
			rt_kprintf("mcp3421 gainCal\n");			
			rt_kprintf("mcp3421 getCal\n");
			
			return REPLY_INVALID_CMD;
	}
	if (argc == 2)
	{
		if (!strcmp(argv[1], "getCurrent")) 
		{	
			MCP3421StartConvertion(&VBUS_MCP3421);
			
			char StrCurrentUSB[10]="0.0";

			sprintf(StrCurrentUSB,"%.3f",ConvertADC_to_mA(MCP3421GetValue(&VBUS_MCP3421)));
			
			rt_kprintf("usb current=%smA\n",StrCurrentUSB);
		}
		else if (!strcmp(argv[1], "offsetCal")) 
		{			
			MCP3421StartConvertion(&VBUS_MCP3421);		
			USB_CurrentCal.offset=MCP3421GetValue(&VBUS_MCP3421)/SAMPLE_RES;		
			return REPLY_OK;
		}		
		else if (!strcmp(argv[1], "getCal")) 
		{	
			char StrMCP3421_CAL[10]="0.0";
			sprintf(StrMCP3421_CAL,"%.5f",USB_CurrentCal.gain);		rt_kprintf("gain=%s\n",StrMCP3421_CAL);
			sprintf(StrMCP3421_CAL,"%.5f",USB_CurrentCal.offset);	rt_kprintf("gain=%s\n",StrMCP3421_CAL);
			return RT_EOK;	
		}
	}		
	if (argc == 3)
	{

		if (!strcmp(argv[1], "gainCal")) 
		{
			float Idmm=atof(argv[2]);
			MCP3421StartConvertion(&VBUS_MCP3421);		
			USB_CurrentCal.gain=Idmm/((MCP3421GetValue(&VBUS_MCP3421)-USB_CurrentCal.offset)/SAMPLE_RES);	
			at24cxx.write(at24c256 , (EEPROM_MCP3421_CAL_PAGE_ADDR)*64, (uint8_t *)&USB_CurrentCal, sizeof(MCP3421_CAL_T));
			return REPLY_OK;
		}							
	}
}
//INIT_APP_EXPORT(mcp3421_hw_init);
MSH_CMD_EXPORT(mcp3421, ...);

