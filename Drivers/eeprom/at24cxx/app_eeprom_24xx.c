
#include "app_eeprom_24xx.h"
#include "bsp_mcu_delay.h"
#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


//at24cxx_t at24c256=
//{
//	{0},
//	{PC_14,PC_15},	
//	0xA2,	
//};

/**********************************************************************************************************************/
//static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
//static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

/************************************************************************************************************************************************************/
uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	uint8_t _len=strlen(_string);

	//保存的长度为<=EEPROM_PAGE_BYTES										点位<=EEPROM_INFO_PAGE_LEN
	if(_len<(EEPROM_PAGE_BYTES+1)	&& 0< _pageNumber &&  _pageNumber < (EEPROM_INFO_PAGE_LEN+1)	)
	{
		at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+(_pageNumber-1))*EEPROM_PAGE_BYTES,(uint8_t *)_string,_len);
		
		if(_len!=EEPROM_PAGE_BYTES)	
		{
			//保证读取字符串的结束标志
			at24cxx.write(at24c256,(EEPROM_INFO_PAGE_ADDR+(_pageNumber-1))*EEPROM_PAGE_BYTES+_len,(uint8_t *)'\0',1);	
		}
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}

uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber , char *_string)
{
	if(0< _pageNumber &&  _pageNumber <( EEPROM_INFO_PAGE_LEN+1)	)
	{
		uint8_t i;
		
		at24cxx.read(at24c256 , (EEPROM_INFO_PAGE_ADDR+(_pageNumber-1))*EEPROM_PAGE_BYTES, (uint8_t *)_string, 64);
		
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
	at24c256.eeprom=C24LC128;		//C24LC256;
	
	at24cxx.init(at24c256);
	
	if (at24cxx.check(at24c256) == 0)
	{
		DBG_TRACE("iic device at24c256 init [error]\n");
	}
	else 
	{
		rt_kprintf("iic device at24c128 found\n");
		DBG_TRACE("iic device at24c256 [found] addr=%d\n",at24c256.devAddress);
	}	
}
/*******命令顺序格式一 自定义格式***********/
#if 1		
void printf_cmdList_eeprom(void)
{
	rt_kprintf("\nEEPROM Usage: \n");
	rt_kprintf("rom[]----------------read roms\n");
	rt_kprintf("rom[][]--------------write roms\n");
	rt_kprintf("save info xx yyyyy---write datas\n");
	rt_kprintf("read info xx---------read datas\n");
	rt_kprintf("savedata: save data to page 1~%d\n",EEPROM_INFO_PAGE_LEN);
	rt_kprintf("readdata: read data from page 1~%d\n",EEPROM_INFO_PAGE_LEN);
}
uint8_t  command_fsn(char *Commands)
{
	char fsn[64];
	DBG_TRACE("%s\n",Commands);
	
	if(strcmp("fsn",		Commands)==0)			
	{	
		at24cxx.read(at24c256 , EEPROM_FSN_PAGE_ADDR*EEPROM_PAGE_BYTES, (uint8_t *)fsn, 64);
		for(uint8_t i=0;i<EEPROM_PAGE_BYTES;i++)	
		{
			if(fsn[i]>127) { fsn[i]='\0';break;}
		}
//		if(!ReadPageStringFromEEPROM(EEPROM_FSN_PAGE_ADDR,fsn))
		rt_kprintf("%s<%.64s>\n",Commands,fsn);
		return REPLY_OK;	
	}
	if(	(Commands[0]=='s')	&&	(Commands[1]=='n')	&&	(Commands[2]==':')	)
	{
//		if(!ReadPageStringFromEEPROM(EEPROM_FSN_PAGE_ADDR,fsn))
		at24cxx.write(at24c256, EEPROM_FSN_PAGE_ADDR*EEPROM_PAGE_BYTES, (uint8_t *) (&Commands[3]), strlen(&Commands[3])+1);
		rt_kprintf("%s<OK>\n",Commands);
		return REPLY_OK;
	}

	if(strncmp("fsn[",	Commands,4)==0)	
	{	
		uint8_t i;
		for(i=0;i<64;i++)
		{
			if(Commands[i+4]==']' && Commands[i+5]=='\0')
			{
				rt_kprintf("%s",Commands);Commands[i+4]='\0';
				
				DBG_TRACE("Commands[i+4]=%d\n",Commands[i+4]);
				break ;
			}
		}
		if(	i>0	&& i<64	&& Commands[i+4]=='\0'&& Commands[i+5]=='\0')			
		{		
			at24cxx.write(at24c256, EEPROM_FSN_PAGE_ADDR*EEPROM_PAGE_BYTES, (uint8_t *)(&Commands[4]), strlen(&Commands[4]));
			
			DBG_TRACE("strlen(&Commands[4])+1=%d\n",strlen(&Commands[4])+1);
			
			rt_kprintf("<OK>\n",Commands);
		}
		else rt_kprintf("fsn must be 1~64Byte\n");
		return REPLY_OK;		
	}	
	else return REPLY_INVALID_CMD;
}
uint8_t  command_rom(char *Commands)
{
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		f32 DataSet;		
		u8 DataNum=0;
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='[' && ( DataNum>0 && DataNum < (EEPROM_ROM_PAGE_LEN+1) ))
		{
			s=NULL;
			DataSet=strtof(p+2, &s);			
			if(*(s)==']'&& *(s+1)=='\0' )			
			{																	
				at24cxx.writeF32(at24c256,(EEPROM_ROM_PAGE_ADDR)*64+(DataNum-1)*4,DataSet);
				rt_kprintf("%s<OK>\n",Commands);
				
			}
			return REPLY_OK; 
		}
		
	}
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char strF32[10]="";
		char *p = NULL;
		char *s = &Commands[4];	
		u16 DataNum=0;	
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& (DataNum>0 && DataNum<(EEPROM_ROM_PAGE_LEN+1)))
		{	
			sprintf(strF32,"%.3f",at24cxx.readF32(at24c256 , (EEPROM_ROM_PAGE_ADDR)*64+(DataNum-1)*4));
			rt_kprintf("%s<%s>\n",Commands,strF32);
			
		}
		return REPLY_OK;	
	}	
	
	
	return REPLY_INVALID_CMD;
	
}
//
uint8_t  command_infoPage(char *Commands)
{
	if( strncmp(Commands,"save info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u32 PageNum=0;	
		PageNum=strtol(s, &p, 10);	

		if(	*p==' ' && PageNum>0 && PageNum < EEPROM_INFO_PAGE_LEN+1	)			
		{		
			if(!WritePageStringToEEPROM(PageNum, p+1))	rt_kprintf("%s OK\n",Commands);
			else  rt_kprintf("pageNum must be 1~%d,data length must be 1~%d\n",EEPROM_INFO_PAGE_LEN,EEPROM_PAGE_BYTES);							
		}
		return REPLY_OK;		 
	}
	if( strncmp(Commands,"read info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u8 i;
		u32 PageNum=0;	
		char read_info[64];
		PageNum=strtol(s, &p, 10);	
		
		if(	*p=='\0' && PageNum>0 && PageNum <EEPROM_INFO_PAGE_LEN+1	)			//前100页用于FSN+ROM 
		{
			if(!ReadPageStringFromEEPROM(PageNum,read_info))
			{
				rt_kprintf("%s %.64s\n",Commands,read_info);
			}
		}
		else rt_kprintf("pageNum must be 1~%d\n",EEPROM_INFO_PAGE_LEN);
		return REPLY_OK;		
	}
	else return REPLY_INVALID_CMD;
}

uint8_t Command_analysis_eeprom(char *string)
{
	if( !strcmp(string,"eeprom help") )		 				{ printf_cmdList_eeprom();	return REPLY_OK;	}
	else if(command_fsn(string)==REPLY_OK)				{return REPLY_OK;	}
	else if(command_rom(string)==REPLY_OK)				{return REPLY_OK;	}
	else if(command_infoPage(string)==REPLY_OK)		{return REPLY_OK;	}

/*	
	else if( !strncmp(string,"savedata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		PageNum=strtol(s, &p, 10);	
		if( (*p==' ')	&& (!WritePageStringToEEPROM(PageNum,(char *)p+1)))
		{
			rt_kprintf("\nsavedata[%d] ok\n",PageNum);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			rt_kprintf("\npageNum must be 1~200,data length must be 1~64\n");
		}
	}
	else if( !strncmp(string,"readdata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		uint8_t read_info[64];
		PageNum=strtol(s, &p, 10);	
		if( (*p=='\0')	&& (!ReadPageStringFromEEPROM(PageNum,(char *)read_info)))
		{
			rt_kprintf("\nreaddata[%d]=%.64s\n",PageNum,read_info);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			rt_kprintf("\npageNum must be 1~200\n");
		}
	}
*/
 return REPLY_INVALID_CMD;
}







#endif

/*******命令顺序格式二 直接调用MSH护着FINSH****MSH_CMD_EXPORT********FINSH_FUNCTION_EXPORT***********/
#if 1		
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

uint8_t at24cxx_msh1(char *string)
{
	uint8_t result = REPLY_OK;
	
	if( !strcmp(string,"eeprom help") )		 { printf_cmdList_eeprom();	}
	
	else if(!command_rom(string))			{;}
	else if(!command_infoPage(string))	{;}
		
	else if( !strncmp(string,"savedata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		PageNum=strtol(s, &p, 10);	
		if( (*p==' ')	&& (!WritePageStringToEEPROM(PageNum,(char *)p+1)))
		{
			rt_kprintf("\nsavedata[%d] ok\n",PageNum);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			rt_kprintf("\npageNum must be 1~200,data length must be 1~64\n");
		}
	}
	else if( !strncmp(string,"readdata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		uint8_t read_info[64];
		PageNum=strtol(s, &p, 10);	
		if( (*p=='\0')	&& (!ReadPageStringFromEEPROM(PageNum,(char *)read_info)))
		{
			rt_kprintf("\nreaddata[%d]=%.64s\n",PageNum,read_info);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			rt_kprintf("\npageNum must be 1~200\n");
		}
	}
}

MSH_CMD_EXPORT(savedata, Save data-without spaces to page);
MSH_CMD_EXPORT(readdata, Read data from page);
#endif
//
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
/*任意位置 最长200字节 由串口缓存决定*/
void WriteDataToEEPROM(u16 address,u8 *data)
{
	u16 length=strlen((char*)data);
	
	u16 addTemp=address;
	if(length<201 && length>0 && (address+length ) < ( at24c256.eeprom.maxAddress + 2) )
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
/*任意位置 最长200字节*/
void ReadDataFromEEPROM( u16 address,u16 length)
{
	u8 data[200]="";
	if(length<201 && length>0 && address+length < (at24c256.eeprom.maxAddress + 2)	)
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

FINSH_FUNCTION_EXPORT_ALIAS(WriteDataToEEPROM,WriteEEPROM,...);
FINSH_FUNCTION_EXPORT_ALIAS(ReadDataFromEEPROM,ReadEEPROM,...);

