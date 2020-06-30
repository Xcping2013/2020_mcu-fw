/****************************************************************************************/
//#include "app_gpio_dido.h"	

//#include "bsp_include.h"	
//#include "app_include.h"
//#include "inc_mbtmc429.h"
//#include "app_sys_control.h"	
//CubeMX txt

#include "bsp_mcu_gpio.h"
#include "inc_dido.h"
/****************************************************************************************/

#define INPUT_COUNT    	12	
#define OUTPUT_COUNT    8	
#define LED_RGB_COUNT   3	

#define INPUT1	PBin(5)
#define INPUT2	PBin(6)
#define INPUT3	PBin(7)
#define INPUT4	PBin(8)
#define INPUT5	PBin(9)

#define INPUT6	PEin(0)
#define INPUT7	PEin(1)
#define INPUT8	PEin(2)
#define INPUT9	PEin(3)
#define INPUT10	PEin(4)
#define INPUT11	PEin(5)
#define INPUT12	PEin(6)

#define OUTPUT1		PAout(15)
#define OUTPUT2		PCout(10)
#define OUTPUT3		PCout(11)
#define OUTPUT4		PCout(12)
#define OUTPUT5		PDout(0)
#define OUTPUT6		PDout(1)
#define OUTPUT7		PDout(2)
#define OUTPUT8		PDout(3)

#define RGB_R 	PDout(6)
#define RGB_G 	PDout(5)
#define RGB_B 	PDout(4)

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


#if 1

uint8_t	inputs_pin_num[INPUT_COUNT]=
{
	PB_5,	//IN1
	PB_6, //IN2
	PB_7, //IN3
	PB_8,	//IN4
	PB_9,	//IN5	---HOME0
	PE_0,	//IN6	---HOME1
	PE_1,	//IN7	---HOME2
	PE_2,	//IN8
	PE_3,	//IN9
	PE_4,	//IN10
	PE_5,	//IN11
	PE_6,	//IN12
	
//PB_1,
//PD_8,
//PD_12,
//PD_9,
//PB_15,
//PB_13,
};  
uint8_t	outputs_pin_num[OUTPUT_COUNT]=
{
	PA_15	 ,
	PC_10  ,
	PC_11	 ,
	PC_12	 ,
	PD_0	 ,
	PD_1	 ,
	PD_2   ,
	PD_3   ,
//	PB_14  ,
//	PD_10  ,
//	PD_13  ,
//	PD_11  ,
//	PE_14  ,
//	PE_15	 ,
};

uint8_t rgb_pin_num[3]={PD_6,PD_5,PD_4};

//uint8_t homeSensorPin[3]={PB_9,PE_0,PE_1};//IN5-IN6-IN7
/****************************************************************************************/
/****************************************************************************************/
void dido_hw_init(void)
{    
	uint8_t i;
	for(i=0;i<INPUT_COUNT;i++)
	{
    rt_pin_mode(inputs_pin_num[i], PIN_MODE_INPUT_PULLUP); 
	}
	for(i=0;i<OUTPUT_COUNT;i++)
	{
    rt_pin_mode(outputs_pin_num[i], PIN_MODE_OUTPUT); 
		rt_pin_write(outputs_pin_num[i],PIN_LOW);
	}   
	for(i=0;i<LED_RGB_COUNT;i++)
	{
    rt_pin_mode(rgb_pin_num[i], PIN_MODE_OUTPUT); 
		rt_pin_write(rgb_pin_num[i],PIN_LOW);
	} 		
}
//
uint8_t inputGet(uint8_t channel)
{
	return rt_pin_read(inputs_pin_num[channel-1]) ? 1:0 ;
}
uint8_t getOutput(uint8_t channel)
{
	//return OUT1_GPIO_Port->ODR & OUT1_Pin ? 1:0 ;
}
void outputSet(uint8_t channel, uint8_t setval)
{
	rt_pin_write(outputs_pin_num[channel-1],setval);
}
int readinput(int argc, char **argv)
{
    if (argc == 1)
    {
        rt_kprintf("Please input: readinput <ch1> <ch2> ......\n");
        return -RT_ERROR;
    }
    else
    {
			uint8_t channel,i;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 13)
				{
					if((argc-i)>1) rt_kprintf("in[%d]=%d,",channel,rt_pin_read(inputs_pin_num[channel-1]) ? 0:1 );
					else           rt_kprintf("in[%d]=%d",channel,rt_pin_read(inputs_pin_num[channel-1]) ? 0:1 );
				}
				else 
				{		
					rt_kprintf("input channel must be 1~12\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
    }
    return 0;
}
int output(int argc, char **argv)
{
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					rt_pin_write(outputs_pin_num[channel-1],PIN_HIGH);
					if((argc-i)>1)  rt_kprintf("out[%d]=1,",channel); 
					else           	rt_kprintf("out[%d]=1",channel);
					
				}
				else 
				{		
					rt_kprintf("output channel must be 1~8\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
		else if (!strcmp(argv[1], "off")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					rt_pin_write(outputs_pin_num[channel-1],PIN_LOW);
					if((argc-i)>1)  rt_kprintf("out[%d]=0,",channel); 
					else           	rt_kprintf("out[%d]=0",channel);
				}
				else 
				{		
					rt_kprintf("output channel must be 1~8\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
	}
	rt_kprintf("Usage: \n");
	rt_kprintf("output on <ch>  - set the output channel on\n");
	rt_kprintf("output off <ch>     - set the output channel off\n");
	return -RT_ERROR;
}
int rgb(int argc, char **argv)
{
	if (argc != 2)
	{
		rt_kprintf("Please input: rgb <r|g|b|0>......\n");
		return -1;
	}
  else
  {
		//char *rgbtemp=argv[1];
//		switch(rgbtemp[0])
		switch(argv[1][0])
		{
			case 'r':	rt_pin_write(rgb_pin_num[0],1);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],0);
				break;
			case 'g':	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],1);rt_pin_write(rgb_pin_num[2],0);
				break;
			case 'b':	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],1);
				break;
			default:	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],0);
				break;		
		}
		rt_kprintf("rgb=%c\n",argv[1][0]);
	}
	return 0;
}
//
//INIT_APP_EXPORT(dido_hw_init);
MSH_CMD_EXPORT(readinput, readinput 1~12);
MSH_CMD_EXPORT(output, output on|off 1~8);
MSH_CMD_EXPORT(rgb, rgb <r|g|b|0>);

#endif


//
//int msh_gpio(int argc, char **argv)
//{
//    if (argc == 1)
//    {
//			rt_kprintf("gpio get\n");
//			rt_kprintf("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			rt_kprintf("  pin:uint8<0,15>\n");
//			rt_kprintf("gpio set");
//			rt_kprintf("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			rt_kprintf("  pin:uint8<0,15>\n");
//			rt_kprintf("  state:enum<0|1>\n");
//			
//			rt_kprintf("gpio config show\n");
//			rt_kprintf("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			rt_kprintf("  pin:uint8<0,15>\n");
//			
//			rt_kprintf("gpio config\n");
//			rt_kprintf("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			rt_kprintf("  pin:uint8<0,15>\n");
//			rt_kprintf("  mode:enum<input|outputPP|outputOD|altPP|altOD|analog|analogADC>(input)\n");
//			rt_kprintf("  pullup:enum<none|up|down>(none)\n");
//			rt_kprintf("  speed:enum<low|med|high|vhigh>(high)\n");		
//			rt_kprintf("  altFunc:uint8<0,15>(0)\n");

//      return -RT_ERROR;
//    }
//    else
//    {

//    }
//    return 0;
//}

//MSH_CMD_EXPORT_ALIAS(msh_gpio, gpio,GPIO commands);


//
