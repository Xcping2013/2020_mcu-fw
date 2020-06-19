#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_eeprom_24xx.h"

//以下是基于AT24C128|256的应用
#define EEPROM_PAGE_BYTES						64													//!< EEPROM 每页长度

//readdata、savedata 进行字符串保存功能		
//100页 100串
#define EEPROM_INFO_PAGE_ADDR				( 0 )			//readdata、savedata存取空间起始地址
#define EEPROM_INFO_PAGE_LEN				100													//readdata、savedata存取的可用页数

//ROM<number><data> 进行浮点数的保存功能	
//10页  10*64/4=160个
#define EEPROM_ROM_PAGE_ADDR				( 100 )								
#define EEPROM_ROM_PAGE_LEN					10

//以下其他参数保存从110页开始
/*
#define EEPROM_ADC_CAL_PAGE_ADDR		110~111 

#define EEPROM_MCP3421_CAL_PAGE_ADDR		112 

#define EEPROM_TMC429_PAR_ADDR		 113 
#define EEPROM_TMC429_PAR_LEN		   2   

#define EEPROM_TMC429_PAR_ADDR		 113 
#define EEPROM_TMC429_PAR_LEN		  

#define EEPROM_LED_PAR_ADDR				 114 
#define EEPROM_LED_PAR_LEN				 1		  
		   
*/
extern at24cxx_t at24c256;

void at24cxx_hw_init(void);

uint8_t at24cxx_msh(char *string);

#endif
















