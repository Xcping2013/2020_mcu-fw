
#include "app_eeprom_24xx.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DEG_TRACE		rt_kprintf
	#else
	#define DEG_TRACE(...)		
#endif

#endif

//

//at24cxx_t at24c256=
//{
//	{0},
//	{PC_15,PC_14},	
//	0xA0,	
//};

//at24cxx_t at24c256=
//{
//	{0},
//	{PC_14,PC_15},	
//	0xA2,	
//};

/**********************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

/************************************************************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	uint8_t _len=strlen(_string);

	//保存的长度为<=EEPROM_PAGE_BYTES										点位<=EEPROM_INFO_PAGE_LEN
	if(_len<(EEPROM_PAGE_BYTES+1)	&& 0< _pageNumber &&  _pageNumber < (EEPROM_INFO_PAGE_LEN+1)	)
	{
		at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+_pageNumber)*EEPROM_PAGE_BYTES,(uint8_t *)_string,_len);
		
		if(_len!=EEPROM_PAGE_BYTES)	
		{
			//保证读取字符串的结束标志
			at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+_pageNumber)*EEPROM_PAGE_BYTES+_len,(uint8_t *)'\0',1);	
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
		
		at24cxx.read(at24c256 , (EEPROM_INFO_PAGE_ADDR+_pageNumber)*EEPROM_PAGE_BYTES, (uint8_t *)_string, 64);
		
		//防止读取到不相干的错误数据
		for(i=0;i<EEPROM_PAGE_BYTES;i++)	
		{
			if(_string[i]>127) { _string[i]='\0';break;}
		}
		return REPLY_OK;
	}
	else return REPLY_INVALID_VALUE;
}
/*********************************************************************************************************************/
void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;		//C24LC256;
	
	at24cxx.init(at24c256);
	
	if (at24cxx.check(at24c256) == 0)
	{
		DEG_TRACE("iic device at24c256 init [error]\n");
	}
	else 
	{
		DEG_TRACE("iic device at24c256 [found] addr=%d\n",at24c256.devAddress);
	}	
}
int savedata(int argc, char **argv)
{
	uint8_t result = REPLY_OK;

	if (argc ==3 )
	{
		uint8_t pageNum=atoi(argv[1]);
		
		if(!WritePageStringToEEPROM(pageNum,argv[2]))
		{
			rt_kprintf("savedata %d ok\n",pageNum);
		}
		else 
		{
			rt_kprintf("pageNum must be 1~%d,data length must be 1~%d\n",EEPROM_INFO_PAGE_LEN,EEPROM_PAGE_BYTES);
		}
	}
	else
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("savedata <var1> <var2>\n");
		rt_kprintf("    var1: page number --- 1~%d\n",EEPROM_INFO_PAGE_LEN);
		rt_kprintf("    var2: string to save --- Cannot contain spaces,max len=%d\n",EEPROM_PAGE_BYTES);			
		
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
		uint8_t pageNum=atoi(argv[1]);
		char read_info[64];
		if(!ReadPageStringFromEEPROM(pageNum,read_info))
		{
			rt_kprintf("readdata[%d]=%.64s\n",pageNum,read_info);
		}
		else rt_kprintf("pageNum must be 1~%d\n",EEPROM_INFO_PAGE_LEN);
	}
	else
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("readdata <var>\n");
	  rt_kprintf("    var:page number --- 1~%d\n",EEPROM_INFO_PAGE_LEN);			
		result = REPLY_INVALID_CMD;
	}	
	return result;
}

//
MSH_CMD_EXPORT(savedata, Save data to page);
MSH_CMD_EXPORT(readdata, Read data from page);
//


