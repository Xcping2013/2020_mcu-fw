
#include "inc_mbb372chg.h"

//#define	rt_kprintf	printf
//#define EEPROM_DEBUG
#ifdef 	EEPROM_DEBUG
#define EEPROM_TRACE         rt_kprintf
#else
#define EEPROM_TRACE(...)  				     
#endif
/**********************************************************************************************************************/
static void LoadParamFromEeprom(void);

/**********************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

static void printf_cmdList_param_save(void);

/************************************************************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	uint8_t _len=strlen(_string);

	if(_len<(EEPROM_PAGE_BYTES+1)	&& 0< _pageNumber &&  _pageNumber < (EEPROM_INFO_PAGE_LEN+1)	)
	{
		at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+_pageNumber)*64,(uint8_t *)_string,_len);	
		if(_len!=EEPROM_PAGE_BYTES)	
		{
			at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+_pageNumber)*64+_len,(uint8_t *)'\0',1);	
		}
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}

static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber , char *_string)
{
	if(0< _pageNumber &&  _pageNumber <( EEPROM_INFO_PAGE_LEN+1)	)
	{
		uint8_t i;
		at24cxx.read(at24c256 , (EEPROM_INFO_PAGE_ADDR+_pageNumber)*64, (uint8_t *)_string, 64);
		for(i=0;i<EEPROM_PAGE_BYTES;i++)	
		{
			if(_string[i]>127) { _string[i]='\0';break;}
		}
		return REPLY_OK;
	}
	else return REPLY_INVALID_VALUE;
}
/*
*********************************************************************************************************
*	函 数 名: SaveParamToEeprom
*	功能说明: 将全局变量g_tParam 写入到EEPROM
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SaveParamToEeprom(void)
{
	at24cxx.write(at24c256,EEPROM_PARAM_PAGE_ADDR,(uint8_t *)&g_tParam,sizeof(PARAM_T));
	
}
/*
*********************************************************************************************************
*	函 数 名: LoadParamFromEeprom
*	功能说明: 从EEPROM读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LoadParamFromEeprom(void)
{
	at24cxx.read(at24c256 , EEPROM_PARAM_PAGE_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));
	if (g_tParam.ParamVer != PARAM_VER)
	{
		g_tParam.ParamVer   	=  PARAM_VER;
		g_tParam.Project_ID  	=  B372_CHG;
		
		SaveParamToEeprom();						
	}	
}
static void printf_cmdList_param_save(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("Please input: ParamSave <var1> <var2>\n");
}
/*********************************************************************************************************************/
void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;		//C24LC256;
	at24cxx.init(at24c256);
	if (at24cxx.check(at24c256) == 0)
	{
		//EEPROM_TRACE("iic device at24c256 init [error]\n");
	}
	else 
	{
		rt_kprintf("iic device at24c256 [found] addr=%d\n",at24c256.devAddress);
		LoadParamFromEeprom();
	}	
}
int savedata(int argc, char **argv)
{
	uint8_t result = REPLY_OK;

	if (argc ==3 )
	{
		int pageNum=atoi(argv[1]);
		
		if(!WritePageStringToEEPROM(pageNum,argv[2]))
		{
			rt_kprintf("savedata[%d] ok\n",pageNum);
		}
		else 
		{
			rt_kprintf("pageNum must be 1~200,data length must be 1~64\n");
		}
	}
	else
	{
			//rt_kprintf("Please input: savedata <page number> <data to save>\n");
			  rt_kprintf("Usage: \n");
			  rt_kprintf("savedata <var1> <var2>\n");
			  rt_kprintf("    var1:1~200\n");
				rt_kprintf("    var2:string(max=64) without spaces\n");				
		result = REPLY_INVALID_CMD;
	}	
	return result;
}
//
int readdata(int argc, char **argv)
{
	uint8_t result = REPLY_OK;

	if (argc ==2)
	{
		int pageNum=atoi(argv[1]);
		char read_info[64];
		if(!ReadPageStringFromEEPROM(pageNum,read_info))
		{
			rt_kprintf("readdata[%d]=%.64s\n",pageNum,read_info);
		}
		else rt_kprintf("pageNum must be 1~200\n");
	}
	else
	{
			//rt_kprintf("Please input: readdata <page number>\n");
			  rt_kprintf("Usage: \n");
			  rt_kprintf("readdata <var>\n");
			  rt_kprintf("    var:1~200\n");			
		result = REPLY_INVALID_CMD;
	}	
	return result;
}
//
int ParamSave(int argc, char **argv)
{
	int result = REPLY_OK;

	if (argc == 3)
	{
		if (!strcmp(argv[1], "MasterBaudrate")) 	{;}			
		
		else if (!strcmp(argv[1], "ProjectType"))
		{
			if (!strcmp(argv[2], "B372_CHG")) 				g_tParam.Project_ID  = B372_CHG;	
			else 	result = REPLY_INVALID_VALUE;
		}
		else 	result = REPLY_WRONG_TYPE;
	}
	else result = REPLY_INVALID_CMD;
	
	if(result == REPLY_OK ) 
	{
		SaveParamToEeprom();	
		rt_kprintf("save parameter ok and controller start to load setting......");
		rt_kprintf("ok\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else
	{
		printf_cmdList_param_save();
	}
	return result;
}

