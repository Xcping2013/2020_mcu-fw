
#include "fbholyrelay.h"	
	
#include "inc_dido.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif
TCA6424A_t FBHOLYRELAY_tca6424=
{
	{PD_12,PD_13},
	TCA6424A_DEFAULT_ADDRESS,
	{0,0,0},
	{0,0,0},
};

uint8_t FBHOLYRELAY_is_inited=0;

void FBHOLYRELAYSetPortBit(uint8_t BitPosition, uint8_t Bitdata);
uint32_t FBHOLYRELAYGetPortData(void);
/****************************************************************************************/
void FBHOLYRELAY_Init(void)
{ 
	uint8_t reg_data[3]={0,0,0}; 	
	
	if(FBHOLYRELAY_is_inited)	{;}
			
  else	
	{
		SoftI2c.pin_init(FBHOLYRELAY_tca6424.pins);	
		if (SoftI2c.check(FBHOLYRELAY_tca6424.pins,FBHOLYRELAY_tca6424.devAddress) == REPLY_OK)
		{			
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P0,&reg_data[0],1); //set all outputs off		
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P1,&reg_data[0],1); //set all outputs off				
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P2,&reg_data[0],1); //set all outputs off				
			
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,CONFIG_P0,&reg_data[0],1); //Set p0~p2 as output
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,CONFIG_P1,&reg_data[0],1); //Set p0~p2 as output
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,CONFIG_P2,&reg_data[0],1); //Set p0~p2 as output

			FBHOLYRELAY_is_inited=1;
			rt_kprintf("iic device tca6424a found on FBHOLYRELAY addr=0x%x\n",FBHOLYRELAY_tca6424.devAddress);
			
			FBHOLYRELAYSetPortBit(1,1); //VBUS_ON
			FBHOLYRELAYSetPortBit(2,1);	//GND_ON
			
		}
	}
}
void FBHOLYRELAYSetPort(uint32_t Pdata)
{
	FBHOLYRELAY_tca6424.out_data[0]=Pdata;	
	FBHOLYRELAY_tca6424.out_data[1]=Pdata>>8;	
	FBHOLYRELAY_tca6424.out_data[2]=Pdata>>16;
	SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P0,&FBHOLYRELAY_tca6424.out_data[0],1);	
	SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P1,&FBHOLYRELAY_tca6424.out_data[1],1);	
	SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P2,&FBHOLYRELAY_tca6424.out_data[2],1);	

}
void FBHOLYRELAYSetPortBit(uint8_t BitPosition, uint8_t Bitdata)
{
	if(0<BitPosition && BitPosition< 21)
	{
		if(BitPosition<9)	
		{
			FBHOLYRELAY_tca6424.out_data[0]= Bitdata? FBHOLYRELAY_tca6424.out_data[0] | (1 << (BitPosition-1)) : FBHOLYRELAY_tca6424.out_data[0] & ((uint8_t) ~(1 << (BitPosition-1)));
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P0,&FBHOLYRELAY_tca6424.out_data[0],1);
		}
		else if(BitPosition<17)	
		{
			FBHOLYRELAY_tca6424.out_data[1]= Bitdata? FBHOLYRELAY_tca6424.out_data[1] | (1 << (BitPosition-9)) : FBHOLYRELAY_tca6424.out_data[1] & ((uint8_t) ~(1 << (BitPosition-9)));
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P1,&FBHOLYRELAY_tca6424.out_data[1],1);
		}	
		else if(BitPosition<25)	
		{
			FBHOLYRELAY_tca6424.out_data[2]= Bitdata? FBHOLYRELAY_tca6424.out_data[2] | (1 << (BitPosition-17)) : FBHOLYRELAY_tca6424.out_data[2] & ((uint8_t) ~(1 << (BitPosition-17)));
			SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P2,&FBHOLYRELAY_tca6424.out_data[2],1);
		}	
		//SoftI2c.writes(FBHOLYRELAY_tca6424.pins,1,FBHOLYRELAY_tca6424.devAddress,OUTPUT_P0,&FBHOLYRELAY_tca6424.out_data[2],3);

	}
}

uint32_t FBHOLYRELAYGetPortData(void)
{
	return FBHOLYRELAY_tca6424.out_data[2]<<16 | FBHOLYRELAY_tca6424.out_data[1]<<8 | FBHOLYRELAY_tca6424.out_data[0];	
}
uint8_t FBHOLYRELAYGetPortBit(uint8_t BitPosition)
{
	return (FBHOLYRELAYGetPortData() & (1 << BitPosition))? 1:0;
}

int relay(int argc, char **argv)
{
	FBHOLYRELAY_Init();
	
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 21)
				{
					FBHOLYRELAYSetPortBit(channel,1);
					if((argc-i)>1)  rt_kprintf("relay[%d]=1,",channel); 
					else           	rt_kprintf("relay[%d]=1",channel);	
				}
				else 
				{		
					rt_kprintf("relay channel must be 1~20\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
		
		if (argc==3 && !strcmp(argv[1], "set")) 
		{	
			uint32_t status;		
			if(argv[2][0]=='0' && (argv[2][1]=='x' || argv[2][1]=='X'))
			status = strtoll(&argv[2][2],NULL,16);
			else status=atoi(argv[2]);
			
			FBHOLYRELAYSetPort(status);
		
			rt_kprintf("relay set 0x%08x ok\n",status);	
	
			return RT_EOK;
		}
				
		else if (!strcmp(argv[1], "off")) 
		{	
			if(!strcmp(argv[2], "all")) 
			{			
				FBHOLYRELAYSetPort(0);
				rt_kprintf("relay[1~20]=0\n");	
				return RT_EOK;
			}
			else
			{
				uint8_t channel,i;
				for(i=2;argc>i;i++)
				{
					channel=atoi(argv[i]);
					if(0<channel && channel< 32)
					{
						FBHOLYRELAYSetPortBit(channel,0);
						if((argc-i)>1)  rt_kprintf("relay[%d]=0,",channel); 
						else           	rt_kprintf("relay[%d]=0",channel);	
					}
					else 
					{		
						rt_kprintf("relay channel must be 1~20\n");
						return RT_ERROR;
					}
				}
				rt_kprintf("\n");
				return RT_EOK;
			}
		}
		else if (!strcmp(argv[1], "read")) 
		{	
			uint8_t channel,i,retval=0;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				
				if(0<channel && channel< 21)
				{
					retval=FBHOLYRELAYGetPortBit(channel);							
					if((argc-i)>1) 
					{
						rt_kprintf("out[%d]=%d,",channel,retval );
					}
					else 
					{
						rt_kprintf("out[%d]=%d",channel,retval );
					}
				}
				else 
				{		
					rt_kprintf("relay channel must be 1~20\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");	
			return RT_EOK;
		}
	}
	else if (!strcmp(argv[1], "reads")) 
	{
		rt_kprintf("relay status:0x%08x\n",FBHOLYRELAYGetPortData());		
		return RT_EOK;
	}
	rt_kprintf("Usage: \n");
	rt_kprintf("relay off all         - set the relay channel on\n");
	rt_kprintf("relay set         - set the relay \n");

	rt_kprintf("relay on <var>         - set the relay channel on\n");
	rt_kprintf("relay off <var>        - set the relay channel off\n");
	rt_kprintf("relay read <var>       - get the status of the relay\n");
	rt_kprintf("relay reads            - get the relays status\n");
	rt_kprintf("    var:1~20\n");
	return -RT_ERROR;
}

MSH_CMD_EXPORT(relay, turn relay on|off);


