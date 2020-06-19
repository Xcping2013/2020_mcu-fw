
#include "app_pca9539a_dido.h"
#include "inc_dido.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DEG_TRACE		rt_kprintf
	#else
	#define DEG_TRACE(...)		
#endif

#endif
/*******************************************app pca9539a*******************************
																						app pca9539a 
********************************************app pca9539a******************************/
pca9539a_t pca9539a_1=
{
	{PC_14,PC_15},
	//0xE8,	//PCA9539
	0x48,	//PCA9555
	0,
	0,
};
pca9539a_t pca9539a_2=
{
	{PC_14,PC_15},
	//0xEC,	//PCA9539	
	0x4C,	//PCA9555	
	0,
	0,
};
pca9539a_t pca9539a_3=
{
	//{DEV0PA1_PIN,DEV0PA2_PIN},
	//0xEC,	//PCA9539
	
	{PD_6,PD_7},
	0x4C,	//PCA9555
	0,
	0,
};
pca9539a_t pca9539a_4=
{
	//{DEV0PC0_PIN,DEV0SCK_PIN},
	//0xEC PCA9539	
	
	{PD_4,PB_3},	
	0x4C,	//PCA9555		
	0,
	0,
};
////87654321 -> 12345678
/****************************************************************************************/
uint8_t pca9539a_devAddR[4]={0x48,0xE8,0x4C,0xEC};
uint8_t data_invert_order(uint8_t data); 
uint8_t pca9539a_is_inited=0;
/****************************************************************************************/
static void dido_pca9539a_init(pca9539a_t pca9539a_n);
/****************************************************************************************/
void dido_hw_init(void)
{
	if(pca9539a_is_inited==0)
	{
		dido_pca9539a_init(pca9539a_1);	//由于IIC引脚一样，时有同样的地址会造成重复初始化或者未初始化现象
		dido_pca9539a_init(pca9539a_2);
		dido_pca9539a_init(pca9539a_3);
		dido_pca9539a_init(pca9539a_4);
		
		if(pca9539a_is_inited!=0)
		{
			rt_kprintf("iic device pca95xx [found]\n");
		}
	}	
}
static void dido_pca9539a_init(pca9539a_t pca9539a_n)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		pca9539a_n.devAddress=pca9539a_devAddR[i];  
		if(	pca9539a_init(pca9539a_n)==REPLY_OK)
		{
			pca9539a_is_inited++;
			
			//pca9539a_devAddR[i]+=1;//防止同一个IIC上的设备重复初始化，却不能防止不一样的IIC设备问题
//			rt_kprintf("iic device pca95xx [found] addr1=%d\n",pca9539a_n.devAddress);
//			break;
		}	
	}	
}
uint8_t inputGet(uint8_t channel)
{
	uint8_t retval;
	
	SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
	SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
	
	if(channel>16)
	{			
		SoftI2c.reads(pca9539a_3.pins,1,pca9539a_3.devAddress,0, &pca9539a_3.in_data ,1);	
		SoftI2c.reads(pca9539a_4.pins,1,pca9539a_4.devAddress,0, &pca9539a_4.in_data ,1);	
	}		
	if(channel<9) 		  retval=pca9539a_1.in_data & (1 << (channel-1))? 1:0;
	else if(channel<17) retval=pca9539a_2.in_data & (1 << (channel-9))? 1:0;
	else if(channel<25) retval=pca9539a_3.in_data & (1 << (24-channel))? 1:0;
	else if(channel<33) retval=pca9539a_4.in_data & (1 << (32-channel))? 1:0;
	
	return retval;
}
void outputSet(uint8_t channel, uint8_t setval)
{
	if(0<channel && channel< 33)
	{
		if(channel<9)	
		{
			pca9539a_1.out_data= setval? pca9539a_1.out_data | (1 << (channel-1)) : pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1)));
			SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
		}
		else if(channel<17)	
		{
			pca9539a_2.out_data= setval? pca9539a_2.out_data | (1 << (16-channel)) : pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel)));
			SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		}	
		else if(channel<25)	
		{
			pca9539a_3.out_data= setval? pca9539a_3.out_data | (1 << (24-channel)) : pca9539a_3.out_data & ((uint8_t) ~(1 << (24-channel)));
			SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
		}	
		else
		{
			pca9539a_4.out_data= setval? pca9539a_4.out_data | (1 << (channel-25)) : pca9539a_4.out_data & ((uint8_t) ~(1 << (channel-25)));
			SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
		}
	}
}
//
int readinputs(void)
{
		SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
		SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);
		if(pca9539a_is_inited>=3)
		{			
			SoftI2c.reads(pca9539a_3.pins,1,pca9539a_3.devAddress,0, &pca9539a_3.in_data ,1);	
			SoftI2c.reads(pca9539a_4.pins,1,pca9539a_4.devAddress,0, &pca9539a_4.in_data ,1);	
		}	
		rt_kprintf("+ok@input.readpin(0x%08x)\n",(data_invert_order(pca9539a_4.in_data))<<24|(data_invert_order(pca9539a_3.in_data))<<16|(pca9539a_2.in_data)<<8 | pca9539a_1.in_data);
		return RT_EOK;
}
int readoutputs(void)
{
	rt_kprintf("+ok@output.readpin(0x%08x)\n",(pca9539a_4.out_data)<<24 | data_invert_order(pca9539a_3.out_data)<<16 | data_invert_order(pca9539a_2.out_data)<<8 | pca9539a_1.out_data);	
	return RT_EOK;
}
int readinput(int argc, char **argv)
{
    if (argc == 1)
    {
			  rt_kprintf("Usage: \n");
			  rt_kprintf("readinput <var>\n");
			  rt_kprintf("    var:1~32\n");		
        return -RT_ERROR;
    }
    else
    {
			uint8_t channel,i,retval;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				
				if(0<channel && channel< 33)
				{
					retval=inputGet(channel);
					if((argc-i)>1) 
					{
						rt_kprintf("in[%d]=%d,",channel,retval );
					}
					else 
					{
						rt_kprintf("in[%d]=%d",channel,retval );
					}
				}
				else 
				{		
					rt_kprintf("input channel must be 1~32\n");
					return RT_ERROR;
				}

			}
			rt_kprintf("\n");
    }
    return 0;
}

uint8_t data_invert_order(uint8_t data)
{
	uint8_t t,out_data=0;

	uint8_t byte=data;
	
	for(t=0;t<8;t++)	
	{
		out_data+=((byte & 0x80)>>(7-t));
			byte<<=1;
	}	
	return out_data;
}

int readoutput(int argc, char **argv)
{
    if (argc == 1)
    {
        //rt_kprintf("Please input: readinput <ch1> <ch2> ......\n");			
			  rt_kprintf("Usage: \n");
			  rt_kprintf("readoutput <var>\n");
			  rt_kprintf("    var:1~31\n");
			
        return -RT_ERROR;
    }
    else
    {
			uint8_t channel,i,retval;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				
				if(0<channel && channel< 31)
				{
					if(channel<9) 		  retval=pca9539a_1.out_data & (1 << (channel-1))?  1:0;
					else if(channel<17) retval=pca9539a_2.out_data & (1 << (16-channel))? 1:0;
					else if(channel<25) retval=pca9539a_3.out_data & (1 << (24-channel))? 1:0;
					else if(channel<32) retval=pca9539a_4.out_data & (1 << (channel-31))? 1:0;
					
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
					rt_kprintf("output channel must be 1~31\n");
					return RT_ERROR;
				}

			}
			rt_kprintf("\n");
    }
    return 0;
}
//
int writeoutputs(int argc, char **argv)
{
	if(argc==2)
	{
		  
		uint32_t status;
		
		if(argv[1][0]=='0' && (argv[1][1]=='x' || argv[1][1]=='X'))
		status = strtoll(&argv[1][2],NULL,16);
		else status=atoi(argv[1]);
		pca9539a_1.out_data=status;	pca9539a_2.out_data=data_invert_order(status>>8);
		pca9539a_3.out_data=data_invert_order(status>>16);	pca9539a_4.out_data=status>>24;
		SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
		SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
		
		rt_kprintf("+ok@output.writepins(0x%08x)\n",status);	
	}
	return RT_EOK;
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
				if(0<channel && channel< 32)
				{
					outputSet(channel,1);
					if((argc-i)>1)  rt_kprintf("out[%d]=1,",channel); 
					else           	rt_kprintf("out[%d]=1",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~31\n");
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
				if(0<channel && channel< 32)
				{
					outputSet(channel,0);
					if((argc-i)>1)  rt_kprintf("out[%d]=0,",channel); 
					else           	rt_kprintf("out[%d]=0",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~31\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
	}
	rt_kprintf("Usage: \n");
	rt_kprintf("output on <var>         - set the output channel on\n");
	rt_kprintf("output off <var>        - set the output channel off\n");
	rt_kprintf("    var:1~16\n");
	return -RT_ERROR;
}
static uint8_t  command_int(char *Commands)
{
	if( strcmp(Commands,"inputs")==0  ) 
	{
		uint8_t i;
		char data_read[16]="0000000000000000";
		SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
		SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
		for(i=0;i<8;i++)
		{
			if(pca9539a_1.in_data & (1 << i))	data_read[i]='1'; else data_read[i]='0';
			if(pca9539a_2.in_data & (1 << i))	data_read[i+8]='1'; else data_read[i+8]='0';	
		}
		rt_kprintf("<%.16s>\n",data_read);
		return REPLY_OK;
	}
	else if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& channel>0 && channel<17)
		{	
//			PCA9539A_TRACE("pca9539a_1.in_data=%d,pca9539a_2.in_data=%d\n",pca9539a_1.in_data,pca9539a_2.in_data);
			SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
			SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
//			PCA9539A_TRACE("pca9539a_1.in_data=%d,pca9539a_2.in_data=%d\n",pca9539a_1.in_data,pca9539a_2.in_data);
			rt_kprintf("<%d>\n", channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0));	
				
			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;	 
	}
  else return REPLY_INVALID_CMD;	
}
static uint8_t  command_out(char *Commands)
{
	if( strncmp(Commands,"out[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];		
		u8 setval;		
		u8 channel=strtol(s, &p, 10);	

		if(*p==']'&& *(p+1)=='[' && channel>0 && channel<=16)	
		{
			s=NULL;
			setval=strtol(p+2, &s, 10);	
			if(*(s)==']'&& *(s+1)=='\0' )
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
				//PCA9539A_TRACE("pca9539a_1.out_data=%d,pca9539a_2.out_data=%d\n",pca9539a_1.out_data,pca9539a_2.out_data);			
				rt_kprintf("<OK>\n");
				return REPLY_OK;
			}			
		}
		else return REPLY_INVALID_VALUE;	 
	}	
  return REPLY_INVALID_CMD;	 
}
static void printf_cmdList_gpio(void)
{
		rt_kprintf("\nGPIO Usage: \n");
		rt_kprintf("int[var]----------------get the input status\n");
	  rt_kprintf("  var:1~16\n");
		rt_kprintf("out[var1][var2]---------set the output status\n");
		rt_kprintf("  var1:1~16\n");
		rt_kprintf("  var2:0|1\n");
  	rt_kprintf("\n");
		rt_kprintf("readinput <var>: read the state of the input channel\n");
		rt_kprintf("output on <var>: set the output on\n");
  	rt_kprintf("output off <var>: set the output off\n");	
		rt_kprintf("  var1:1~16\n");
		rt_kprintf("input.readpin(indx)|()\n");
		rt_kprintf("output.readpin()\n");
		rt_kprintf("output.writepin(indx, b)\n");
}
uint8_t MB1616_DIDO_msh(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"gpio help") )	{printf_cmdList_gpio();}
	
	else if(!command_int(string))			{;}
	else if(!command_out(string))			{;}		
		
	else if( !strncmp(string,"readinput ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (32+1)))
		{
			rt_kprintf("\nin[%d]=%d\n",channel,inputGet( channel));
		}
		else 
		{
			printf("\ninput channel must be 1~32\n");
		}
	}
	else if( !strncmp(string,"output on ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (32+1)))
		{
			outputSet(channel,1);
			printf("\nout[%d]=1\n",channel); 
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("\noutput channel must be 1~32\n");
		}
	}
	else if( !strncmp(string,"output off ",11) ) 
	{
		char *p = NULL;
		char *s = &string[11];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (32+1)))
		{
			outputSet(channel,0);
			printf("\nout[%d]=0\n",channel); 
		}
		else 
		{
			printf("\noutput channel must be 1~32\n");
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}




uint8_t MB1616_DIDO_Finsh(char* string)
{
	if(!strncmp("input.readpin()",string,15))
	{
		rt_kprintf("\n");
		readinputs();
		return 255;
	}
	if(!strncmp("output.readpin()",string,16))
	{
		rt_kprintf("\n");
		readoutputs();
		return 255;
	}
	if(!strncmp("input.",string,6))
	{
		return 6;
	}
	if(!strncmp("output.",string,7))
	{
		return 7;
	}
	return 0;
}

static void readpin(int8_t indx)
{
	rt_kprintf("+ok@input.readpin(%02d,%d)\n",indx,inputGet(indx+1));
	
}
static void writepin(uint8_t indx, uint8_t status)
{	
	outputSet(indx+1,status);
	rt_kprintf("+ok@output.writepin(0x%08x)\n",(pca9539a_4.out_data)<<24 | data_invert_order(pca9539a_3.out_data)<<16 | data_invert_order(pca9539a_2.out_data)<<8 | pca9539a_1.out_data);	
}
/****************************MSH_CMD_EXPORT*****************FINSH_FUNCTION_EXPORT*******************************************/
MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(readinputs, read the state of the input channels);
MSH_CMD_EXPORT(readoutput, read the state of the output channel);
MSH_CMD_EXPORT(readoutputs, read the state of the output channels);
MSH_CMD_EXPORT(output, set the output channel status);

FINSH_FUNCTION_EXPORT(readpin,...);
FINSH_FUNCTION_EXPORT(writepin,...);
/****************************************************************************************/
#if 	( DBG_ENABLE )

	int writes(uint32_t status)
	{
			pca9539a_1.out_data=status;	pca9539a_2.out_data=data_invert_order(status>>8);
			pca9539a_3.out_data=data_invert_order(status>>16);	pca9539a_4.out_data=status>>24;
			SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
			SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
			SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
			SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
			rt_kprintf("+ok@output.writepins(0x%08x)\n",status);	
	}
	FINSH_FUNCTION_EXPORT(writes,...);
#endif










