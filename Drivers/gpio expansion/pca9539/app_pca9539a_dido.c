
#include "app_pca9539a_dido.h"

#define USING_PCA9539A	2

#if defined(USING_PCA9539A) 

pca9539a_t pca9539a_1;
pca9539a_t pca9539a_2;

uint8_t OriginSensorPin[3]={PD_12,PD_10,PD_11};

static rt_timer_t timer1;
static rt_uint8_t out_flash_on[16]={0};
static rt_uint8_t out_flash_value[16]={0};
static rt_uint16_t out_flash_delay[16]={0};
static rt_uint16_t out_flash_cnt[16]={0};

static void dido_pca9539a_init(void);
static int timer_10ms_init(void);
static void timeout_10ms(void *parameter);

/******************--->	INPUT & OUTPUT	**************************
 */
pca9539a_t pca9539a_1=
{
	{PC_14,PC_15},
	0xE8,
	0,
	0,
};
pca9539a_t pca9539a_2=
{
	{PC_14,PC_15},
	0xEC,
	0,
	0,
};

void dido_pca9539a_init(void)
{
	pca9539a_init(pca9539a_1);
	pca9539a_init(pca9539a_2);
}
//
uint8_t getChInput(uint8_t channel)
{
	SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
	SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
	return (channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0));	
}
void setChOutput(uint8_t channel, uint8_t setval)
{
	if(channel<9)	
	{
		pca9539a_1.out_data= setval? (pca9539a_1.out_data | (1 << (channel-1))) :(pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1))));
		SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
	}
	else
	{
		pca9539a_2.out_data= setval? (pca9539a_2.out_data | (1 << (16-channel))) :(pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel))));
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
	}	
}
//
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
				if(0<channel && channel< 17)
				{
					SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
					SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	

					if((argc-i)>1) rt_kprintf("in[%d]=%d,",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
					else           rt_kprintf("in[%d]=%d", channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
				}
				else 
				{		
					rt_kprintf("input channel must be 1~16\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
    }
    return 0;
}
//
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
				if(0<channel && channel< 17)
				{
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data | (1 << (channel-1)) ;
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data | (1 << (16-channel));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					if((argc-i)>1)  rt_kprintf("out[%d]=1,",channel); 
					else           	rt_kprintf("out[%d]=1",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~16\n");
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
				if(0<channel && channel< 17)
				{
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1)));
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel)));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					if((argc-i)>1)  rt_kprintf("out[%d]=0,",channel); 
					else           	rt_kprintf("out[%d]=0",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~16\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
	}
	if (argc == 4)
	{
		if (!strcmp(argv[1], "toggle"))
		{
			rt_uint8_t flash_ch=atoi(argv[2]);
			rt_uint8_t Delaycnt=atoi(argv[3]);
			if(0<flash_ch && flash_ch< 17)
			{
				if(Delaycnt)
				{
					out_flash_on[flash_ch-1]=1;
					out_flash_delay[flash_ch-1]=Delaycnt;							 
					rt_kprintf("out[%d] toggle every %dx100ms\n",flash_ch,Delaycnt);
				}
				else
				{
					out_flash_on[flash_ch-1]=0;
					if(flash_ch<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (flash_ch-1)));
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-flash_ch)));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					rt_kprintf("out[%d] stop toggle\n",flash_ch);		
				}
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("output channel must be 1~16\n");
				return RT_ERROR;
			}
		}
	}
	rt_kprintf("Usage: \n");
	rt_kprintf("output on <ch>  - set the output channel on\n");
	rt_kprintf("output off <ch>     - set the output channel off\n");
	//rt_kprintf("output toggle <delay_time>  - start or stop output toggle per time x100ms\n");
	return -RT_ERROR;
}

static int timer_10ms_init(void)
{
	timer1 = rt_timer_create("timer1", timeout_10ms,
                             RT_NULL, 10,
                             RT_TIMER_FLAG_PERIODIC);
  if (timer1 != RT_NULL) 
		rt_timer_start(timer1);	
	return 0;
}
static void timeout_10ms(void *parameter)
{
	for(uint8_t i=0;i<16;i++)
	{
		if ((out_flash_on[i]==1) && (out_flash_cnt[i]++>out_flash_delay[i]))
		{
			out_flash_value[i]=out_flash_value[i]? 0:1;
			if(i<9)	
			{
				pca9539a_1.out_data= out_flash_value[i]? (pca9539a_1.out_data | (1 << (i))) :(pca9539a_1.out_data & ((uint8_t) ~(1 << (i))));
				SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
			}
			else
			{
				pca9539a_2.out_data= out_flash_value[i]? (pca9539a_2.out_data | (1 << (17-i))) :(pca9539a_2.out_data & ((uint8_t) ~(1 << (17-i))));
				SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
			}	
			//DEBUG_TRACE("out%d is flashing\n",i+1);	
			out_flash_cnt[i]=0;
		}
	}	
}
//
INIT_APP_EXPORT(dido_pca9539a_init);
MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);//MSH_CMD_EXPORT(memcheck, check memory data);

#endif
/******************			INPUT & OUTPUT<---**************************
*/


