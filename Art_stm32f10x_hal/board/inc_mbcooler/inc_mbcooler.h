

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INC_MBCOOLER_H
#define __INC_MBCOOLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#if 1																				//stm32fxx_hal_conf.h

  #define HAL_ADC_MODULE_ENABLED
	#define HAL_GPIO_MODULE_ENABLED
	#define HAL_TIM_MODULE_ENABLED
	#define HAL_UART_MODULE_ENABLED
	
#endif
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_GREEN1_Pin GPIO_PIN_2
#define GPIO_GREEN1_GPIO_Port GPIOE
#define GPIO_RED1_Pin GPIO_PIN_3
#define GPIO_RED1_GPIO_Port GPIOE
#define DUT_IN1_Pin GPIO_PIN_4
#define DUT_IN1_GPIO_Port GPIOE
#define DUT_IN2_Pin GPIO_PIN_5
#define DUT_IN2_GPIO_Port GPIOE
#define DUT_IN3_Pin GPIO_PIN_6
#define DUT_IN3_GPIO_Port GPIOE
#define DUT_IN4_Pin GPIO_PIN_13
#define DUT_IN4_GPIO_Port GPIOC
#define DUT_IN5_Pin GPIO_PIN_14
#define DUT_IN5_GPIO_Port GPIOC
#define DUT_IN6_Pin GPIO_PIN_15
#define DUT_IN6_GPIO_Port GPIOC
#define LID_IN1_Pin GPIO_PIN_0
#define LID_IN1_GPIO_Port GPIOC
#define LID_IN2_Pin GPIO_PIN_1
#define LID_IN2_GPIO_Port GPIOC
#define LID_IN3_Pin GPIO_PIN_2
#define LID_IN3_GPIO_Port GPIOC
#define LID_IN4_Pin GPIO_PIN_3
#define LID_IN4_GPIO_Port GPIOC
#define VDC_IN_Pin GPIO_PIN_0
#define VDC_IN_GPIO_Port GPIOA
#define LID_IN5_Pin GPIO_PIN_1
#define LID_IN5_GPIO_Port GPIOA
#define LID_IN6_Pin GPIO_PIN_2
#define LID_IN6_GPIO_Port GPIOA
#define VSNS5_Pin GPIO_PIN_3
#define VSNS5_GPIO_Port GPIOA
#define VSNS4_Pin GPIO_PIN_4
#define VSNS4_GPIO_Port GPIOA
#define VSNS3_Pin GPIO_PIN_5
#define VSNS3_GPIO_Port GPIOA
#define VSNS2_Pin GPIO_PIN_6
#define VSNS2_GPIO_Port GPIOA
#define VSNS1_Pin GPIO_PIN_7
#define VSNS1_GPIO_Port GPIOA
#define VSNS0_Pin GPIO_PIN_4
#define VSNS0_GPIO_Port GPIOC
#define DET_Pin GPIO_PIN_0
#define DET_GPIO_Port GPIOB
#define DE_Pin GPIO_PIN_1
#define DE_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_7
#define LED0_GPIO_Port GPIOE
#define RX_LED_Pin GPIO_PIN_8
#define RX_LED_GPIO_Port GPIOE
#define TX_LED_Pin GPIO_PIN_9
#define TX_LED_GPIO_Port GPIOE
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOE
#define SDA_Pin GPIO_PIN_11
#define SDA_GPIO_Port GPIOE
#define A4_Pin GPIO_PIN_12
#define A4_GPIO_Port GPIOE
#define A3_Pin GPIO_PIN_13
#define A3_GPIO_Port GPIOE
#define A2_Pin GPIO_PIN_14
#define A2_GPIO_Port GPIOE
#define A1_Pin GPIO_PIN_15
#define A1_GPIO_Port GPIOE
#define A0_Pin GPIO_PIN_10
#define A0_GPIO_Port GPIOB
#define GPIO_OUT5_Pin GPIO_PIN_11
#define GPIO_OUT5_GPIO_Port GPIOB
#define GPIO_FAN6_Pin GPIO_PIN_12
#define GPIO_FAN6_GPIO_Port GPIOB
#define GPIO_FAN5_Pin GPIO_PIN_13
#define GPIO_FAN5_GPIO_Port GPIOB
#define GPIO_FAN4_Pin GPIO_PIN_14
#define GPIO_FAN4_GPIO_Port GPIOB
#define GPIO_FAN3_Pin GPIO_PIN_15
#define GPIO_FAN3_GPIO_Port GPIOB
#define GPIO_FAN2_Pin GPIO_PIN_8
#define GPIO_FAN2_GPIO_Port GPIOD
#define GPIO_FAN1_Pin GPIO_PIN_9
#define GPIO_FAN1_GPIO_Port GPIOD
#define GPIO_OUT4_Pin GPIO_PIN_10
#define GPIO_OUT4_GPIO_Port GPIOD
#define GPIO_LOCK6_Pin GPIO_PIN_11
#define GPIO_LOCK6_GPIO_Port GPIOD
#define PWM0_Pin GPIO_PIN_12
#define PWM0_GPIO_Port GPIOD
#define PWM1_Pin GPIO_PIN_13
#define PWM1_GPIO_Port GPIOD
#define GPIO_LOCK5_Pin GPIO_PIN_14
#define GPIO_LOCK5_GPIO_Port GPIOD
#define GPIO_LOCK4_Pin GPIO_PIN_15
#define GPIO_LOCK4_GPIO_Port GPIOD
#define PWM2_Pin GPIO_PIN_6
#define PWM2_GPIO_Port GPIOC
#define PWM3_Pin GPIO_PIN_7
#define PWM3_GPIO_Port GPIOC
#define PWM4_Pin GPIO_PIN_8
#define PWM4_GPIO_Port GPIOC
#define PWM5_Pin GPIO_PIN_9
#define PWM5_GPIO_Port GPIOC
#define GPIO_LOCK3_Pin GPIO_PIN_8
#define GPIO_LOCK3_GPIO_Port GPIOA
#define GPIO_LOCK2_Pin GPIO_PIN_11
#define GPIO_LOCK2_GPIO_Port GPIOA
#define GPIO_LOCK1_Pin GPIO_PIN_12
#define GPIO_LOCK1_GPIO_Port GPIOA
#define GPIO_OUT3_Pin GPIO_PIN_15
#define GPIO_OUT3_GPIO_Port GPIOA
#define GPIO_YELLOW6_Pin GPIO_PIN_10
#define GPIO_YELLOW6_GPIO_Port GPIOC
#define GPIO_GREEN6_Pin GPIO_PIN_11
#define GPIO_GREEN6_GPIO_Port GPIOC
#define GPIO_RED6_Pin GPIO_PIN_12
#define GPIO_RED6_GPIO_Port GPIOC
#define GPIO_YELLOW5_Pin GPIO_PIN_0
#define GPIO_YELLOW5_GPIO_Port GPIOD
#define GPIO_GREEN5_Pin GPIO_PIN_1
#define GPIO_GREEN5_GPIO_Port GPIOD
#define GPIO_RED5_Pin GPIO_PIN_2
#define GPIO_RED5_GPIO_Port GPIOD
#define GPIO_OUT2_Pin GPIO_PIN_3
#define GPIO_OUT2_GPIO_Port GPIOD
#define GPIO_YELLOW4_Pin GPIO_PIN_4
#define GPIO_YELLOW4_GPIO_Port GPIOD
#define GPIO_GREEN4_Pin GPIO_PIN_7
#define GPIO_GREEN4_GPIO_Port GPIOD
#define GPIO_RED4_Pin GPIO_PIN_3
#define GPIO_RED4_GPIO_Port GPIOB
#define GPIO_YELLOW3_Pin GPIO_PIN_4
#define GPIO_YELLOW3_GPIO_Port GPIOB
#define GPIO_GREEN3_Pin GPIO_PIN_5
#define GPIO_GREEN3_GPIO_Port GPIOB
#define GPIO_RED3_Pin GPIO_PIN_6
#define GPIO_RED3_GPIO_Port GPIOB
#define GPIO_OUT1_Pin GPIO_PIN_7
#define GPIO_OUT1_GPIO_Port GPIOB
#define GPIO_YELLOW2_Pin GPIO_PIN_8
#define GPIO_YELLOW2_GPIO_Port GPIOB
#define GPIO_GREEN2_Pin GPIO_PIN_9
#define GPIO_GREEN2_GPIO_Port GPIOB
#define GPIO_RED2_Pin GPIO_PIN_0
#define GPIO_RED2_GPIO_Port GPIOE
#define GPIO_YELLOW1_Pin GPIO_PIN_1
#define GPIO_YELLOW1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
