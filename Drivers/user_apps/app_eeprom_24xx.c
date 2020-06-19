#include "app_eeprom_24xx.h"
#include "inc_controller.h"
#include "user_msh.h"	
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
		g_tParam.ParamVer   = PARAM_VER;
		g_tParam.MasterBaudrate  = 115200;
		g_tParam.SlaveBaudrate   = 115200;
		g_tParam.cmdSet		= 1;
		if( g_tParam.Project_ID == BUTTON_ROAD || g_tParam.Project_ID == OQC_FLEX)
		{
			;
		}
		else 
		{
			g_tParam.cmdSet		= 2;
			g_tParam.Project_ID = BX88;
			//g_tParam.Project_ID = COMMON;	//不封装按键复位等动作
		}
		uint8_t i;
		for(i=0;i<N_O_MOTORS;i++)
		{
			g_tParam.tmc429_VMax[i] = 140;
			g_tParam.tmc429_AMax[i] = 1000;
			g_tParam.tmc429_PulseDiv[i]=5;
			g_tParam.tmc429_RampDiv[i]=8;
			g_tParam.speed_home[i]=380;
		}
		SaveParamToEeprom();						
	}	
}
static void printf_cmdList_param_save(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("Please input: ParamSave <var1> <var2>\n");
		rt_kprintf("ParamSave MasterBaudrate <baudrate>      -set the controller baudrate\n");	
		rt_kprintf("ParamSave SlaveBaudrate <baudrate>       -set the device baudrate\n");	
		rt_kprintf("ParamSave homing_speed <speed>           -set the project type\n");	

		rt_kprintf("ParamSave cmdSet <1|2>                   -set the project type\n");		
	
		rt_kprintf("ParamSave ProjectType <project>          -set the project type\n\n");	
	
//		rt_kprintf("ParamSave ProjectType button_online      -set controller fit to button online fixture\n");	
//		rt_kprintf("ParamSave ProjectType button_offline     -set controller fit to button offline fixture\n");	
//		
//		rt_kprintf("ParamSave ProjectType button_road        -set controller fit to button road fixture\n");
//		
//		rt_kprintf("ParamSave ProjectType OQC-Flex           -set controller fit to OQC-Flex fixture\n");
//		rt_kprintf("ParamSave ProjectType lidopen            -set controller fit to lidopen fixture\n");
//		
//		rt_kprintf("ParamSave ProjectType BUTTON-AXIS-1      -set controller fit to use 1 motor\n");
//		rt_kprintf("ParamSave ProjectType BUTTON-AXIS-2      -set controller fit to use 2 motor\n");
		rt_kprintf("ParamSave ProjectType common_use         -set controller fit to common_use\n");	
    rt_kprintf("ParamSave ProjectType BX88               -set controller fit to BX88\n");	
		
		rt_kprintf("ParamSave ProjectType Atlas_QT1|Sec_QT   \n");
		
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
		if (!strcmp(argv[1], "MasterBaudrate")) 				g_tParam.MasterBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "SlaveBaudrate")) 		g_tParam.SlaveBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "homing_speed")) 		{  g_tParam.speed_home[2]  = atoi(argv[2]);	g_tParam.speed_home[1]  = atoi(argv[2]);}
		
		else if (!strcmp(argv[1], "cmdset"))						
		{
			g_tParam.cmdSet  = atoi(argv[2]);		
			if(g_tParam.cmdSet==2) cmd_mshMode_exit();	
		}
			
		else if (!strcmp(argv[1], "ProjectType"))
		{
			if (!strcmp(argv[2], "button_online")) 				g_tParam.Project_ID  = BUTTON_ONLINE;	
			else if (!strcmp(argv[2], "button_offline")) 	g_tParam.Project_ID  = BUTTON_OFFLINE;	
			else if (!strcmp(argv[2], "OQC-Flex")) 		    g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "lidopen")) 				g_tParam.Project_ID  = LIDOPEN;	
			else if (!strcmp(argv[2], "button_road")) 		g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			else if (!strcmp(argv[2], "EOK-POWE-2AXIS")) 	g_tParam.Project_ID  = EOK_POWE_2AXIS;			//双轴
			
			else if (!strcmp(argv[2], "BUTTON-AXIS-1")) 		      g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "BUTTON-AXIS-2")) 		      g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			else if (!strcmp(argv[2], "AOI-AXIS-2")) 		      		g_tParam.Project_ID  = AOI_2AXIS;				//双轴
			
			else if (!strcmp(argv[2], "common_use")) 		      		g_tParam.Project_ID  = COMMON;					//双轴
			
			else if (!strcmp(argv[2], "OKOA_LED")) 		      			g_tParam.Project_ID  = OKOA_LED;
			else if (!strcmp(argv[2], "Atlas_QT1")) 		      		g_tParam.Project_ID  = Atlas_QT1;
			
			else if (!strcmp(argv[2], "Sec_QT")) 		      		g_tParam.Project_ID  = Sec_QT;
			
			else if (!strcmp(argv[2], "BX88")) 		      					
			{
				g_tParam.Project_ID  = BX88;						//双轴
				g_tParam.cmdSet=2;		
			}
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
/* user commands  */
static uint8_t  command_sv(char *Commands)
{
  if(strcmp("sv",		Commands)==0)			
	{		
		rt_kprintf("<sv:fw-mb1616dev6-rtt-128-04.hex>\n");
		return REPLY_OK;		
	}
	else return REPLY_INVALID_CMD;
}
//
static uint8_t  command_hv(char *Commands)
{
	if(strcmp("hv",		Commands)==0)			
	{		
		rt_kprintf("<hv:inc-mb1616dev6-04>\n");
		return REPLY_OK;	
	}
	else return REPLY_INVALID_CMD;
}
//空格的处理：多参数拼接
void save_data(u8 pageNum , char *data)
{
	u8 len=strlen(data);
	if(len<65	&& pageNum>0)	
	{
		at24cxx.write(at24c256,pageNum*64,(u8 *)data,len);
		if(len!=64)	at24cxx.write(at24c256,pageNum*64,(u8 *)'\0',1);
		EEPROM_TRACE("<ok>");
	}
	else EEPROM_TRACE("<pageNum must be 1~255,data len must be 1~64>");
}

void read_data(u8 pageNum)
{
	u8 read_info[64];
	u8 i;
	if(pageNum>0)	
	{
		at24cxx.read(at24c256 , pageNum*64, read_info, 64);
		for(i=0;i<64;i++)	
		{
			if(read_info[i]>127) { read_info[i]='\0';break;}
		}
		EEPROM_TRACE("<%.64s>",read_info);
	}
	else EEPROM_TRACE("<pageNum must be 1~255>");
}
uint8_t  command_fsn(char *Commands)
{
	if(strcmp("fsn",		Commands)==0)			
	{		
		uint8_t	i;
		uint8_t fsn[64];
		at24cxx.read(at24c256 , EEPROM_VAR_PAGE_ADDR*EEPROM_PAGE_BYTES, fsn, 64);
		//at24lc04_reads(0);
		for(i=0;i<64;i++)	
		{
			if(fsn[i]>127) { fsn[i]='\0';break;}
		}
		rt_kprintf("<%.64s>\n",fsn);
		return REPLY_OK;	
	}
	if(	(Commands[0]=='s')	&&	(Commands[1]=='n')	&&	(Commands[2]==':')	)
	{
		at24cxx.write(at24c256, EEPROM_VAR_PAGE_ADDR*EEPROM_PAGE_BYTES, (uint8_t *) (&Commands[3]), strlen(&Commands[3])+1);
		rt_kprintf("<OK>\n");
		return REPLY_OK;
	}

	else if(strncmp("fsn[",	Commands,4)==0)	
	{	
		uint8_t i;
		for(i=0;i<64;i++)
		{
			if(Commands[i+4]==']') {Commands[i+4]='\0';	break ;}
		}
		if(	i>0	&& i<64	&& Commands[i+4]=='\0'&& Commands[i+5]=='\0')			
		{		
			at24cxx.write(at24c256, EEPROM_VAR_PAGE_ADDR*EEPROM_PAGE_BYTES, (uint8_t *)(&Commands[4]), strlen(&Commands[4])+1);
			
			//rt_kprintf("strlen(&Commands[4])+1=%d\n",strlen(&Commands[4])+1);
			
			rt_kprintf("<OK>\n");
			return REPLY_OK;		
		}
		else return REPLY_INVALID_VALUE;	 
	}	
	else return REPLY_INVALID_CMD;
}
//
uint8_t  command_rom(char *Commands)
{
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u8 DataSet;		
		u8 DataNum=0;
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='[' && ( DataNum>0 && DataNum<EEPROM_ROM_PAGE_LEN ))
		{
			s=NULL;
			DataSet=strtod(p+2, &s);			
			if(*(s)==']'&& *(s+1)=='\0' )			
			{																	
				*(s)='\0';
				at24cxx.write(at24c256,(EEPROM_ROM_PAGE_ADDR+DataNum)*64,(uint8_t *)(p+2),strlen(p+2)+1);

				rt_kprintf("<OK>\n");
				return REPLY_OK;
			}
			else return REPLY_INVALID_VALUE;	 
		}	
	}
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u8 i;
		u8 rom[64];
		u16 DataNum=0;	
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& (DataNum>0 && DataNum<EEPROM_ROM_PAGE_LEN))
		{	
			at24cxx.read(at24c256 , (EEPROM_ROM_PAGE_ADDR+DataNum)*64, rom, 64);
			for(i=0;i<64;i++)	
			{
				if(rom[i]>127) { rom[i]='\0';break;}
			}
			rt_kprintf("<%.64s>\n",rom);

			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;	 
	}		
	else return REPLY_INVALID_CMD;
	
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
			//at24cxx.write(at24c256,(PageNum+100)*64,p+1,strlen(p+1)+1);
			at24cxx.write(at24c256,(PageNum+EEPROM_INFO_PAGE_ADDR)*64,(uint8_t *)(p+1),strlen(p+1)+1);
			rt_kprintf(" OK\n");

			return REPLY_OK;
		}
		else return REPLY_INVALID_VALUE;	 
	}
	if( strncmp(Commands,"read info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u8 i;
		u32 PageNum=0;	
		u8 read_info[64];
		PageNum=strtol(s, &p, 10);	
		
		if(	*p=='\0' && PageNum>0 && PageNum <EEPROM_INFO_PAGE_LEN+1	)			//前100页用于FSN+ROM 
		{
			//at24cxx.read(at24c256 , (PageNum+100)*64, read_info, 64);
			at24cxx.read(at24c256 , (PageNum+EEPROM_INFO_PAGE_ADDR)*64, read_info, 64);
			for(i=0;i<64;i++)	
			{
				if(read_info[i]>127) { read_info[i]='\0';break;}
			}
			rt_kprintf(" %.64s\n",read_info);

			return REPLY_OK;
		}	
		else return REPLY_INVALID_VALUE;	 
	}
	else return REPLY_INVALID_CMD;
}

void printf_cmdList_eeprom(void)
{
	rt_kprintf("\nEEPROM Usage: \n");
	rt_kprintf("sv-------------------get software version\n");
	rt_kprintf("hv-------------------get hardware version\n");	
	rt_kprintf("fsn------------------get fixture serial number\n");	
	rt_kprintf("fsn[]----------------set fixture serial number\n");	
	rt_kprintf("rom[]----------------read roms\n");
	rt_kprintf("rom[][]--------------write roms\n");
	rt_kprintf("save info xx yyyyy---write datas\n");
	rt_kprintf("read info xx---------read datas\n");
	rt_kprintf("savedata: save data to page 1~200\n");
	rt_kprintf("readdata: read data from page 1~200\n");
	rt_kprintf("\nParamSave Usage: \n");
//	rt_kprintf("ParamSave cmdset 1|2\n");
//	rt_kprintf("ParamSave ProjectType button_online\n");	
//	rt_kprintf("ParamSave ProjectType button_offline\n");		
//	rt_kprintf("ParamSave ProjectType button_road\n");		
//	rt_kprintf("ParamSave ProjectType OQC-Flex\n");
//	rt_kprintf("ParamSave ProjectType lidopen\n");		
//	rt_kprintf("ParamSave ProjectType BUTTON-AXIS-1\n");
//	rt_kprintf("ParamSave ProjectType BUTTON-AXIS-2\n");
//	rt_kprintf("ParamSave ProjectType AOI-AXIS-2\n");
	rt_kprintf("ParamSave ProjectType common_use\n");			
	rt_kprintf("ParamSave ProjectType BX88:\n");	
}

uint8_t Command_analysis_eeprom(char *string)
{
	uint8_t result = REPLY_OK;
	
	if( !strcmp(string,"eeprom help") )		 { printf_cmdList_eeprom();	}
	
	else if(!command_fsn(string))			{;}
	else if(!command_rom(string))			{;}
	else if(!command_infoPage(string))	{;}
	else if(!command_hv(string))			{;}
	else if(!command_sv(string))			{;}
		
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
	else if( !strcmp(string,"paramsave projecttype oqc-flex") ) 			
	{
		g_tParam.Project_ID  = OQC_FLEX;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
	}
	else if( !strcmp(string,"paramsave projecttype button-axis-1") ) 
	{
		g_tParam.Project_ID  = OQC_FLEX;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
	}
	else if( !strcmp(string,"paramsave projecttype button_road") ) 
	{
		g_tParam.Project_ID  = BUTTON_ROAD;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
	}
	else if( !strcmp(string,"paramsave projecttype button-axis-2") ) 
	{
		g_tParam.Project_ID  = BUTTON_ROAD;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
	}
/**********************************************USED**********************************************************/
	else if( !strcmp(string,"paramsave projecttype okoa") ) 
	{
		g_tParam.cmdSet  = 1;g_tParam.Project_ID  = COMMON;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}	
	else if( !strcmp(string,"paramsave projecttype okoa_led") ) 
	{
		g_tParam.cmdSet  = 1;g_tParam.Project_ID  = OKOA_LED;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}	
	else if( !strcmp(string,"paramsave projecttype atlas_qt1") ) 
	{
		g_tParam.cmdSet  = 1;g_tParam.Project_ID  = Atlas_QT1;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else if( !strcmp(string,"paramsave projecttype sec_qt") ) 
	{
		g_tParam.cmdSet  = 1;g_tParam.Project_ID  = Sec_QT;	SaveParamToEeprom();	rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
/*****************************************************************************************************************/	
	else if( !strcmp(string,"paramsave cmdset 1") || !strcmp(string,"paramsave cmdset 1") ) 
	{
		g_tParam.cmdSet  = 1;	SaveParamToEeprom(); cmd_mshMode_enter();
		rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		//SCB_AIRCR=SCB_RESET_VALUE;
	}	
		else if( !strcmp(string,"paramsave cmdset 2") || !strcmp(string,"paramsave cmdset 2") ) 
	{
		g_tParam.cmdSet  = 2;	SaveParamToEeprom(); cmd_mshMode_exit();
		rt_kprintf("\nsave parameter ok and controller start to load setting......ok\n");
		//SCB_AIRCR=SCB_RESET_VALUE;
	}	
	else result = REPLY_INVALID_CMD;
	return result;
}


/******************************************finsh***********************************************************************/
//INIT_APP_EXPORT(at24cxx_hw_init);
//MSH_CMD_EXPORT(savedata, save data to page 1~200);
//MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);
/*********************************************************************************************************************/

/*********************************************************************************************************************/
//								EEPROM FINSH TEST
/*********************************************************************************************************************/
#if 0
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

FINSH_FUNCTION_EXPORT(eeW,...);
FINSH_FUNCTION_EXPORT(eeR,...);

#endif


//



