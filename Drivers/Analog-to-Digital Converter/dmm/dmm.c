#include "dmm.h"
#include "app_eeprom_24xx.h"

uint8_t  dmm_fun_type	= DMM_FUNC_UNKNOW;
static uint8_t  dmm_data_type	= data;
/***************************************dmm_kc_eeprom****************************************************/
#if 1	
#define EEPROM_DMM_CAL_PAGE_ADDR		115 
#define EEPROM_DMM_CAL_PAGE_LEN	 	  2								
		
#define CAL_DATA_SAVED	'Y'

DMM_Calibration_T DMM;

float Cal_GetK( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	DMM.KTable[ _Func ];	
	}
	return 1.0;
} 
void Cal_SetK( int _Func , float _K )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		DMM.KTable[ _Func ] = _K ;
	}
}
float Cal_GetC( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	DMM.CTable[ _Func ];	
	}
	return 0.0;
}
void Cal_SetC( int _Func , float _C )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		DMM.CTable[ _Func ] = _C ;
	}
}
void CalDefault_IdentifyRam( void )
{
	unsigned int i= 0;
	for( ; i < DMM_FUNC_COUNT ; ++i )
	{
		DMM.KTable[ i ] = 1.0f;	
		DMM.CTable[ i ] = 0.0f;	
	}
	DMM.saved=CAL_DATA_SAVED;
	DMM.averTimes=1;
}
void Cal_RamToEEPROM( void )
{
	at24cxx.write(at24c256 , (EEPROM_DMM_CAL_PAGE_ADDR)*64, (uint8_t *)&DMM, sizeof(DMM_Calibration_T));			
}
void Cal_EEPROMToRam()
{
	uint8_t tempFlag;
	at24cxx.read(at24c256 , (EEPROM_DMM_CAL_PAGE_ADDR)*64,&tempFlag, 1);		

	if( tempFlag == CAL_DATA_SAVED )
	{
		at24cxx.read(at24c256 , (EEPROM_DMM_CAL_PAGE_ADDR)*64, (uint8_t *)&DMM, sizeof(DMM_Calibration_T));		
	}
	else
	{
		CalDefault_IdentifyRam();
	}
}

#endif
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/***************************************dmm_function****************************************************/
#if 1

__weak void adc_hw_init0(void)
{
	
}
__weak void adc_print_data0( int _Func )
{
	
	
}

void dmm_init(void)
{
	dmm_fun_type=DMM_FUNC_UNKNOW;
	dmm_data_type	= data;
	
	Cal_EEPROMToRam();
	
	adc_hw_init();
}

void dmm_get(uint8_t dmm_data_type,uint8_t dmm_fun_type)
{
	char StrRet[6]="";	
	
	switch(dmm_data_type)
	{

		case data:
			
		    adc_print_data(dmm_fun_type);
			
				break;
		
		case sps:
			  break;
		
		case gain:								
				sprintf(StrRet,"%.5f",DMM.KTable[dmm_fun_type]);		rt_kprintf("K[%d]=%s\n",dmm_fun_type,StrRet);		
				break;
		case offset:
				sprintf(StrRet,"%.5f",DMM.CTable[dmm_fun_type]);		rt_kprintf("C[%d]=%s\n",dmm_fun_type,StrRet);		
				break;
		case averTimes:
				rt_kprintf("averTimes=%d\n",DMM.averTimes);		
				break;
		default:
				break;
	}	
}
void dmm_set(uint8_t dmm_data_type,uint8_t dmm_fun_type, char* value)
{
	switch(dmm_data_type)
	{
		case data:
			
				break;
		
		case sps:
	
		break;
				
		case gain:			
					Cal_SetK(dmm_fun_type,atof(value));		Cal_RamToEEPROM();	
					rt_kprintf("set K[%d] ok\n",dmm_fun_type);				
				break;
		case offset:
					Cal_SetC(dmm_fun_type,atof(value));		Cal_RamToEEPROM();	
					rt_kprintf("set C[%d] ok\n",dmm_fun_type);	
				break;
		case averTimes:
				DMM.averTimes=atoi(value);
				rt_kprintf("set averTimes ok\n");		
				break;
		default:
				break;
	}	
}
#endif


/***************************************dmm_msh_cmd****************************************************/
#if 1

static void	printf_cmdList_dmm(void)
{
	rt_kprintf("\n");
	rt_kprintf("dmm usage: \n");
	rt_kprintf("dmm get <type> <fun>\n");	

	rt_kprintf("    type: data | gain | offset \n");
	rt_kprintf("    fun: DMM_FUNC_DCV_100mV | DMM_FUNC_OHM_10R_4W | DMM_FUNC_DCI_500mA\n");
	rt_kprintf("dmm set <type> <fun> <value>\n");	
	rt_kprintf("    type: gain | offset \n");
	rt_kprintf("    fun: DMM_FUNC_DCV_100mV | DMM_FUNC_OHM_10R_4W | DMM_FUNC_DCI_500mA\n");
}
static void printf_cmdList_dmm_set(void)
{
		rt_kprintf("\ndmm set usage: \n");	
		rt_kprintf("dmm set gain <fun> <value>\n");	
		rt_kprintf("dmm set offset <fun> <value>\n");
		
}
static void printf_cmdList_dmm_get(void)
{
	rt_kprintf("\ndmm get usage: \n");
	rt_kprintf("dmm get <data|gain|offset> <fun>\n");
}

/*****************************************************************************************/
int dmm(int argc, char **argv)
{
	int result = REPLY_OK;
	
	static uint8_t dmm_inited=0;
	
//	if(dmm_inited==0)
//	{
//		dmm_inited=1;
//		dmm_init();
//	}
	/*
	dmm
	*/
	if		  (argc == 1 )  									{	printf_cmdList_dmm();	}
	/*
	dmm set
	dmm get
	*/
	else if (argc == 2 )					
	{
		if 			 (!strcmp(argv[1], "set"))		{ printf_cmdList_dmm_set();}
		else	if (!strcmp(argv[1], "get"))		{	printf_cmdList_dmm_get();}
		else																	{ result = REPLY_INVALID_CMD;}
	}
	else if (argc == 3 )					
	{
		if(!strcmp(argv[1], "get") && !strcmp(argv[2], "avertime"))
		{
				rt_kprintf("averTimes=%d\n",DMM.averTimes);		
		}
		else																	{ result = REPLY_INVALID_CMD;}
	}
	else if (argc == 4)
	{	
		if (!strcmp(argv[1], "get"))
		{
			if      (!strcmp(argv[3], "DMM_FUNC_DCV_100mV")) 	 	dmm_fun_type=DMM_FUNC_DCV_100mV;
			else if (!strcmp(argv[3], "DMM_FUNC_DCV_10V")) 		  dmm_fun_type=DMM_FUNC_DCV_10V;
			else if (!strcmp(argv[3], "DMM_FUNC_DCV_25V")) 		  dmm_fun_type=DMM_FUNC_DCV_25V;
			
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_100R_2W")) 	dmm_fun_type=DMM_FUNC_OHM_100R_2W;
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_10K_2W")) 	dmm_fun_type=DMM_FUNC_OHM_10K_2W;                       
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_1M_2W")) 		dmm_fun_type=DMM_FUNC_OHM_1M_2W;
			
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_10R_4W")) 	dmm_fun_type=DMM_FUNC_OHM_10R_4W;
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_10K_4W")) 	dmm_fun_type=DMM_FUNC_OHM_10K_4W;
			else if (!strcmp(argv[3], "DMM_FUNC_OHM_1M_4W")) 	  dmm_fun_type=DMM_FUNC_OHM_1M_4W;
			
			else if (!strcmp(argv[3], "DMM_FUNC_DCI_100mA")) 		dmm_fun_type=DMM_FUNC_DCI_100mA;
			else if (!strcmp(argv[3], "DMM_FUNC_DCI_500mA")) 		dmm_fun_type=DMM_FUNC_DCI_500mA;
			else if (!strcmp(argv[3], "DMM_FUNC_DCI_1A")) 	  	dmm_fun_type=DMM_FUNC_DCI_1A;
		
			//ÃüÁîÕýÈ·
			if( dmm_fun_type > DMM_FUNC_UNKNOW && dmm_fun_type < DMM_FUNC_COUNT )
			{
				
				if(!strcmp(argv[2], "gain")) 	 					dmm_data_type=gain;
				else if(!strcmp(argv[2], "offset")) 		dmm_data_type=offset;
				else if(!strcmp(argv[2], "sps")) 				dmm_data_type=sps;
				else if(!strcmp(argv[2], "avertime")) 	dmm_data_type=averTimes;
				else 																		dmm_data_type=data;
				dmm_get(dmm_data_type,dmm_fun_type);	
			}
			//ÃüÁî´íÎó
			else 
			{
				result=REPLY_WRONG_TYPE;
				printf_cmdList_dmm_get();			
			}
		}	
		else if(!strcmp(argv[1], "set") && !strcmp(argv[2], "avertime"))
		{
				DMM.averTimes=atoi(argv[3]);
		}
	}
	else if (argc == 5)
	{
		if (!strcmp(argv[1], "set"))
		{
			if (!strcmp(argv[2], "gain")) 					 		  dmm_data_type=gain;
			else if (!strcmp(argv[2], "sps")) 						dmm_data_type=sps;	
			else if (!strcmp(argv[2], "offset")) 					dmm_data_type=offset;	
			
			if(dmm_data_type!=data)	
			{
				if      (!strcmp(argv[3], "DMM_FUNC_DCV_100mV")) 	 	dmm_fun_type=DMM_FUNC_DCV_100mV;
				else if (!strcmp(argv[3], "DMM_FUNC_DCV_10V")) 		  dmm_fun_type=DMM_FUNC_DCV_10V;
				else if (!strcmp(argv[3], "DMM_FUNC_DCV_25V")) 		dmm_fun_type=DMM_FUNC_DCV_25V;
				
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_100R_2W")) 	dmm_fun_type=DMM_FUNC_OHM_100R_2W;
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_10K_2W")) 	dmm_fun_type=DMM_FUNC_OHM_10K_2W;                       
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_1M_2W")) 		dmm_fun_type=DMM_FUNC_OHM_1M_2W;
				
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_10R_4W")) 	dmm_fun_type=DMM_FUNC_OHM_10R_4W;
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_10K_4W")) 	dmm_fun_type=DMM_FUNC_OHM_10K_4W;
				else if (!strcmp(argv[3], "DMM_FUNC_OHM_1M_4W")) 	  dmm_fun_type=DMM_FUNC_OHM_1M_4W;
				
				else if (!strcmp(argv[3], "DMM_FUNC_DCI_100mA")) 		dmm_fun_type=DMM_FUNC_DCI_100mA;
				else if (!strcmp(argv[3], "DMM_FUNC_DCI_500mA")) 		dmm_fun_type=DMM_FUNC_DCI_500mA;
				else if (!strcmp(argv[3], "DMM_FUNC_DCI_1A")) 	  	dmm_fun_type=DMM_FUNC_DCI_1A;

				if( dmm_fun_type > DMM_FUNC_UNKNOW && dmm_fun_type < DMM_FUNC_COUNT )
				{				
					dmm_set(dmm_data_type,dmm_fun_type,argv[4]);	
				}		
			}
			//ÀàÐÍ´íÎó
			else 
			{
				result =REPLY_WRONG_TYPE;				
				printf_cmdList_dmm_set();													
			}				
		}
	}		
	if( result == REPLY_INVALID_CMD )	{	printf_cmdList_dmm();	}
	return result;
}
#endif
MSH_CMD_EXPORT(dmm, ......);
//


//


