#include "cmds.h"	

const char INPUT_HELP[] = {
	"input commands:"
	"\n input.help()"
	"\n input.readPin()/(indx)/(indx0,indx1)"
	"\n input.enableFalling(indx)/()"
	"\n input.disableFalling(indx)/()"	
	"\n input.enableRaising(indx)/()"
	"\n input.disableRaising(indx)/()\r\n"
};
/****************************************************************************************/
int input(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("usage: \n");
			  rt_kprintf("input readpin\n");
			  rt_kprintf("input readpin <channel>\n");	
        return -RT_ERROR;
    }
    if (argc == 2)
    {	
			
    }
    else
    {
			uint8_t channel,i,retval;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				
				if(0<channel && channel< 33)
				{
					SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
					SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
					
					SoftI2c.reads(pca9539a_3.pins,1,pca9539a_3.devAddress,0, &pca9539a_3.in_data ,1);	
					SoftI2c.reads(pca9539a_4.pins,1,pca9539a_4.devAddress,0, &pca9539a_4.in_data ,1);	
				
					if(channel<9) 		  retval=pca9539a_1.in_data & (1 << (channel-1))? 1:0;
					else if(channel<17) retval=pca9539a_2.in_data & (1 << (channel-9))? 1:0;
					else if(channel<25) retval=pca9539a_3.in_data & (1 << (24-channel))? 1:0;
					else if(channel<33) retval=pca9539a_4.in_data & (1 << (32-channel))? 1:0;
					
					//if((argc-i)>1) rt_kprintf("in[%d]=%d,",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );				
					if((argc-i)>1) 
					{
						rt_kprintf("in[%d]=%d,",channel,retval );
					}
					else 
					{
						//rt_kprintf("in[%d]=%d", channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
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



void dido_hw_init(void)
{ 

}



uint8_t inputGet(uint8_t channel)
{
	SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
	SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
	return (channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0));	

}
void outputSet(uint8_t channel, uint8_t setval)
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
int readinputs(void)
{
		SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
		SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);
		SoftI2c.reads(pca9539a_3.pins,1,pca9539a_3.devAddress,0, &pca9539a_3.in_data ,1);	
		SoftI2c.reads(pca9539a_4.pins,1,pca9539a_4.devAddress,0, &pca9539a_4.in_data ,1);
	
		rt_kprintf("(0x%08x)\n",(data_invert_order(pca9539a_4.in_data))<<24|(data_invert_order(pca9539a_3.in_data))<<16|(pca9539a_2.in_data)<<8 | pca9539a_1.in_data);
		return RT_EOK;
}
int readinput(int argc, char **argv)
{
    if (argc == 1)
    {
        //rt_kprintf("Please input: readinput <ch1> <ch2> ......\n");			
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
					SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
					SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
					
					SoftI2c.reads(pca9539a_3.pins,1,pca9539a_3.devAddress,0, &pca9539a_3.in_data ,1);	
					SoftI2c.reads(pca9539a_4.pins,1,pca9539a_4.devAddress,0, &pca9539a_4.in_data ,1);	
				
					if(channel<9) 		  retval=pca9539a_1.in_data & (1 << (channel-1))? 1:0;
					else if(channel<17) retval=pca9539a_2.in_data & (1 << (channel-9))? 1:0;
					else if(channel<25) retval=pca9539a_3.in_data & (1 << (24-channel))? 1:0;
					else if(channel<33) retval=pca9539a_4.in_data & (1 << (32-channel))? 1:0;
					
					//if((argc-i)>1) rt_kprintf("in[%d]=%d,",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );				
					if((argc-i)>1) 
					{
						rt_kprintf("in[%d]=%d,",channel,retval );
					}
					else 
					{
						//rt_kprintf("in[%d]=%d", channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
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
int readoutputs(void)
{
	rt_kprintf("(0x%08x)\n",(pca9539a_4.out_data)<<24 | data_invert_order(pca9539a_3.out_data)<<16 | data_invert_order(pca9539a_2.out_data)<<8 | pca9539a_1.out_data);	
	return RT_EOK;
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
//
int writeoutputs(int argc, char **argv)
{
	if(argc==2)
	{
		  
		uint32_t status;
		
		if(argv[1][0]=='0' && (argv[1][1]=='x' || argv[1][1]=='X'))
		status = strtol(&argv[1][2],NULL,16);
		else status=atoi(argv[1]);
		pca9539a_1.out_data=status;	pca9539a_2.out_data=data_invert_order(status>>8);
		pca9539a_3.out_data=data_invert_order(status>>16);	pca9539a_4.out_data=status>>24;
		SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
		SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
		
		rt_kprintf("+ok@output.writepins(0x%08x)\n",status);	
	}
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
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data | (1 << (channel-1)) ;
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else if(channel<17)	
					{
						pca9539a_2.out_data= pca9539a_2.out_data | (1 << (16-channel));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					else if(channel<25)	
					{
						pca9539a_3.out_data= pca9539a_3.out_data | (1 << (24-channel));
						SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
					}	
					else
					{
						pca9539a_4.out_data= pca9539a_4.out_data | (1 << (channel-25));
						SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
					}	
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
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1)));
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else if(channel<17)	
					{
						pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel)));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					else if(channel<25)	
					{
						pca9539a_3.out_data= pca9539a_3.out_data & ((uint8_t) ~(1 << (24-channel)));
						SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
					}	
					else
					{
						pca9539a_4.out_data= pca9539a_4.out_data & ((uint8_t) ~(1 << (channel-25)));
						SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
					}	
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
}
uint8_t Command_analysis_dido(char *string)
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
		
		if( (*p=='\0')	&& (0<channel && channel< (IN_CNT+1)))
		{
			SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
			SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	

			rt_kprintf("\nin[%d]=%d\n",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
		}
		else 
		{
			printf("\ninput channel must be 1~%d\n",IN_CNT);
		}
	}
	else if( !strncmp(string,"output on ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (OUT_CNT+1)))
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
			printf("\nout[%d]=1\n",channel); 
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("\noutput channel must be 1~%d\n",OUT_CNT);
		}
	}
	else if( !strncmp(string,"output off ",11) ) 
	{
		char *p = NULL;
		char *s = &string[11];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (OUT_CNT+1)))
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
			printf("\nout[%d]=0\n",channel); 
		}
		else 
		{
			printf("\noutput channel must be 1~%d\n",OUT_CNT);
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}

//INIT_APP_EXPORT(dido_hw_init);
//MSH_CMD_EXPORT(readinput, read the state of the input channel);
//MSH_CMD_EXPORT(readoutput, read the state of the output channel);
//MSH_CMD_EXPORT(output, set the output channel status);

int writes(uint32_t status)
{
		pca9539a_1.out_data=status;	pca9539a_2.out_data=data_invert_order(status>>8);
		pca9539a_3.out_data=data_invert_order(status>>16);	pca9539a_4.out_data=status>>24;
		SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		SoftI2c.writes(pca9539a_3.pins,1,pca9539a_3.devAddress,3,&pca9539a_3.out_data,1);
		SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
	
	//rt_kprintf("(0x%08x)\n",(pca9539a_4.out_data)<<24 | data_invert_order(pca9539a_3.out_data)<<16 | data_invert_order(pca9539a_2.out_data)<<8 | pca9539a_1.out_data);	

		rt_kprintf("+ok@output.writepins(0x%08x)\n",status);	
}

FINSH_FUNCTION_EXPORT(writes,...);
