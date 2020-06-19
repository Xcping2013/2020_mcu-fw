
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INC_MBKBCN1_H__
#define __INC_MBKBCN1_H__

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_defines.h"
#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softi2c.h"
#include "bsp_eeprom_24xx.h"
#include "app_eeprom_24xx.h"



enum ProjectId 
{
	GRME_PONY_Keyboard,
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

extern SPI_HandleTypeDef hspi1;

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/*  CubeMX Config

*/ 
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif
