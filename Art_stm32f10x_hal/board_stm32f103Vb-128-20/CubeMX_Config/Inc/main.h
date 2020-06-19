/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define DEV3PB2_Pin GPIO_PIN_2
#define DEV3PB2_GPIO_Port GPIOE
#define DEV3IRQ_Pin GPIO_PIN_3
#define DEV3IRQ_GPIO_Port GPIOE
#define DEV3PB1_Pin GPIO_PIN_4
#define DEV3PB1_GPIO_Port GPIOE
#define DEV3PB0_Pin GPIO_PIN_5
#define DEV3PB0_GPIO_Port GPIOE
#define DEV3PC1_Pin GPIO_PIN_6
#define DEV3PC1_GPIO_Port GPIOE
#define DEV3PC0_Pin GPIO_PIN_13
#define DEV3PC0_GPIO_Port GPIOC
#define EROM_SCL_Pin GPIO_PIN_14
#define EROM_SCL_GPIO_Port GPIOC
#define EROM_SDA_Pin GPIO_PIN_15
#define EROM_SDA_GPIO_Port GPIOC
#define DEV5PC0_Pin GPIO_PIN_0
#define DEV5PC0_GPIO_Port GPIOC
#define DEV5PC1_Pin GPIO_PIN_1
#define DEV5PC1_GPIO_Port GPIOC
#define DEV5PB1_Pin GPIO_PIN_2
#define DEV5PB1_GPIO_Port GPIOC
#define DEV5PB0_Pin GPIO_PIN_3
#define DEV5PB0_GPIO_Port GPIOC
#define DEV5IRQ_Pin GPIO_PIN_0
#define DEV5IRQ_GPIO_Port GPIOA
#define DEV5PB3_Pin GPIO_PIN_1
#define DEV5PB3_GPIO_Port GPIOA
#define DEV5TX_Pin GPIO_PIN_2
#define DEV5TX_GPIO_Port GPIOA
#define DEV5RX_Pin GPIO_PIN_3
#define DEV5RX_GPIO_Port GPIOA
#define FROMCS_Pin GPIO_PIN_4
#define FROMCS_GPIO_Port GPIOA
#define DEVXSCK_Pin GPIO_PIN_5
#define DEVXSCK_GPIO_Port GPIOA
#define DEVXMISO_Pin GPIO_PIN_6
#define DEVXMISO_GPIO_Port GPIOA
#define DEVXMOSI_Pin GPIO_PIN_7
#define DEVXMOSI_GPIO_Port GPIOA
#define DEV5PB2_Pin GPIO_PIN_4
#define DEV5PB2_GPIO_Port GPIOC
#define DEV5PC3_Pin GPIO_PIN_5
#define DEV5PC3_GPIO_Port GPIOC
#define DEV5PC2_Pin GPIO_PIN_0
#define DEV5PC2_GPIO_Port GPIOB
#define INTPUT_IRQ_Pin GPIO_PIN_1
#define INTPUT_IRQ_GPIO_Port GPIOB
#define LED_BOOT1_Pin GPIO_PIN_2
#define LED_BOOT1_GPIO_Port GPIOB
#define DEV4PC0_Pin GPIO_PIN_7
#define DEV4PC0_GPIO_Port GPIOE
#define DEV4PC1_Pin GPIO_PIN_8
#define DEV4PC1_GPIO_Port GPIOE
#define DEV4PB0_Pin GPIO_PIN_9
#define DEV4PB0_GPIO_Port GPIOE
#define DEV4PB1_Pin GPIO_PIN_10
#define DEV4PB1_GPIO_Port GPIOE
#define DEV4IRQ_Pin GPIO_PIN_11
#define DEV4IRQ_GPIO_Port GPIOE
#define DEV4PB2_Pin GPIO_PIN_12
#define DEV4PB2_GPIO_Port GPIOE
#define DEV4PB3_Pin GPIO_PIN_13
#define DEV4PB3_GPIO_Port GPIOE
#define DEV4PC2_Pin GPIO_PIN_14
#define DEV4PC2_GPIO_Port GPIOE
#define DEV4PC3_Pin GPIO_PIN_15
#define DEV4PC3_GPIO_Port GPIOE
#define DEV2PC0_Pin GPIO_PIN_10
#define DEV2PC0_GPIO_Port GPIOB
#define DEV2PC1_Pin GPIO_PIN_11
#define DEV2PC1_GPIO_Port GPIOB
#define DEV1CS_Pin GPIO_PIN_12
#define DEV1CS_GPIO_Port GPIOB
#define DEV1SCK_Pin GPIO_PIN_13
#define DEV1SCK_GPIO_Port GPIOB
#define DEV1MISO_Pin GPIO_PIN_14
#define DEV1MISO_GPIO_Port GPIOB
#define DEV1MOSI_Pin GPIO_PIN_15
#define DEV1MOSI_GPIO_Port GPIOB
#define DEV4TX_Pin GPIO_PIN_8
#define DEV4TX_GPIO_Port GPIOD
#define DEV4RX_Pin GPIO_PIN_9
#define DEV4RX_GPIO_Port GPIOD
#define DEV1PC3_Pin GPIO_PIN_10
#define DEV1PC3_GPIO_Port GPIOD
#define DEV1PC2_Pin GPIO_PIN_11
#define DEV1PC2_GPIO_Port GPIOD
#define DEV1PA2_Pin GPIO_PIN_12
#define DEV1PA2_GPIO_Port GPIOD
#define DEV1PA1_Pin GPIO_PIN_13
#define DEV1PA1_GPIO_Port GPIOD
#define DEV1PA0_Pin GPIO_PIN_14
#define DEV1PA0_GPIO_Port GPIOD
#define DEV1PC1_Pin GPIO_PIN_15
#define DEV1PC1_GPIO_Port GPIOD
#define DEV1PC0_Pin GPIO_PIN_6
#define DEV1PC0_GPIO_Port GPIOC
#define DEV2PC3_Pin GPIO_PIN_7
#define DEV2PC3_GPIO_Port GPIOC
#define DEV2PC2_Pin GPIO_PIN_8
#define DEV2PC2_GPIO_Port GPIOC
#define DEV2PB3_Pin GPIO_PIN_9
#define DEV2PB3_GPIO_Port GPIOC
#define DEV2PB2_Pin GPIO_PIN_8
#define DEV2PB2_GPIO_Port GPIOA
#define MCU_TX_Pin GPIO_PIN_9
#define MCU_TX_GPIO_Port GPIOA
#define MCU_RX_Pin GPIO_PIN_10
#define MCU_RX_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_LANIRQ_Pin GPIO_PIN_13
#define SWDIO_LANIRQ_GPIO_Port GPIOA
#define SWCLK_LANCS_Pin GPIO_PIN_14
#define SWCLK_LANCS_GPIO_Port GPIOA
#define DEV2IRQ_Pin GPIO_PIN_15
#define DEV2IRQ_GPIO_Port GPIOA
#define DEV2TX_Pin GPIO_PIN_10
#define DEV2TX_GPIO_Port GPIOC
#define DEV2RX_Pin GPIO_PIN_11
#define DEV2RX_GPIO_Port GPIOC
#define DEV3TX_Pin GPIO_PIN_12
#define DEV3TX_GPIO_Port GPIOC
#define DEV2PB1_Pin GPIO_PIN_0
#define DEV2PB1_GPIO_Port GPIOD
#define DEV2PB0_Pin GPIO_PIN_1
#define DEV2PB0_GPIO_Port GPIOD
#define DEV3RX_Pin GPIO_PIN_2
#define DEV3RX_GPIO_Port GPIOD
#define DEV0PC1_Pin GPIO_PIN_3
#define DEV0PC1_GPIO_Port GPIOD
#define DEV0PC0_Pin GPIO_PIN_4
#define DEV0PC0_GPIO_Port GPIOD
#define DEV0IRQ_Pin GPIO_PIN_5
#define DEV0IRQ_GPIO_Port GPIOD
#define DEV0PA1_Pin GPIO_PIN_6
#define DEV0PA1_GPIO_Port GPIOD
#define DEV0PA2_Pin GPIO_PIN_7
#define DEV0PA2_GPIO_Port GPIOD
#define DEV0SCK_Pin GPIO_PIN_3
#define DEV0SCK_GPIO_Port GPIOB
#define DEV0MISO_Pin GPIO_PIN_4
#define DEV0MISO_GPIO_Port GPIOB
#define DEV0MOSI_Pin GPIO_PIN_5
#define DEV0MOSI_GPIO_Port GPIOB
#define DEV0PC3_Pin GPIO_PIN_6
#define DEV0PC3_GPIO_Port GPIOB
#define DEV0CS_Pin GPIO_PIN_7
#define DEV0CS_GPIO_Port GPIOB
#define DEV0PC2_Pin GPIO_PIN_8
#define DEV0PC2_GPIO_Port GPIOB
#define DEV3PC3_Pin GPIO_PIN_9
#define DEV3PC3_GPIO_Port GPIOB
#define DEV3PC2_Pin GPIO_PIN_0
#define DEV3PC2_GPIO_Port GPIOE
#define DEV3PB3_Pin GPIO_PIN_1
#define DEV3PB3_GPIO_Port GPIOE

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
void cube_hal_inits(void);

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
