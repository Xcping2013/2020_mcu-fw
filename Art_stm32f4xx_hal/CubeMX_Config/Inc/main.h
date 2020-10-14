/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SD_CS_Pin GPIO_PIN_4
#define SD_CS_GPIO_Port GPIOE
#define VALVE_CTRL_PUFF6_Pin GPIO_PIN_13
#define VALVE_CTRL_PUFF6_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOF
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOF
#define SD_DETECT_Pin GPIO_PIN_2
#define SD_DETECT_GPIO_Port GPIOF
#define VALVE_CTRL_PUFF0_Pin GPIO_PIN_6
#define VALVE_CTRL_PUFF0_GPIO_Port GPIOF
#define VALVE_CTRL_PUFF1_Pin GPIO_PIN_7
#define VALVE_CTRL_PUFF1_GPIO_Port GPIOF
#define VALVE_CTRL_PUFF2_Pin GPIO_PIN_1
#define VALVE_CTRL_PUFF2_GPIO_Port GPIOH
#define EEPROM_WP_Pin GPIO_PIN_14
#define EEPROM_WP_GPIO_Port GPIOE
#define I2C2_SCL_Pin GPIO_PIN_10
#define I2C2_SCL_GPIO_Port GPIOB
#define I2C2_SDA_Pin GPIO_PIN_11
#define I2C2_SDA_GPIO_Port GPIOB
#define I2C1_IRQ_Pin GPIO_PIN_9
#define I2C1_IRQ_GPIO_Port GPIOC
#define VALVE_CTRL_PUFF3_Pin GPIO_PIN_14
#define VALVE_CTRL_PUFF3_GPIO_Port GPIOA
#define VALVE_CTRL_PUFF4_Pin GPIO_PIN_15
#define VALVE_CTRL_PUFF4_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_1
#define LED_BLUE_GPIO_Port GPIOD
#define DISPLAY_RESET_Pin GPIO_PIN_7
#define DISPLAY_RESET_GPIO_Port GPIOD
#define VALVE_CTRL_PUFF5_Pin GPIO_PIN_7
#define VALVE_CTRL_PUFF5_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_9
#define I2C1_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
/*
#define SCALE_RX_MCU_TX_PIN     GET_PIN(C, 10)  USART3
#define SCALE_TX_MCU_RX_PIN     GET_PIN(C, 11)

#define MFC_TX_MCU_RX_PIN       GET_PIN(G, 9)   USART6
#define MFC_RX_MCU_TX_PIN       GET_PIN(G, 14)

#define DUT_RX_MCU_TX_PIN       GET_PIN(C, 12)  UART5
#define DUT_TX_MCU_RX_PIN       GET_PIN(D, 2)

#define TERM_TX_MCU_RX_PIN      GET_PIN(E, 0)   UART8
#define TERM_RX_MCU_TX_PIN      GET_PIN(E, 1)

#define VALVE_CTRL_PUFF0_PIN    GET_PIN(F, 6)   OUTPUT
#define VALVE_CTRL_PUFF1_PIN    GET_PIN(F, 7)
#define VALVE_CTRL_PUFF2_PIN    GET_PIN(A, 13)
#define VALVE_CTRL_PUFF3_PIN    GET_PIN(A, 14)
#define VALVE_CTRL_PUFF4_PIN    GET_PIN(A, 15)
#define VALVE_CTRL_PUFF5_PIN    GET_PIN(B, 7)
#define VALVE_CTRL_PUFF6_PIN    GET_PIN(C, 13)

#define DISPLAY_RX_MCU_TX_PIN   GET_PIN(D, 5)   USART2
#define DISPLAY_TX_MCU_RX_PIN   GET_PIN(D, 6)
#define DISPLAY_RESET_PIN       GET_PIN(D, 7)

#define SD_SCLK_PIN             GET_PIN(E, 2)   SPI4
#define SD_CS_PIN               GET_PIN(E, 4)
#define SD_MISO_PIN             GET_PIN(E, 5)
#define SD_MOSI_PIN             GET_PIN(E, 6)
#define SD_DETECT_PIN           GET_PIN(F, 2)

#define LED_RED_PIN             GET_PIN(F, 1)
#define LED_GREEN_PIN           GET_PIN(F, 0)
#define LED_BLUE_PIN            GET_PIN(D, 1)

#define I2C_IRQ1_PIN            GET_PIN(C, 9)   I2C1    DLLR SENSOR     DLLR-L10D-E2BS-C-NAV6
#define I2C_SCL1_PIN            GET_PIN(B, 8)
#define I2C_SDA1_PIN            GET_PIN(B, 9)

#define BOARD_I2C_SCL_PIN       GET_PIN(B, 10)  I2C2    EEPROM_AT24CXX  BME280
#define BOARD_I2C_SDA_PIN       GET_PIN(B, 11)

#define EEPROM_WP_PIN           GET_PIN(E, 14)      
*/
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
