

#include <mpm.h>

#define VALVE_CTRL_PUFF0_PIN    GET_PIN(F, 6)   //OUTPUT
#define VALVE_CTRL_PUFF1_PIN    GET_PIN(F, 7)

#define VALVE_CTRL_PUFF2_PIN    GET_PIN(H, 1)

#define VALVE_CTRL_PUFF3_PIN    GET_PIN(C, 2)
#define VALVE_CTRL_PUFF4_PIN    GET_PIN(C, 3)
#define VALVE_CTRL_PUFF5_PIN    GET_PIN(B, 7)
#define VALVE_CTRL_PUFF6_PIN    GET_PIN(C, 13)

uint8_t valve_pin[7]=
{
    VALVE_CTRL_PUFF0_PIN,
    VALVE_CTRL_PUFF1_PIN,
    VALVE_CTRL_PUFF2_PIN,
    VALVE_CTRL_PUFF3_PIN,
    VALVE_CTRL_PUFF4_PIN,
    VALVE_CTRL_PUFF5_PIN,
    VALVE_CTRL_PUFF6_PIN,
};

void valve_hw_init(void)
{
    for(uint8_t i=0;i<7;i++)
    {
        rt_pin_mode(valve_pin[i], PIN_MODE_OUTPUT);
        rt_pin_write(valve_pin[i], PIN_LOW);
    }
}

int valveCtrl(int argc, char *argv[])
{
    static uint8_t valve_inited=0;

    uint8_t ret = RT_EOK;

    if(valve_inited==0)
    {
        valve_inited=1;
        valve_hw_init();
    }

    if (argc == 1)
    {
			rt_kprintf("Usage: \n");
			rt_kprintf("valveCtrl on <var>         - set the output channel on\n");
			rt_kprintf("valveCtrl off <var>        - set the output channel off\n");
			rt_kprintf("    var:0~6\n");
    }
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<=channel && channel< 7)
				{

					rt_pin_write(valve_pin[channel],  PIN_HIGH);
					if((argc-i)>1)  rt_kprintf("valve[%d]=1,",channel); 
					else           	rt_kprintf("valve[%d]=1",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 0~6\n");
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
				if(0<=channel && channel< 7)
				{
					rt_pin_write(valve_pin[channel],  PIN_LOW);
					if((argc-i)>1)  rt_kprintf("valve[%d]=0,",channel); 
					else           	rt_kprintf("valve[%d]=0",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 0~6\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
	}


    else
    {
        ret = RT_ERROR;
    }

    return ret;
}

int doorSensor(void)
{
	static uint8_t in_io_inited=0;
	if(in_io_inited==0)
	{
		
		in_io_inited=1;
//		pinMode(PA_7,PIN_MODE_INPUT_PULLUP);

	}
	rt_kprintf("doorSensor:PB6=%d\n",pinRead(PB_6)); 
}



/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(valveCtrl, ctrl valve 0~6 on or off);
MSH_CMD_EXPORT(doorSensor, Read the door sensor status);

