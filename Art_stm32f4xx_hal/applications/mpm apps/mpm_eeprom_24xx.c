#include "mpm.h"

#define EEPROM_PAGE_BYTES						64					//!< EEPROM 每页长度
#define PARAM_VER										21

#define EEPROM_PARAM_PAGE_ADDR			0			
#define EEPROM_PARAM_PAGE_LEN				10

#define EEPROM_VAR_PAGE_ADDR				10			
#define EEPROM_VAR_PAGE_LEN					10

#define EEPROM_ROM_PAGE_ADDR				20					//!< EEPROM ROM等其他命令存取空间起始地址
#define EEPROM_ROM_PAGE_LEN					35

#define EEPROM_INFO_PAGE_ADDR				55					//!< EEPROM 存取操作空间起始地址
#define EEPROM_INFO_PAGE_LEN				200					//!< EEPROM 设置的可用页数


at24cxx_t at24c256=
{
	{0},
	{BOARD_I2C_SCL_PIN,BOARD_I2C_SDA_PIN},	
	0xA0,	
};

/**********************************************************************************************************************/
//static void LoadParamFromEeprom(void);

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
void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;		//C24LC256;
	at24cxx.init(at24c256);
	
	if (at24cxx.check(at24c256) == 0)
	{
		//rt_kprintf("iic device at24c256 init [error]\n");
	}
	else 
	{
		rt_kprintf("iic device at24c256 [found] addr=%d\n",at24c256.devAddress);
		//LoadParamFromEeprom();
		    rt_pin_mode(EEPROM_WP_PIN, PIN_MODE_OUTPUT);
        rt_pin_write(EEPROM_WP_PIN, PIN_LOW);
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
/******************************************finsh***********************************************************************/
INIT_APP_EXPORT(at24cxx_hw_init);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);
/*********************************************************************************************************************/

/*********************************************************************************************************************/
//								EEPROM FINSH TEST
/*********************************************************************************************************************/
#if 1
void eeW(u16 address,u8 *data)
{
	u16 length=strlen((char*)data);
	if((address+length ) < ( at24c256.eeprom.maxAddress + 2) )
	//if((address+length ) <= (dev.eeprom.maxAddress+1 ))
	{
		s16 SizeLeft	 = length;						// bytes left to write
		s16 OffsetData = 0;									// current offset in data pointer
		s16 OffsetPage = 0;									// current offset in page
		s16 SizeNext 	 = 0;									// next n bytes to write	
		
		while(SizeLeft>0)									  //还有数据要写
		{	
			//页偏移地址 	
			OffsetPage	 = address % at24c256.eeprom.pageSize;		
			SizeNext 		 = min(min(SizeLeft, at24c256.eeprom.pageSize), at24c256.eeprom.pageSize - OffsetPage);		
			//从页偏移地址开始写入下次可写的长度
			if (at24c256.eeprom.type < AT24C04)
			{
				SoftI2c.writes(at24c256.pins, 1, at24c256.devAddress, address,(data+OffsetData),SizeNext);
			}
			else if (at24c256.eeprom.type > AT24C02 && at24c256.eeprom.type < AT24C32)
			{
				SoftI2c.writes(at24c256.pins, 3, at24c256.devAddress, address,(data+OffsetData),SizeNext);
			}
			else 
			{
				SoftI2c.writes(at24c256.pins, 2, at24c256.devAddress, address,(data+OffsetData),SizeNext);
			}
			drv_mdelay(5);
			SizeLeft-=SizeNext;							//剩下的字节数
			OffsetData+=SizeNext;						//下次需要写入的DATA位置
			address+=SizeNext;							//下次需要写入的ADDR位置	
		}
	}	
}
void eeR( u16 address,u16 length)
{
	u8 data[1000];
	if(address+length < (at24c256.eeprom.maxAddress + 2)	)
	{
		if (at24c256.eeprom.type < AT24C04)
		{
			SoftI2c.reads(at24c256.pins, 1, at24c256.devAddress, address ,data,length);
		}
		else if (at24c256.eeprom.type > AT24C02 && at24c256.eeprom.type < AT24C32)
		{
			SoftI2c.reads(at24c256.pins, 3, at24c256.devAddress, address ,data,length);
		}
		else 
		{
			SoftI2c.reads(at24c256.pins, 2, at24c256.devAddress, address ,data,length);
		}	
	}
	data[length]='\0';
	rt_kprintf("read data:\n");
	for(uint8_t i=0; i<length; i++)
	{
		rt_kprintf("%d ",data[i]);
	}
	rt_kprintf("\n");
}



#endif

int eeprom(int argc, char **argv)
{
	if(argc==1)
	{
		   rt_kprintf("eeprom r <addr_read> <data_len>\n");
			 rt_kprintf("eeprom w <addr_write> <data to write>\n");
		
	}
	if(argc==4)
	{
		if (!strcmp(argv[1], "r")) 
		{
			eeR(atoi(argv[2]),atoi(argv[3]));
			return RT_EOK;
		}
		if (!strcmp(argv[1], "w")) 
		{
			eeW(atoi(argv[2]),argv[3]);
			return RT_EOK;
		}
	}	
	return RT_ERROR;
	
}
MSH_CMD_EXPORT(eeprom,...);

//



