
#include "cal.h"
#include "string.h"
#include "dmm.h"
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"

//#define FLASH_USER_START_ADDR   // ((uint32_t)0x0803F800)   /* Start @ of user Flash area */
//#define FLASH_USER_END_ADDR     //ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE   /* End @ of user Flash area */

#define FLASH_USER_START_ADDR    ((uint32_t)0x08019000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_99 + FLASH_PAGE_SIZE   /* End @ of user Flash area */


#define FLASH_ADDR ( 0x08000000 + 100 * 1024 )	//page50

#define DataIsSaved	0xA5A1A2A4

uint32_t PAGEError = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

float KTable[ DMM_FUNC_COUNT ];

float CTable[ DMM_FUNC_COUNT ];

float Cal_GetK( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	KTable[ _Func ];	
	}
	return 1.25f;
} 

void Cal_SetK( int _Func , float _K )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		KTable[ _Func ] = _K ;
	}
}

float Cal_GetC( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	CTable[ _Func ];	
	}
	return 1.25f;
} 

void Cal_SetC( int _Func , float _C )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		CTable[ _Func ] = _C ;
	}
}

void Cal_IdentifyRam( void )
{
	unsigned int i= 0;
	for( ; i < DMM_FUNC_COUNT ; ++i )
	{
		KTable[ i ] = 1.0f;	
		CTable[ i ] = 0.0f;	
		
	}	
	DMM_CAL_Table.filterTimes=2;
}

// 将RAM中的校准数据保存到Flash
void Cal_RamToFlash( void )
{
	unsigned int i = 0;
	unsigned int Addr = FLASH_ADDR;
	HAL_FLASH_Unlock();
	
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = Addr;
	EraseInitStruct.NbPages=1;
  //EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Addr, DataIsSaved);

	Addr += 4;
   for( i = 0 ; i < sizeof( KTable ) / sizeof(KTable[0]) ; ++i )
	{
       	HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, Addr , *(((u32*)(&KTable))+i));
		Addr += 4;
	}
    for( i = 0 ; i < sizeof( CTable ) / sizeof(CTable[0]) ; ++i )
	{
       	HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, Addr , *(((u32*)(&CTable))+i));
		Addr += 4;
	}
	HAL_FLASH_Lock();
}
// 将flash 中的数据加载到
void Cal_FlashToRam()
{
	unsigned int i = 0;
	float* pFlash = (float*)(FLASH_ADDR);
    HAL_FLASH_Unlock();
	if( *(u32*)pFlash == DataIsSaved )
	{
    	++pFlash;
		for( i = 0 ; i < sizeof( KTable ) / sizeof(KTable[0]) ; ++i )
		{
	       	*(((float*)(&KTable))+i) = *pFlash;
			++pFlash;
		}
		for( i = 0 ; i < sizeof( CTable ) / sizeof(CTable[0]) ; ++i )
		{
	       	*(((float*)(&CTable))+i) = *pFlash;
			++pFlash;
		}
	}
	else
	{
		Cal_IdentifyRam();
	}
    HAL_FLASH_Lock();
}
 
#if 1

static float KRead[ DMM_FUNC_COUNT ];

static float CRead[ DMM_FUNC_COUNT ];

void flash_data_write_test(void)
{
	unsigned int i= 0;
	for( ; i < DMM_FUNC_COUNT ; ++i )
	{
		KTable[ i ] = 12.34f;	
		CTable[ i ] = 43.21f;	
	}
	Cal_RamToFlash();
}
void flash_data_read_test(void)
{
	unsigned int i = 0;
	
	uint8_t falsh_value_str[10];

	float* pFlash = (float*)(FLASH_ADDR);
   HAL_FLASH_Unlock();
	if( *(u32*)pFlash == 0xA5A1A2A4 )
	{
    	++pFlash;
		for( i = 0 ; i < sizeof( KRead ) / sizeof(KRead[0]) ; ++i )
		{
	       	*(((float*)(&KRead))+i) = *pFlash;
			++pFlash;
		}
		for( i = 0 ; i < sizeof( CRead ) / sizeof(CRead[0]) ; ++i )
		{
	       	*(((float*)(&CRead))+i) = *pFlash;
			++pFlash;
		}
	}
	HAL_FLASH_Lock();
	for( i=0; i < DMM_FUNC_COUNT ; ++i )
	{
		sprintf(falsh_value_str,"%lf",KRead[i]);							
		AD7124_TRACE("KRead[%d]=%s\n",i,falsh_value_str);
		sprintf(falsh_value_str,"%lf",CRead[i]);							
		AD7124_TRACE("CRead[%d]=%s\n",i,falsh_value_str);
	}
}
//MSH_CMD_EXPORT_ALIAS(flash_data_read_test, flash_r,...);
//MSH_CMD_EXPORT_ALIAS(flash_data_write_test, flash_w,...);

int Cal(int argc, char **argv)
{
	if (argc == 4 )
	{
		if (!strcmp(argv[1], "setK"))
		{
			Cal_SetK(atoi(argv[2]),atof(argv[3]));
			AD7124_TRACE("setK ok\n");
			return RT_EOK	; 				
		}
		if (!strcmp(argv[1], "setC"))
		{
			Cal_SetC(atoi(argv[2]),atof(argv[3]));
			AD7124_TRACE("setC ok\n");
			return RT_EOK	; 				
		}
	}
	return RT_EOK	; 	
}
//MSH_CMD_EXPORT(Cal,...);

#include "bsp_eeprom_24xx.h"

DMM_KCTable DMM_CAL_Table;//=

void DMM_CAL_KCTable_RamToEeprom(void)
{
	DMM_CAL_Table.Gain[0]=KTable[ DMM_FUNC_DCV_10V];
	DMM_CAL_Table.Gain[1]=KTable[ DMM_FUNC_DCI_100mA];
	DMM_CAL_Table.Gain[2]=KTable[ DMM_FUNC_DCI_1A];
	DMM_CAL_Table.Gain[3]=KTable[ DMM_FUNC_OHM_10R_4W];
	
	DMM_CAL_Table.Offset[0]=CTable[ DMM_FUNC_DCV_10V];
	DMM_CAL_Table.Offset[1]=CTable[ DMM_FUNC_DCI_100mA];
	DMM_CAL_Table.Offset[2]=CTable[ DMM_FUNC_DCI_1A];
	DMM_CAL_Table.Offset[3]=CTable[ DMM_FUNC_OHM_10R_4W];
	at24cxx.write(at24c256,54*64+1,(uint8_t *)&DMM_CAL_Table,sizeof(DMM_CAL_Table));	
}
void DMM_CAL_KCTable_EepromToRam(void)
{
	unsigned int i = 0;
	
	at24cxx.read(at24c256,54*64+1,(uint8_t *)&DMM_CAL_Table,sizeof(DMM_CAL_Table));	
	
	KTable[ DMM_FUNC_DCV_10V]		=DMM_CAL_Table.Gain[0];
	KTable[ DMM_FUNC_DCI_100mA]	=DMM_CAL_Table.Gain[1];
	KTable[ DMM_FUNC_DCI_1A]		=DMM_CAL_Table.Gain[2];
	KTable[ DMM_FUNC_OHM_10R_4W]=DMM_CAL_Table.Gain[3];
	
	CTable[ DMM_FUNC_DCV_10V]		=DMM_CAL_Table.Offset[0];
	CTable[ DMM_FUNC_DCI_100mA]	=DMM_CAL_Table.Offset[1];
	CTable[ DMM_FUNC_DCI_1A]		=DMM_CAL_Table.Offset[2];
	CTable[ DMM_FUNC_OHM_10R_4W]=DMM_CAL_Table.Offset[3];
}
/*
DMM SetGain BySignal DCI_1A 285		电流单位校准285在0~500ma采集都非常准

*/
void SetGainViaInputSignal( int _Func ,	int milliValue)
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		DMM_CAL_Table.filterTimes=20;
		ADCChannelConvert(_Func);
		//ADCChannelConfigToRam

#if defined (USING_GAIN_AD_VALUE)	
		int32_t getAdValue=AD7124_ReadADAverValue(ad7124_handler);
		
		//AD7124_TRACE("getAdValue=%d\n",getAdValue);
		
		if( getAdValue!=0)
		KTable[_Func]=(float)(milliValue*1000.0/getAdValue);
#else		
		double getValue=AD7124_ReadAverData(ad7124_handler);
		if( getValue!=0)
		KTable[_Func]=milliValue*1000/getValue;
#endif
		DMM_CAL_Table.filterTimes=2;
		//DMM_CAL_KCTable_RamToEeprom();
	}
	
}
void SetOffsetViaInputSignal( int _Func ,	int milliValue)
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		ADCChannelConvert(_Func);
		//CTable[_Func]=milliValue;
		//CTable[_Func]=milliValue*1000/AD7124_ReadAverData(ad7124_handler);
		//DMM_CAL_KCTable_RamToEeprom();
	}
}
void printGainOffset( void )
{
	uint8_t i;
	uint8_t kc_value_str[10];
	for( i=0; i < DMM_FUNC_COUNT ; ++i )
	{
		sprintf(kc_value_str,"%lf",KTable[i]);							
		AD7124_TRACE("Gain[%d]=%s\n",i,kc_value_str);
		sprintf(kc_value_str,"%lf",CTable[i]);							
		AD7124_TRACE("Offset[%d]=%s\n",i,kc_value_str);
	}	
}

//MSH_CMD_EXPORT_ALIAS(DMM_CAL_KCTable_RamToEeprom,DMM_CAL_SAVE,...);
//MSH_CMD_EXPORT_ALIAS(DMM_CAL_KCTable_EepromToRam,DMM_CAL_READ,...);


#endif

