#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_eeprom_24xx.h"

/*	AT24C128|256	*/
#define EEPROM_PAGE_BYTES						64													//!< EEPROM 每页长度

/*	100Pages  0~99		readdata、savedata 字符串进行页读写操作	*/
#define EEPROM_INFO_PAGE_ADDR				( 0 )												//readdata、savedata存取空间起始地址
#define EEPROM_INFO_PAGE_LEN				100													//readdata、savedata存取的可用页数
/*	剩下的156页用于其他数据保存	*/

/* 10Pages  100~109	ROM<number><data> 进行浮点数的保存功能	10*64/4=160个	 */
#define EEPROM_ROM_PAGE_ADDR				( 100 )								
#define EEPROM_ROM_PAGE_LEN					10

#define EEPROM_FSN_PAGE_ADDR				250			
#define EEPROM_FSN_PAGE_LEN					1

/*	剩下的146页用于其他数据保存	*/

//以下其他参数保存从110页开始
/*
#define EEPROM_ADC_CAL_PAGE_ADDR				110~111 

#define EEPROM_MCP3421_CAL_PAGE_ADDR		112 

#define EEPROM_TMC429_PAR_ADDR		 			113 
#define EEPROM_TMC429_PAR_LEN		   			2   

#define EEPROM_TMC429_PAR_ADDR		 			113 
#define EEPROM_TMC429_PAR_LEN		  

#define EEPROM_LED_PAR_ADDR				 			114 
#define EEPROM_LED_PAR_LEN				 			1		  
	
#define EEPROM_DMM_CAL_PAGE_ADDR				115-116
#define EEPROM_DMM_CAL_PAGE_LEN	 	  		2	

#define EEPROM_TEST_PAGE_ADDR			 			254	



其他单字节独立标志信息 统一在255页

#define EEPROM_APP_EN_ADDR 				 			255*64+0
#define EEPROM_APP_CS_ADDR 							255*64+1


*/
extern at24cxx_t at24c256;

extern uint8_t Command_analysis_eeprom(char *string);
void at24cxx_hw_init(void);

uint8_t at24cxx_msh(char *string);

uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

#endif
















