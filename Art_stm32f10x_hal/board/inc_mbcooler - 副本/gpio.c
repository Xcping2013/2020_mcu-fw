/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_GREEN1_Pin|GPIO_RED1_Pin|LED0_Pin|RX_LED_Pin
                          |TX_LED_Pin|SCL_Pin|SDA_Pin|GPIO_RED2_Pin
                          |GPIO_YELLOW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DE_Pin|GPIO_OUT5_Pin|GPIO_FAN6_Pin|GPIO_FAN5_Pin
                          |GPIO_FAN4_Pin|GPIO_FAN3_Pin|GPIO_RED4_Pin|GPIO_YELLOW3_Pin
                          |GPIO_GREEN3_Pin|GPIO_RED3_Pin|GPIO_OUT1_Pin|GPIO_YELLOW2_Pin
                          |GPIO_GREEN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_FAN2_Pin|GPIO_FAN1_Pin|GPIO_OUT4_Pin|GPIO_LOCK6_Pin
                          |GPIO_LOCK5_Pin|GPIO_LOCK4_Pin|GPIO_YELLOW5_Pin|GPIO_GREEN5_Pin
                          |GPIO_RED5_Pin|GPIO_OUT2_Pin|GPIO_YELLOW4_Pin|GPIO_GREEN4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_LOCK3_Pin|GPIO_LOCK2_Pin|GPIO_LOCK1_Pin|GPIO_OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_YELLOW6_Pin|GPIO_GREEN6_Pin|GPIO_RED6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin */
  GPIO_InitStruct.Pin = GPIO_GREEN1_Pin|GPIO_RED1_Pin|LED0_Pin|RX_LED_Pin
                          |TX_LED_Pin|SCL_Pin|SDA_Pin|GPIO_RED2_Pin
                          |GPIO_YELLOW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = DUT_IN1_Pin|DUT_IN2_Pin|DUT_IN3_Pin|A4_Pin
                          |A3_Pin|A2_Pin|A1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = DUT_IN4_Pin|DUT_IN5_Pin|DUT_IN6_Pin|LID_IN1_Pin
                          |LID_IN2_Pin|LID_IN3_Pin|LID_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = LID_IN5_Pin|LID_IN6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = DET_Pin|A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin
                           PBPin PBPin PBPin PBPin
                           PBPin PBPin PBPin PBPin
                           PBPin */
  GPIO_InitStruct.Pin = DE_Pin|GPIO_OUT5_Pin|GPIO_FAN6_Pin|GPIO_FAN5_Pin
                          |GPIO_FAN4_Pin|GPIO_FAN3_Pin|GPIO_RED4_Pin|GPIO_YELLOW3_Pin
                          |GPIO_GREEN3_Pin|GPIO_RED3_Pin|GPIO_OUT1_Pin|GPIO_YELLOW2_Pin
                          |GPIO_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin PDPin PDPin PDPin
                           PDPin PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = GPIO_FAN2_Pin|GPIO_FAN1_Pin|GPIO_OUT4_Pin|GPIO_LOCK6_Pin
                          |GPIO_LOCK5_Pin|GPIO_LOCK4_Pin|GPIO_YELLOW5_Pin|GPIO_GREEN5_Pin
                          |GPIO_RED5_Pin|GPIO_OUT2_Pin|GPIO_YELLOW4_Pin|GPIO_GREEN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = GPIO_LOCK3_Pin|GPIO_LOCK2_Pin|GPIO_LOCK1_Pin|GPIO_OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = GPIO_YELLOW6_Pin|GPIO_GREEN6_Pin|GPIO_RED6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
