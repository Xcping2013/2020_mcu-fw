/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "stm32f1xx_hal.h"

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
//void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SEL0_Pin GPIO_PIN_13
#define SEL0_GPIO_Port GPIOC
#define SEL1_Pin GPIO_PIN_14
#define SEL1_GPIO_Port GPIOC
#define SEL2_Pin GPIO_PIN_15
#define SEL2_GPIO_Port GPIOC
#define SEL3_Pin GPIO_PIN_0
#define SEL3_GPIO_Port GPIOD
#define SEL4_Pin GPIO_PIN_1
#define SEL4_GPIO_Port GPIOD
#define AN0_Pin GPIO_PIN_0
#define AN0_GPIO_Port GPIOA
#define AN1_Pin GPIO_PIN_1
#define AN1_GPIO_Port GPIOA
#define AN2_Pin GPIO_PIN_4
#define AN2_GPIO_Port GPIOA
#define AN3_Pin GPIO_PIN_5
#define AN3_GPIO_Port GPIOA
#define AN4_Pin GPIO_PIN_6
#define AN4_GPIO_Port GPIOA
#define AN5_Pin GPIO_PIN_7
#define AN5_GPIO_Port GPIOA
#define AN6_Pin GPIO_PIN_0
#define AN6_GPIO_Port GPIOB
#define AN7_Pin GPIO_PIN_1
#define AN7_GPIO_Port GPIOB
#define SEL5_Pin GPIO_PIN_2
#define SEL5_GPIO_Port GPIOB
#define SEL6_Pin GPIO_PIN_10
#define SEL6_GPIO_Port GPIOB
#define SEL7_Pin GPIO_PIN_11
#define SEL7_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define CHG_EN7_Pin GPIO_PIN_14
#define CHG_EN7_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_15
#define MOSI_GPIO_Port GPIOB
#define CHG_EN6_Pin GPIO_PIN_8
#define CHG_EN6_GPIO_Port GPIOA
#define CHG_EN5_Pin GPIO_PIN_11
#define CHG_EN5_GPIO_Port GPIOA
#define CHG_EN4_Pin GPIO_PIN_12
#define CHG_EN4_GPIO_Port GPIOA
#define SWDIO_DE_Pin GPIO_PIN_13
#define SWDIO_DE_GPIO_Port GPIOA
#define SWCLK_DET_Pin GPIO_PIN_14
#define SWCLK_DET_GPIO_Port GPIOA
#define CHG_EN3_Pin GPIO_PIN_15
#define CHG_EN3_GPIO_Port GPIOA
#define CHG_EN2_Pin GPIO_PIN_3
#define CHG_EN2_GPIO_Port GPIOB
#define CHG_EN1_Pin GPIO_PIN_4
#define CHG_EN1_GPIO_Port GPIOB
#define CHG_EN0_Pin GPIO_PIN_5
#define CHG_EN0_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_7
#define LED0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

int CubeMX_Hal_Init(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
