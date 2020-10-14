
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

char version_string[]="ver01";
/*

*/
at24cxx_t at24c256=
{
	{0},
	{PB_13,PB_12},	
	0xA0,	
};

uint8_t LED_PIN=PA_13;

//AT24C128	128Kb (16K x 8)  pageSize=64Byte blockNumbers=16K/64=256Pages
/****************************************MAIN---LED Blink*******************************************************/
int main(void)
{
	rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED_PIN, PIN_HIGH);				//灭
	while (1)
	{
			rt_pin_write(LED_PIN, PIN_HIGH);
			rt_thread_mdelay(1000);
			rt_pin_write(LED_PIN, PIN_LOW);
			rt_thread_mdelay(500);
	}
	return RT_EOK;
}

/****************************************mbrelayadp_hw_init**********************************************************/
int mbeeprom_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\nFW:MBEEPROM-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
			
	__HAL_AFIO_REMAP_SWJ_DISABLE();
		
	at24cxx_hw_init();
	rt_kprintf("-------------------------------\n");
	rt_kprintf("you can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n>>");
	
  return 0;
}

uint8_t at24cxx_msh(char *string)
{
	//uint8_t result = REPLY_OK;
		
	if( !strncmp(string,"EEPROM WritePage ",17) )
	{
		char *p = NULL;
		char *s = &string[17];	
		
		uint32_t PageNum=strtol(s, &p, 10);	
		
		if(PageNum<256 && *p==' ')//	&& (!WritePageStringToEEPROM(PageNum,(char *)p+1)))
		{
			uint8_t _len=strlen((char *)p+1);
			
			if( 0<_len && _len< 65)
			{
				at24cxx.write(at24c256,PageNum*64,(uint8_t *)(char *)p+1,_len);
				
				if(_len!=64)
				{
					at24cxx.write(at24c256,PageNum*64+_len,(uint8_t *)'\0',1);	
				}
				rt_kprintf("ok\n",PageNum);
				//rt_kprintf("EEPROM WritePage %d ok\n",PageNum);
				
			}	
		}		
		else 
		{		
			rt_kprintf("pageNum must be 0~255,data length must be 1~64\n");
		}
		return REPLY_OK;
	}
	else if( !strncmp(string,"EEPROM ReadPage ",16) ) 
	{
		char *p = NULL;
		char *s = &string[16];	
		uint32_t PageNum=0;	
		uint8_t read_info[64];
		PageNum=strtol(s, &p, 10);	
		if(PageNum<256 && *p=='\0')//	&& (!ReadPageStringFromEEPROM(PageNum,(char *)read_info)))
		{
			uint8_t i;
		
			at24cxx.read(at24c256 , PageNum*64, (uint8_t *)read_info, 64);
		
			//防止读取到不相干的错误数据
			for(i=0;i<64;i++)	
			{
				if(read_info[i]>127) { read_info[i]='\0';break;}
			}
			rt_kprintf("=%.64s\n",read_info);
			//rt_kprintf("EEPROM ReadPage %d=%.64s\n",PageNum,read_info);
			
		}	
		else
		{
			rt_kprintf("pageNum must be 0~255\n");
		}
		return REPLY_OK;
	}


	else if( !strncmp(string,"EEPROM Write ",13) )
	{
		char *p = NULL;
		char *s = &string[13];	
		
		uint32_t eeAddr=strtol(s, &p, 10);	
		
		if(eeAddr<(16*1024) && *p==' ')//	&& (!WritePageStringToEEPROM(eeAddr,(char *)p+1)))
		{
			uint16_t _len=strlen((char *)p+1);
			
			if( 0<_len && ((eeAddr+_len)< (16*1024)))
			{
				at24cxx.write(at24c256,eeAddr,(uint8_t *)(char *)p+1,_len);
				
				rt_kprintf("ok\n",eeAddr);			
			}	
		}		
		else 
		{		
			rt_kprintf("eeAddr must be 0~16383\n");
		}
		return REPLY_OK;
	}
	else if( !strncmp(string,"EEPROM Read ",12) ) 
	{
		char *p = NULL;
		char *s = &string[12];	
		uint32_t eeAddr=0;	
		uint8_t read_info[1024];
		
		eeAddr=strtol(s, &p, 10);	
		
		if(eeAddr<(16*1024) && *p==' ')//	&& (!WritePageStringToEEPROM(eeAddr,(char *)p+1)))
		{
			s=NULL;
			
			uint16_t _len=strtol(p+1, &s, 10);	
			
			if( 0<_len && ((eeAddr+_len)< (16*1024))&& _len < 1000)
			{
				at24cxx.read(at24c256 , eeAddr, (uint8_t *)read_info, _len);
				
				read_info[_len]='\0';
				
				rt_kprintf("=%s\n",read_info);
						
			}	
		}	
		else
		{
			rt_kprintf("eeAddr must be 0~16383\n");
		}
		return REPLY_OK;
	}
	else if(!strncmp("SN:",	string,3))	
	{	
		//第一个字符页不保存字符长度，不然其他命令覆盖第一个字符后便不能使用SN命令读出
		//SN固定长度为11+SN:为14
			uint8_t _len=strlen(&string[3]);
			
//			uint8_t ulen=_len+'0';
			if(_len==11)
			//if( 0<_len && _len< 21)
			{
				//at24cxx.writeU8(at24c256,0,_len);
				
				at24cxx.write(at24c256,0,(uint8_t *)"SN:",3);	

				at24cxx.write(at24c256,3,(uint8_t *)&string[3],_len);
				

				//at24cxx.write(at24c256,3+_len,(uint8_t *)'0',1);	
				//页存取可以使用'0'页就是保存\0字符串结束符，在其他模式下读取不能保存次字符
				//不然读取包含次字符的数据都打印不出

				rt_kprintf("WriteSN to EEPROM[0~%d] ok\n",_len+3);
				
			}	
			else
			{
				rt_kprintf("strlen of SN must be 11Bytes\n");	
			}				
			return REPLY_OK;
	}	
	else if(!strcmp("SN",		string))			
	{		
		u8 ReadFsn[21];
		u8 FsnLen=0;
//		FsnLen=at24cxx.readU8(at24c256 , 0);
				
//		if(0<FsnLen && FsnLen<20)
		{
			at24cxx.read(at24c256 , 0, (uint8_t *)ReadFsn, 14);//FsnLen);
//			rt_kprintf("%s\n",ReadFsn);		
		}
		if(!strncmp("SN:",(char *)ReadFsn,3) || (!strncmp("sn:",(char *)ReadFsn,3))		)				
		{
			rt_kprintf("%.14s\n",ReadFsn);		
		}

		else
		{
			rt_kprintf("no sn saved,please write 11Bytes SN with command sn:\n");	
    }			
		return REPLY_OK;
	}
	return REPLY_INVALID_VALUE;
}



//分配下，每次最多读写200个字符串
//串口接收缓冲区 rtconfig.h 	#define RT_CONSOLEBUF_SIZE 256 -->2048
/*
//#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_CMD_SIZE 1000//160
#define RT_SERIAL_RB_BUFSZ 256
*/
void WriteDataToEEPROM(u16 address,u8 *data)
{
	u16 length=strlen((char*)data);
	
	u16 addTemp=address;
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
				rt_kprintf("Write EEPROM page\n",addTemp,addTemp+length-1);
			}
			drv_mdelay(5);
			SizeLeft-=SizeNext;							//剩下的字节数
			OffsetData+=SizeNext;						//下次需要写入的DATA位置
			address+=SizeNext;							//下次需要写入的ADDR位置	
		}
		rt_kprintf("Write EEPROM[%d~%d]<ok>\n",addTemp,addTemp+length-1);
	}	
}
void ReadDataFromEEPROM( u16 address,u16 length)
{
	u8 data[1024]="";
	if(length>0 && address+length < (at24c256.eeprom.maxAddress + 2)	)
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
	rt_kprintf("Read EEPROM[%d~%d]:%s\n",address,address+length-1,data);
//	for(uint8_t i=0; i<length; i++)
//	{
//		rt_kprintf("%d ",data[i]);
//	}
//	rt_kprintf("\n");
}


INIT_APP_EXPORT(mbeeprom_hw_init);

FINSH_FUNCTION_EXPORT_ALIAS(WriteDataToEEPROM,WriteEEPROM,...);
FINSH_FUNCTION_EXPORT_ALIAS(ReadDataFromEEPROM,ReadEEPROM,...);




