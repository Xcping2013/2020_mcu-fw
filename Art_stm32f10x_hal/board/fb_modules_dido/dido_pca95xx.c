
#include "bsp_unity.h"

#include "fbio0808.h"	
#include "FBJFFB1615CN1.h"	

#include "dido_pca95xx.h"

#include "inc_dido.h"

static uint8_t command_int(char *Commands);
static uint8_t command_out(char *Commands);

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

uint8_t inputGet(uint8_t channel)
{
	uint8_t retval;
	
	SoftI2c.reads(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6A_pca95xx.in_data ,1);	
	SoftI2c.reads(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6B_pca95xx.in_data ,1);	
		
	if(channel<9) 		  retval=MB1616DEV6A_pca95xx.in_data & (1 << (channel-1))? 1:0;
	else if(channel<17) retval=MB1616DEV6B_pca95xx.in_data & (1 << (channel-9))? 1:0;
	
	else if(channel<25)	
	{
		if(FBIO0808_is_inited) 						retval=FBIO0808GetP0Bit(channel-17);
		else if(FBJFFB1615CN1_is_inited)	retval=FBJFFB1615CN1GetP0Bit(1,channel-17);
		else retval=0;
	}
	else if(channel<33)	
	{
		if(FBJFFB1615CN1_is_inited) 			retval=FBJFFB1615CN1GetP0Bit(2,channel-25);
		else retval=0;
	} 	
	return retval;
}
void outputSet(uint8_t channel, uint8_t setval)
{
	if(0<channel && channel< 33)
	{
		if(channel<9)	
		{
			MB1616DEV6A_pca95xx.out_data= setval? MB1616DEV6A_pca95xx.out_data | (1 << (channel-1)) : MB1616DEV6A_pca95xx.out_data & ((uint8_t) ~(1 << (channel-1)));
			SoftI2c.writes(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&MB1616DEV6A_pca95xx.out_data,1);
		}
		else if(channel<17)	
		{
			MB1616DEV6B_pca95xx.out_data= setval? MB1616DEV6B_pca95xx.out_data | (1 << (16-channel)) : MB1616DEV6B_pca95xx.out_data & ((uint8_t) ~(1 << (16-channel)));
			SoftI2c.writes(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&MB1616DEV6B_pca95xx.out_data,1);
		}			
		else if(channel<25)			
		{
			if(FBIO0808_is_inited) 						FBIO0808SetP1Bit(channel-17,setval);
			else if(FBJFFB1615CN1_is_inited)	FBJFFB1615CN1SetP1Bit(1,channel-17,setval);			
		}
		else
		{
			if(FBJFFB1615CN1_is_inited)
			{
				FBJFFB1615CN1SetP1Bit(2,channel-25,setval);		
			}
		}
	}
}
//
int readinputs(void)
{
		uint8_t in_data3=0;
		uint8_t in_data4=0;
		SoftI2c.reads(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6A_pca95xx.in_data ,1);	
		SoftI2c.reads(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6B_pca95xx.in_data ,1);
		if(FBIO0808_is_inited) 	
		{		
			in_data3=FBIO0808GetP0();
			rt_kprintf("+ok@input.readpin(0x%08x)\n",((in_data4))<<24|(in_data3)<<16|(MB1616DEV6B_pca95xx.in_data)<<8 | MB1616DEV6A_pca95xx.in_data);
		}
		else if(FBJFFB1615CN1_is_inited) 	
		{		
			in_data3=FBJFFB1615CN1GetP0(1);
			in_data4=FBJFFB1615CN1GetP0(2);
			rt_kprintf("+ok@input.readpin(0x%08x)\n",(data_invert_order(in_data4))<<24|(data_invert_order(in_data3))<<16|(MB1616DEV6B_pca95xx.in_data)<<8 | MB1616DEV6A_pca95xx.in_data);
		}
		else rt_kprintf("+ok@input.readpin(0x%08x)\n",(0x00<<24)|(0x00<<16)|(MB1616DEV6B_pca95xx.in_data)<<8 | MB1616DEV6A_pca95xx.in_data);

		return RT_EOK;
}
int readoutputs(void)
{
	if(FBIO0808_is_inited) 	
	{
		rt_kprintf("+ok@output.readpin(0x%08x)\n",(0x00)<<24 | (FBIO0808GetP1())<<16 | data_invert_order(MB1616DEV6B_pca95xx.out_data)<<8 | MB1616DEV6A_pca95xx.out_data);		
	}
	else if(FBJFFB1615CN1_is_inited) 	
	{
		rt_kprintf("+ok@output.readpin(0x%08x)\n",(FBJFFB1615CN1GetP1(2))<<24 | data_invert_order(FBJFFB1615CN1GetP1(1))<<16 | data_invert_order(MB1616DEV6B_pca95xx.out_data)<<8 | MB1616DEV6A_pca95xx.out_data);			
	}
	else 	rt_kprintf("+ok@output.readpin(0x%08x)\n",(0x00)<<24 | (0x00)<<16 | data_invert_order(MB1616DEV6B_pca95xx.out_data)<<8 | MB1616DEV6A_pca95xx.out_data);		

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



uint8_t	outputGet(uint8_t channel)
{
	uint8_t retdata=0;
	
	if(0<channel && channel< 31)
	{
		if(channel<9) 		  return 	(MB1616DEV6A_pca95xx.out_data & (1 << (channel-1))?  1:0);
		else if(channel<17) return	(MB1616DEV6B_pca95xx.out_data & (1 << (16-channel))? 1:0);
		else if(channel<25)
		{		
			if(FBIO0808_is_inited) 						return FBIO0808GetP1Bit(channel-17);
			else if(FBJFFB1615CN1_is_inited) 	return FBJFFB1615CN1GetP1Bit(1,channel-17);
		}
		else if(channel<32) 
		{	
			if(FBJFFB1615CN1_is_inited) 	return FBJFFB1615CN1GetP1Bit(2,channel-25);
		}
	}
	return retdata;
	
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
			uint8_t channel,i,retval=0;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				
				if(0<channel && channel< 32)
				{
					if(channel<9) 		  retval=MB1616DEV6A_pca95xx.out_data & (1 << (channel-1))?  1:0;
					else if(channel<17) retval=MB1616DEV6B_pca95xx.out_data & (1 << (16-channel))? 1:0;					
					else if(channel<25)
					{		
						if(FBIO0808_is_inited) 						retval = FBIO0808GetP1Bit(channel-17);
						else if(FBJFFB1615CN1_is_inited) 	retval = FBJFFB1615CN1GetP1Bit(1,channel-17);
						else retval=0;
					}
					else if(channel<32) 
					{	
						if(FBJFFB1615CN1_is_inited) 	retval = FBJFFB1615CN1GetP1Bit(2,channel-25);
						else retval=0;
					}										
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
		uint8_t outdata3=0;
		uint8_t	outdata4=0;
		
		if(argv[1][0]=='0' && (argv[1][1]=='x' || argv[1][1]=='X'))
		status = strtoll(&argv[1][2],NULL,16);
		else status=atoi(argv[1]);
		MB1616DEV6A_pca95xx.out_data=status;	MB1616DEV6B_pca95xx.out_data=data_invert_order(status>>8);
		
		outdata3=data_invert_order(status>>16);	outdata4=status>>24;
		SoftI2c.writes(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&MB1616DEV6A_pca95xx.out_data,1);
		SoftI2c.writes(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&MB1616DEV6B_pca95xx.out_data,1);

		if(FBIO0808_is_inited) FBIO0808SetP1(outdata3);
		else if(FBJFFB1615CN1_is_inited) 	
		{
			FBJFFB1615CN1SetP1(1,outdata3);
			FBJFFB1615CN1SetP1(2,outdata4);
		}

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
	if(!strncmp(".input.",string,7))
	{
		return 7;
	}
	if(!strncmp(".output.",string,8))
	{
		return 8;
	}
	return 0;
}

static void readpin(int8_t indx)
{
	rt_kprintf("+ok@input.readpin(%02d,%d)\n",indx,inputGet(indx+1));
	
}
//static void writepin(uint8_t indx, uint8_t status)
//{	
//	outputSet(indx+1,status);
//	rt_kprintf("+ok@output.writepin(0x%08x)\n",(pca9539a_4.out_data)<<24 | data_invert_order(fbio0808_pca95xx.out_data)<<16 | data_invert_order(MB1616DEV6B_pca95xx.out_data)<<8 | MB1616DEV6A_pca95xx.out_data);	
//}

/*******命令顺序格式一 自定义格式***********/
#if 1		

static uint8_t command_int(char *Commands)
{
	if( strcmp(Commands,"inputs")==0  ) 
	{
		uint8_t i;
		char data_read[16]="0000000000000000";
		SoftI2c.reads(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6A_pca95xx.in_data ,1);	
		SoftI2c.reads(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &MB1616DEV6B_pca95xx.in_data ,1);	
	
		for(i=0;i<8;i++)
		{
			if(MB1616DEV6A_pca95xx.in_data & (1 << i))	data_read[i]='1'; else data_read[i]='0';
			if(MB1616DEV6B_pca95xx.in_data & (1 << i))	data_read[i+8]='1'; else data_read[i+8]='0';	
		}
		rt_kprintf("%s<%.16s>\n",Commands,data_read);
		return REPLY_OK;
	}
	if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& channel>0 && channel<17)
		{	
			rt_kprintf("%s<%d>\n", Commands,inputGet(channel));					
		}
		else 
		{
			printf("\ninput channel must be 1~16\n");
		}
		return REPLY_OK;
	}
  else return REPLY_INVALID_CMD;	
}
static uint8_t command_out(char *Commands)
{
	if( strncmp(Commands,"out[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];		
		u8 setval;		
		u8 channel=strtol(s, &p, 10);	

		if(*p==']'&& *(p+1)=='[' && channel>0 && channel<17)	
		{
			s=NULL;
			setval=strtol(p+2, &s, 10);	
			if(*(s)==']'&& *(s+1)=='\0' )
			{
				outputSet(channel,setval);
				rt_kprintf("%s<OK>\n",Commands);		
			}			
			else printf("\ninput channel must be 1~16\n");
		}
		return REPLY_OK;		
	}	
  return REPLY_INVALID_CMD;	 
}

uint8_t Command_analysis_dido(char *string)
{
	if( !strcmp(string,"gpio help") )								 
	{		
		rt_kprintf("%s",string);
		printf_cmdList_gpio();		return REPLY_OK; 
	}
	
	else if(	command_int(string)		==	REPLY_OK	)			{		return REPLY_OK; }
	else if(	command_out(string)		==	REPLY_OK	)			{		return REPLY_OK; }

/*	
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
*/
	return REPLY_INVALID_CMD;
}





#endif

/*******命令顺序格式二 直接调用MSH护着FINSH****MSH_CMD_EXPORT********FINSH_FUNCTION_EXPORT***********/
#if 1			


MSH_CMD_EXPORT(readinput, Read the status of one or more inputs);
MSH_CMD_EXPORT(readinputs, Read the status of all inputs);
MSH_CMD_EXPORT(readoutput,  Read the status of one or more outputs);
MSH_CMD_EXPORT(readoutputs, Read the status of all outputs);
MSH_CMD_EXPORT(output, Set single or multiple output on|off);

FINSH_FUNCTION_EXPORT(readpin,...);
//FINSH_FUNCTION_EXPORT(writepin,...);

#endif

/****************************************************************************************/
#if 	( DBG_ENABLE )

	int writes(uint32_t status)
	{
			MB1616DEV6A_pca95xx.out_data=status;	MB1616DEV6B_pca95xx.out_data=data_invert_order(status>>8);
			fbio0808_pca95xx.out_data=data_invert_order(status>>16);	pca9539a_4.out_data=status>>24;
			SoftI2c.writes(MB1616DEV6A_pca95xx.pins,1,MB1616DEV6A_pca95xx.devAddress,3,&MB1616DEV6A_pca95xx.out_data,1);
			SoftI2c.writes(MB1616DEV6B_pca95xx.pins,1,MB1616DEV6B_pca95xx.devAddress,3,&MB1616DEV6B_pca95xx.out_data,1);
			SoftI2c.writes(fbio0808_pca95xx.pins,1,fbio0808_pca95xx.devAddress,3,&fbio0808_pca95xx.out_data,1);
			SoftI2c.writes(pca9539a_4.pins,1,pca9539a_4.devAddress,3,&pca9539a_4.out_data,1);
			rt_kprintf("+ok@output.writepins(0x%08x)\n",status);	
	}
	FINSH_FUNCTION_EXPORT(writes,...);
#endif
//
	
	










