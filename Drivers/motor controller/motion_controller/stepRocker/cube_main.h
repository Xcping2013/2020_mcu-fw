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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __cube_MAIN_H
#define __cube_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"	
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


/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define IN1_Pin GPIO_PIN_2
#define IN1_GPIO_Port GPIOE
#define IN2_Pin GPIO_PIN_3
#define IN2_GPIO_Port GPIOE
#define IN3_Pin GPIO_PIN_4
#define IN3_GPIO_Port GPIOE
#define IN4_Pin GPIO_PIN_5
#define IN4_GPIO_Port GPIOE
#define IN5_Pin GPIO_PIN_6
#define IN5_GPIO_Port GPIOE
#define IN6_Pin GPIO_PIN_13
#define IN6_GPIO_Port GPIOC
#define IN7_Pin GPIO_PIN_14
#define IN7_GPIO_Port GPIOC
#define IN8_Pin GPIO_PIN_15
#define IN8_GPIO_Port GPIOC
#define IN12_Pin GPIO_PIN_0
#define IN12_GPIO_Port GPIOC
#define IN11_Pin GPIO_PIN_1
#define IN11_GPIO_Port GPIOC
#define IN10_Pin GPIO_PIN_2
#define IN10_GPIO_Port GPIOC
#define IN9_Pin GPIO_PIN_3
#define IN9_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define ORG3_Pin GPIO_PIN_4
#define ORG3_GPIO_Port GPIOC
#define IIC_SDA_Pin GPIO_PIN_14
#define IIC_SDA_GPIO_Port GPIOE
#define IIC_SCL_Pin GPIO_PIN_15
#define IIC_SCL_GPIO_Port GPIOE
#define SPI2_CS2_Pin GPIO_PIN_8
#define SPI2_CS2_GPIO_Port GPIOD
#define LED_SYS_RUN_Pin GPIO_PIN_9
#define LED_SYS_RUN_GPIO_Port GPIOC
#define ORG5_Pin GPIO_PIN_8
#define ORG5_GPIO_Port GPIOA
#define ORG6_Pin GPIO_PIN_11
#define ORG6_GPIO_Port GPIOA
#define ORG4_Pin GPIO_PIN_12
#define ORG4_GPIO_Port GPIOA
#define OUT9_Pin GPIO_PIN_0
#define OUT9_GPIO_Port GPIOD
#define OUT10_Pin GPIO_PIN_1
#define OUT10_GPIO_Port GPIOD
#define OUT11_Pin GPIO_PIN_2
#define OUT11_GPIO_Port GPIOD
#define OUT12_Pin GPIO_PIN_3
#define OUT12_GPIO_Port GPIOD
#define OUT1_Pin GPIO_PIN_4
#define OUT1_GPIO_Port GPIOD
#define OUT2_Pin GPIO_PIN_5
#define OUT2_GPIO_Port GPIOD
#define OUT3_Pin GPIO_PIN_6
#define OUT3_GPIO_Port GPIOD
#define OUT4_Pin GPIO_PIN_7
#define OUT4_GPIO_Port GPIOD
#define OUT5_Pin GPIO_PIN_6
#define OUT5_GPIO_Port GPIOB
#define OUT6_Pin GPIO_PIN_7
#define OUT6_GPIO_Port GPIOB
#define OUT7_Pin GPIO_PIN_8
#define OUT7_GPIO_Port GPIOB
#define OUT8_Pin GPIO_PIN_9
#define OUT8_GPIO_Port GPIOB
#define ORG2_Pin GPIO_PIN_0
#define ORG2_GPIO_Port GPIOE
#define ORG1_Pin GPIO_PIN_1
#define ORG1_GPIO_Port GPIOE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
