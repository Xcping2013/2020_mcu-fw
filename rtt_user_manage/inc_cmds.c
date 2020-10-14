#include <rthw.h>
#include "msh.h"
#include "finsh.h"
#include "shell.h"

#include "user_finsh.h"

#include "cmds_dut_dummy.h"	

#include "app_eeprom_24xx.h"

uint8_t  cmds_lower_flag=0;
uint8_t  cmdtype=0;

uint8_t cmds_lowercase(void)
{
	return cmds_lower_flag;
}
int UserMsh_Commands(char *cmd, rt_size_t length)
{	
#if 0
	if(at24cxx_msh(cmd)==REPLY_OK) 
	{
		return 0;
	}	
	if(!strncmp("eeprom help",cmd,4))
	{
		rt_kprintf("sn Commands:\n");
		rt_kprintf("SN       -------read sn from eeprom\n");
		rt_kprintf("SN:xxxxxx-------save sn to eeprom\n");
		rt_kprintf("-------------------------------------------------\n");
		rt_kprintf("EEPROM Commands:\n");
		rt_kprintf("EEPROM WritePage PageNum(0~255) String(1~64Bytes)\n");
		rt_kprintf("ex:EEPROM WritePage 0 SN:20200923\n");
		rt_kprintf("EEPROM ReadPage PageNum(0~255)\n");
		rt_kprintf("ex:EEPROM ReadPage 0\n");
		rt_kprintf("-------------------------------------------------\n");

		rt_kprintf("EEPROM Write addr(0~16383) String\n");
		rt_kprintf("ex:EEPROM Write 0 SN:20200923\n");
		rt_kprintf("EEPROM Read addr(0~16383) length\n");
		rt_kprintf("ex:EEPROM Read 0 11\n");
		rt_kprintf("-------------------------------------------------\n");
		return 0;
	}	
	//if(MB1616_DIDO_msh(cmd)==REPLY_OK) return 0;
#endif 
	if(DUT_Dummy_Commands(cmd)==REPLY_OK) return 0;
#ifdef FINSH_USING_MSH	
	msh_exec(cmd,length);
#endif
	
	return 0;
}	

/*****************************************************************************************************/
enum boardDevID {

boardMotor, 	

};
uint8_t  boardAddrRxd=0;
uint8_t  boardDev=0;

/*
#define RT_CONSOLEBUF_SIZE 256
后续板卡使用的命令格式为：boardAddr.dev.fun(par1,par2,...parn)
1.提取boardAddr 5位拨码 2**5=0~31
2.提取dev
3.调用function(devType,par)
*/
uint8_t UserFinsh_Commands(struct finsh_parser *parser, char *line, uint8_t position)
{
	uint8_t i,index=0; 
	
	line[position] = '\0';

//	boardAddrRxd=atoi(line);	
//	
//	if(boardAddrRxd<10) {;} else {index=index+1;}

//	if(at24cxx_msh(line)==REPLY_OK) 
//	{
//		line[position] = ';';
//		return 0;
//	}
	
#if 0		//functon par1 par2
	if(!strncmp("help",line,4))
	{
		rt_kprintf("\n");
		MB1616_CMD_help();
		FBTMC429_Motor_help();
		line[position] = ';';
		return 0;
	}	
	
	if(MB1616_DIDO_msh(line)==REPLY_OK) 
	{
		line[position] = ';';
		return 0;
	}
	if(MB1616_EEPROM_msh(line)==REPLY_OK) 
	{
		line[position] = ';';
		return 0;
	}
	
	if(FBTMC429_Motor_msh(line)==REPLY_OK) 
	{
		line[position] = ';';
		return 0;
	}
#endif

#if 0  //functon( par1 par2)
//	
//	index=MB1616_DIDO_Finsh(line);
//	
//	if(index==255)
//	{
//		rt_kprintf("\n");
//		line[position] = ';';
//		return 0;
//	}	
//	index=MB1616_FAL_Finsh(line);
//	
//	if(index==255)
//	{
//		rt_kprintf("\n");
//		line[position] = ';';
//		return 0;
//	}
#endif

{
	line[position] = ';';
#ifndef FINSH_USING_MSH_ONLY
	finsh_run_line(parser, &line[index]);
#endif	
}

	return 0;
}

uint8_t msh_is_used_app(void)
{
#ifdef FINSH_USING_MSH
	if (msh_is_used() == RT_FALSE)
	{
						
	}
#endif
	return 0;

}			
				
//






				
				