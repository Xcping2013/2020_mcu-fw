
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"


//STM32F103VFT6 		ROM=768KB		RAM=96KB
//AT24CM01-SSHD-T		128KB				SCL=PB7		SDA=PB6	A1=A2=L
//W25Q32JVSSIQ			SPI1				CS=PA2		PA5 PA6 PA7
//LED=PE2

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

char version_string[]="FW:INC-MBEEPROM-03 VER01";
/*

*/
at24cxx_t at24c256=
{
	{0},
	{PB_13,PB_12},	
	0xA0,	
};

at24cxx_t at24cm01=
{
	{0},
	{PB_7,PB_6},	
	0xA0,	
};

uint8_t LED_PIN=PE_2;//PE_13;

//AT24C128	128Kb (16K x 8)  pageSize=64Byte blockNumbers=16K/64=256Pages
/****************************************MAIN---LED Blink*******************************************************/
int main(void)
{
	rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED_PIN, PIN_HIGH);				//灭
	while (1)
	{
			rt_pin_write(LED_PIN, PIN_HIGH);
			rt_thread_mdelay(800);
			rt_pin_write(LED_PIN, PIN_LOW);
			rt_thread_mdelay(800);
	}
	return RT_EOK;
}

/****************************************mbrelayadp_hw_init**********************************************************/
int mbeeprom_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\n%s ROM[%d] RAM[%d] build at %s %s\n\n", version_string,STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE,  __TIME__, __DATE__);
			
	at24cm01.eeprom=C24LCM01;
	
	at24cxx.init(at24cm01);
	
	if (at24cxx.check(at24cm01) == 0)
	{
		DBG_TRACE("iic device at24cm01 init [error]\n");
	}
	else 
	{
		rt_kprintf("iic device at24cm01 [found]\n");
		DBG_TRACE("iic device at24cm01 [found] addr=%d\n",at24cm01.devAddress);
	}	

	rt_kprintf("-------------------------------\n");
	rt_kprintf("you can type help to list commands and all commands should end with \\r\\n\n");
	rt_kprintf("-------------------------------\n>>");
	
  return 0;
}


//page0:SN
//page1~255 Data
//不能使用RTT RTT串口组件初始化太多内存了
uint8_t at24cxx_msh(char *string)
{
	//uint8_t result = REPLY_OK;
		
	if( !strncmp(string,"EEPROM WritePage ",17) )//如果RT_CONSOLEBUF_SIZE<串口输入的字符,打字的字符将会出现异常，浪费在RTT组件上的时间太多了
	{
		char *p = NULL;
		char *s = &string[17];	
		
		uint32_t PageNum=strtol(s, &p, 10);	
		
		if(PageNum<512 && *p==' ')//	&& (!WritePageStringToEEPROM(PageNum,(char *)p+1)))
		{
			//uint16_t _len=strlen((char *)p+1);
			uint16_t _len=strlen((char *)(p+1));
			
			rt_kprintf("%s\n",p+1);
			
			if( 0<_len && _len<257)
			{
				at24cxx.write(at24cm01,PageNum*256,(uint8_t *)(p+1),_len);
				
				if(_len!=256)
				{
					at24cxx.write(at24cm01,PageNum*256+_len,(uint8_t *)'\0',1);	
				}
				//rt_kprintf("ok\n");
				rt_kprintf("EEPROM WritePage %d ok\n",PageNum);
				
			}	
		}		
		else 
		{		
			rt_kprintf("pageNum must be 0~512,data length must be 1~256\n");
		}
		return REPLY_OK;
	}
	else if( !strncmp(string,"EEPROM ReadPage ",16) ) 
	{
		char *p = NULL;
		char *s = &string[16];	
		uint32_t PageNum=0;	
		uint8_t read_info[257];
		memset(read_info, '\0', 257);
		
		PageNum=strtol(s, &p, 10);	
		
		if(PageNum<512 && *p=='\0')//	&& (!ReadPageStringFromEEPROM(PageNum,(char *)read_info)))
		{
			uint16_t i;
		
			at24cxx.read(at24cm01 , PageNum*256, (uint8_t *)read_info, 256);
		
			//防止读取到不相干的错误数据
			for(i=0;i<256;i++)	
			{
				if(read_info[i]>127 || read_info[i]<33) { read_info[i]='\0';break;}
			}
			rt_kprintf("=%s\n",read_info);
//			printf("=%s\n",read_info);
			//rt_kprintf("EEPROM ReadPage %d=%.64s\n",PageNum,read_info);
			
		}	
		else
		{
			rt_kprintf("pageNum must be 0~512\n");
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
				at24cxx.write(at24cm01,eeAddr,(uint8_t *)(char *)p+1,_len);
				
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
				at24cxx.read(at24cm01 , eeAddr, (uint8_t *)read_info, _len);
				
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
				//at24cxx.writeU8(at24cm01,0,_len);
				
				at24cxx.write(at24cm01,0,(uint8_t *)"SN:",3);	

				at24cxx.write(at24cm01,3,(uint8_t *)&string[3],_len);
				

				//at24cxx.write(at24cm01,3+_len,(uint8_t *)'0',1);	
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
//		FsnLen=at24cxx.readU8(at24cm01 , 0);
				
//		if(0<FsnLen && FsnLen<20)
		{
			at24cxx.read(at24cm01 , 0, (uint8_t *)ReadFsn, 14);//FsnLen);
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
	if((address+length ) < ( at24cm01.eeprom.maxAddress + 2) )
	//if((address+length ) <= (dev.eeprom.maxAddress+1 ))
	{
		s16 SizeLeft	 = length;						// bytes left to write
		s16 OffsetData = 0;									// current offset in data pointer
		s16 OffsetPage = 0;									// current offset in page
		s16 SizeNext 	 = 0;									// next n bytes to write	
		
		while(SizeLeft>0)									  //还有数据要写
		{	
			//页偏移地址 	
			OffsetPage	 = address % at24cm01.eeprom.pageSize;		
			SizeNext 		 = min(min(SizeLeft, at24cm01.eeprom.pageSize), at24cm01.eeprom.pageSize - OffsetPage);		
			//从页偏移地址开始写入下次可写的长度
			if (at24cm01.eeprom.type < AT24C04)
			{
				SoftI2c.writes(at24cm01.pins, 1, at24cm01.devAddress, address,(data+OffsetData),SizeNext);
			}
			else if (at24cm01.eeprom.type > AT24C02 && at24cm01.eeprom.type < AT24C32)
			{
				SoftI2c.writes(at24cm01.pins, 3, at24cm01.devAddress, address,(data+OffsetData),SizeNext);
			}
			else 
			{
				SoftI2c.writes(at24cm01.pins, 2, at24cm01.devAddress, address,(data+OffsetData),SizeNext);
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
	if(length>0 && address+length < (at24cm01.eeprom.maxAddress + 2)	)
	{
		if (at24cm01.eeprom.type < AT24C04)
		{
			SoftI2c.reads(at24cm01.pins, 1, at24cm01.devAddress, address ,data,length);
		}
		else if (at24cm01.eeprom.type > AT24C02 && at24cm01.eeprom.type < AT24C32)
		{
			SoftI2c.reads(at24cm01.pins, 3, at24cm01.devAddress, address ,data,length);
		}
		else 
		{
			SoftI2c.reads(at24cm01.pins, 2, at24cm01.devAddress, address ,data,length);
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




