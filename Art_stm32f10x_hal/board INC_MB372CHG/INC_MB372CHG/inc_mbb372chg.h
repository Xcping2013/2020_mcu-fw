
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INC_MBB372CHG_H__
#define __INC_MBB372CHG_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "bsp_defines.h"

//#include "bsp_mcu_spi.h"
#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softi2c.h"

#include "bsp_eeprom_24xx.h"

#include "eeprom_24xx.h"

//固定参数1				  10Pagea = 64*10=640Byte	0~9		  结构体数据保存
//固定参数2					10Pagea = 64*10=640Byte	10~19		备用数据保存
//固定参数3					35Pagea = 64*34Bytes  	20~54		ROM数据保存
//可变参数命令			200											55~254
#define EEPROM_PAGE_BYTES						64					//!< EEPROM 每页长度
#define PARAM_VER										00

#define EEPROM_PARAM_PAGE_ADDR			0			
#define EEPROM_PARAM_PAGE_LEN				10

#define EEPROM_VAR_PAGE_ADDR				10			
#define EEPROM_VAR_PAGE_LEN					10

#define EEPROM_ROM_PAGE_ADDR				20					//!< EEPROM ROM等其他命令存取空间起始地址
#define EEPROM_ROM_PAGE_LEN					35

#define EEPROM_INFO_PAGE_ADDR				55					//!< EEPROM 存取操作空间起始地址
#define EEPROM_INFO_PAGE_LEN				200					//!< EEPROM 设置的可用页数

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004


enum ProjectId 
{
	B372_CHG,
};

typedef struct	
{
	uint32_t ParamVer;									//4					/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
	uint8_t	 Project_ID;								//1
}
PARAM_T;															

extern PARAM_T g_tParam;

extern at24cxx_t at24c256;

extern uint8_t LED_PIN;

extern char version_string[];

/* USER CODE END Includes */


/* Private define ------------------------------------------------------------*/

/*  CubeMX Config

*/ 


void _Error_Handler(char *, int);


#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif
