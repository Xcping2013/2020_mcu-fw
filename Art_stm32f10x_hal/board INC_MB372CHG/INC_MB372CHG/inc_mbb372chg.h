
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

//�̶�����1				  10Pagea = 64*10=640Byte	0~9		  �ṹ�����ݱ���
//�̶�����2					10Pagea = 64*10=640Byte	10~19		�������ݱ���
//�̶�����3					35Pagea = 64*34Bytes  	20~54		ROM���ݱ���
//�ɱ��������			200											55~254
#define EEPROM_PAGE_BYTES						64					//!< EEPROM ÿҳ����
#define PARAM_VER										00

#define EEPROM_PARAM_PAGE_ADDR			0			
#define EEPROM_PARAM_PAGE_LEN				10

#define EEPROM_VAR_PAGE_ADDR				10			
#define EEPROM_VAR_PAGE_LEN					10

#define EEPROM_ROM_PAGE_ADDR				20					//!< EEPROM ROM�����������ȡ�ռ���ʼ��ַ
#define EEPROM_ROM_PAGE_LEN					35

#define EEPROM_INFO_PAGE_ADDR				55					//!< EEPROM ��ȡ�����ռ���ʼ��ַ
#define EEPROM_INFO_PAGE_LEN				200					//!< EEPROM ���õĿ���ҳ��

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004


enum ProjectId 
{
	B372_CHG,
};

typedef struct	
{
	uint32_t ParamVer;									//4					/* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */
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
