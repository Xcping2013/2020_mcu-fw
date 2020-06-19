#include "app_eeprom_24xx.h"

//#define EEPROM_DEBUG

#ifdef 	EEPROM_DEBUG
#define EEPROM_TRACE         rt_kprintf
#else
#define EEPROM_TRACE(...)  				     
#endif

#define EEPROM_RW_PAGE_ADDR		55	
#define EEPROM_PAGE_LEN				200
#define EEPROM_PAGE_BUF				64
/**********************************************************************************************************************/

PARAM_T g_tParam;						

at24cxx_t at24c256=
{
	{0},
	{PE_14,PE_15},	//SDA SCL
	0xA2,	//1 0 1 0 0 0 1 0
};

/**********************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

static void SaveParamToEeprom(void);
static void LoadParamFromEeprom(void);

static void LoadSettingViaProID(void);
static void printf_cmdList_param_save(void);

/************************************************************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	uint8_t _len=strlen(_string);

	if(_len<(EEPROM_PAGE_BUF+1)	&& 0< _pageNumber &&  _pageNumber < (EEPROM_PAGE_LEN+1)	)
	{
		if(_len!=EEPROM_PAGE_BUF)	{_string[_len]='\0';_len=_len+1;}
		at24cxx.write(at24c256,(EEPROM_RW_PAGE_ADDR+_pageNumber)*64,(uint8_t *)_string,_len);	
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}

static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	if(0< _pageNumber &&  _pageNumber <( EEPROM_PAGE_LEN+1)	)
	{
		uint8_t i;
		at24cxx.read(at24c256 , (EEPROM_RW_PAGE_ADDR+_pageNumber)*64, _string, 64);
		for(i=0;i<EEPROM_PAGE_BUF;i++)	
		{
			if(_string[i]>127) { _string[i]='\0';break;}
		}
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}
/*
*********************************************************************************************************
*	函 数 名: SaveParamToEeprom
*	功能说明: 将全局变量g_tParam 写入到EEPROM
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SaveParamToEeprom(void)
{
	at24cxx.write(at24c256,PARAM_ADDR,(uint8_t *)&g_tParam,sizeof(PARAM_T));
}
/*
*********************************************************************************************************
*	函 数 名: LoadParamFromEeprom
*	功能说明: 从EEPROM读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void LoadParamFromEeprom(void)
{
	at24cxx.read(at24c256 , PARAM_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));

	EEPROM_TRACE("read data from eeprom when power up,fw ver:%d\n",	g_tParam.ParamVer);
	EEPROM_TRACE("g_tParam.Baudrate=%d\n",													g_tParam.SlaveBaudrate);
	EEPROM_TRACE("g_tParam.Project_ID=%d\n",												g_tParam.Project_ID);
	EEPROM_TRACE("g_tParam.tmc429_VMax[0]=%d\n",										g_tParam.tmc429_VMax[0]);
	EEPROM_TRACE("g_tParam.speed_home[1]=%d\n",											g_tParam.speed_home[1]);
	EEPROM_TRACE("g_tParam.speed_home[2]=%d\n",											g_tParam.speed_home[2]);
	
	if (g_tParam.ParamVer != PARAM_VER)
	{
		g_tParam.ParamVer   = PARAM_VER;
		g_tParam.MasterBaudrate  = 115200;
		g_tParam.SlaveBaudrate   = 115200;
		g_tParam.Project_ID = COMMON;
	
#if defined(USING_TMC429_CLK_12MHZ) 
		
		g_tParam.tmc429_VMax[0] = 140;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 140;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 140;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif

#if defined(USING_TMC429_CLK_16MHZ) 
		
		g_tParam.tmc429_VMax[0] = 839;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 839;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 839;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif
		SaveParamToEeprom();						
	}
	switch(g_tParam.Project_ID)
	{
		case BUTTON_ONLINE: 
			rt_kprintf(" --- type: BUTTON_ONLINE\n");		//button_online
			break;
		case BUTTON_OFFLINE: 
			rt_kprintf(" --- type: BUTTON_OFFLINE\n");	//button_offline
			break;
		case OQC_FLEX: 																//OQC-Flex || BUTTON-AXIS-1
			rt_kprintf(" --- type: OQC_FLEX\n");				
			break;
		case BUTTON_ROAD: 														//button_road	|| BUTTON-AXIS-2
			rt_kprintf(" --- type: BUTTON_ROAD\n");
			break;
		case AOI_2AXIS: 
			rt_kprintf(" --- type: AOI-2AXIS\n");
			break;
		case COMMON: 
			rt_kprintf(" --- type: common user\n");
			break;
		default:
			rt_kprintf("\ncontroller init ok --- type: common user\n");
		break;
	}
}


static void LoadSettingViaProID(void)
{
	
}
static void printf_cmdList_param_save(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("Please input: ParamSave <par> <value> ......\n");
		rt_kprintf("ParamSave MasterBaudrate <baudrate>      -set the controller baudrate\n");	
		rt_kprintf("ParamSave SlaveBaudrate <baudrate>       -set the device baudrate\n");		
		rt_kprintf("ParamSave ProjectType <project>          -set the project type\n\n");	
	
		rt_kprintf("ParamSave ProjectType button_online      -set controller fit to button online fixture\n");	
		rt_kprintf("ParamSave ProjectType button_offline     -set controller fit to button offline fixture\n");	
		
		rt_kprintf("ParamSave ProjectType button_road        -set controller fit to button road fixture\n");
		
		rt_kprintf("ParamSave ProjectType OQC-Flex           -set controller fit to OQC-Flex fixture\n");
		rt_kprintf("ParamSave ProjectType lidopen            -set controller fit to lidopen fixture\n");
		
		rt_kprintf("ParamSave ProjectType BUTTON-AXIS-1      -set controller fit to use 1 motor\n");
		rt_kprintf("ParamSave ProjectType BUTTON-AXIS-2      -set controller fit to use 2 motor\n");

		rt_kprintf("ParamSave ProjectType AOI-AXIS-2         -set controller fit to use 2 motor\n");
		rt_kprintf("ParamSave ProjectType common_use         -set controller fit to common_use\n");			
}
/*********************************************************************************************************************/
void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;		//C24LC256;
	at24cxx.init(at24c256);
	if (at24cxx.check(at24c256) == 0)
	{
		rt_kprintf("at24cxx_hw_init()...fail: no eeprom found!!!\n");
	}
	else 
	{
		rt_kprintf("at24cxx_hw_init()...ok");
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
			rt_kprintf("Please input: savedata <page number> <data to save>\n");
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
			rt_kprintf("Please input: readdata <page number>\n");
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
		if 			(!strcmp(argv[1], "MasterBaudrate")) 
			g_tParam.MasterBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "SlaveBaudrate")) 
			g_tParam.SlaveBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "ProjectType"))
		{
			if 			(!strcmp(argv[2], "button_online")) 	g_tParam.Project_ID  = BUTTON_ONLINE;	
			else if (!strcmp(argv[2], "button_offline")) 	g_tParam.Project_ID  = BUTTON_OFFLINE;	
			else if (!strcmp(argv[2], "OQC-Flex")) 		    g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "lidopen")) 				g_tParam.Project_ID  = LIDOPEN;	
			else if (!strcmp(argv[2], "button_road")) 		g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			
			else if (!strcmp(argv[2], "BUTTON-AXIS-1")) 	g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "BUTTON-AXIS-2")) 	g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			else if (!strcmp(argv[2], "AOI-AXIS-2")) 		  g_tParam.Project_ID  = AOI_2AXIS;			//双轴
			
			else if (!strcmp(argv[2], "common_use")) 		  g_tParam.Project_ID  = COMMON;			//双轴
			
			else 	result = REPLY_INVALID_VALUE;
		}
		else result = REPLY_INVALID_CMD;
	}
	else
	{
		result = REPLY_INVALID_CMD;
	}
	if(result == REPLY_OK ) 
	{
		SaveParamToEeprom();	
		rt_kprintf("save parameter ok and controller start to load setting......");	//reboot();	
		LoadSettingViaProID();
		rt_kprintf("ok\n");
	}
	else 
	{
		printf_cmdList_param_save();
	}
	return result;
}
//
/******************************************finsh***********************************************************************/
//INIT_APP_EXPORT(at24cxx_hw_init);
//MSH_CMD_EXPORT(savedata, save data to page 1~200);
//MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);
/*********************************************************************************************************************/



