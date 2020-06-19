#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_eeprom_24xx.h"

//�����ǻ���AT24C128|256��Ӧ��
#define EEPROM_PAGE_BYTES						64													//!< EEPROM ÿҳ����

//readdata��savedata �����ַ������湦��		
//100ҳ 100��
#define EEPROM_INFO_PAGE_ADDR				( 0 )			//readdata��savedata��ȡ�ռ���ʼ��ַ
#define EEPROM_INFO_PAGE_LEN				100													//readdata��savedata��ȡ�Ŀ���ҳ��

//ROM<number><data> ���и������ı��湦��	
//10ҳ  10*64/4=160��
#define EEPROM_ROM_PAGE_ADDR				( 100 )								
#define EEPROM_ROM_PAGE_LEN					10

//�����������������110ҳ��ʼ
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
















