
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INC_MB1616DEV6_H__
#define __INC_MB1616DEV6_H__

/* USER CODE BEGIN Includes */
#include "bsp_defines.h"
#include "bsp_mcu_delay.h"	
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softi2c.h"
//#include "bsp_mcu_spi.h"
#include "bsp_pca9539a.h"
#include "bsp_eeprom_24xx.h"
/**********************GPIO************************/
#if 1		//DEVXPBY_Pin GPIO_PIN_n  
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

#endif
//
#if 1		//DEVXPBY_PIN_N		PN_n
	#define DEV3PB2_PIN 		PE_2
	#define DEV3IRQ_PIN 		PE_3
	#define DEV3PB1_PIN 		PE_4
	#define DEV3PB0_PIN 		PE_5
	#define DEV3PC1_PIN 		PE_6
	#define DEV3PC0_PIN 		PC_13
	#define EROM_SCL_PIN 		PC_14
	#define EROM_SDA_PIN 		PC_15
	#define DEV5PC0_PIN 		PC_0
	#define DEV5PC1_PIN 		PC_1
	#define DEV5PB1_PIN 		PC_2
	#define DEV5PB0_PIN 		GPC_3
	#define DEV5IRQ_PIN 		PA_0

	#define DEV5PB3_PIN 		PA_1
	#define DEV5TX_PIN 			PA_2
	#define DEV5RX_PIN 			PA_3
	#define FROMCS_PIN 			PA_4
	#define DEVXSCK_PIN 		PA_5
	#define DEVXMISO_PIN 		PA_6
	#define DEVXMOSI_PIN 		PA_7
	#define DEV5PB2_PIN 		PC_4
	#define DEV5PC3_PIN 		PC_5
	#define DEV5PC2_PIN 		PB_0
	#define INTPUT_IRQ_PIN 	PB_1
	#define LED_BOOT1_PIN 	PB_2
	#define DEV4PC0_PIN 		PE_7
	#define DEV4PC1_PIN 		PE_8
	#define DEV4PB0_PIN 		PE_9
	#define DEV4PB1_PIN 		PE_10
	#define DEV4IRQ_PIN 		PE_11
	#define DEV4PB2_PIN 		PE_12
	#define DEV4PB3_PIN 		PE_13
	#define DEV4PC2_PIN 		PE_14
	#define DEV4PC3_PIN 		PE_15

	#define DEV2PC0_PIN 		PB_10
	#define DEV2PC1_PIN 		PB_11
	#define DEV1CS_PIN 			PB_12
	#define DEV1SCK_PIN 		PB_13
	#define DEV1MISO_PIN 		PB_14
	#define DEV1MOSI_PIN 		PB_15
	#define DEV4TX_PIN 			PD_8
	#define DEV4RX_PIN 			PD_9
	#define DEV1PC3_PIN 		PD_10
	#define DEV1PC2_PIN 		PD_11
	#define DEV1PA2_PIN 		PD_12
	#define DEV1PA1_PIN 		PD_13
	#define DEV1PA0_PIN 		PD_14
	#define DEV1PC1_PIN 		PD_15
	#define DEV1PC0_PIN 		PC_6
	#define DEV2PC3_PIN 		PC_7
	#define DEV2PC2_PIN 		PC_8
	#define DEV2PB3_PIN 		PC_9
	#define DEV2PB2_PIN 		PA_8
	#define MCU_TX_PIN 			PA_9
	#define MCU_RX_PIN 			PA_10
	#define USB_DM_PIN 			PA_11
	#define USB_DP_PIN 			PA_12

	#define SWDIO_LANIRQ_PIN 	PA_13
	#define SWCLK_LANCS_PIN 	PA_14

	#define DEV2IRQ_PIN 		PA_15
	#define DEV2TX_PIN 			PC_10
	#define DEV2RX_PIN			PC_11
	#define DEV3TX_PIN 			PC_12
	#define DEV2PB1_PIN 		PD_0
	#define DEV2PB0_PIN 		PD_1
	#define DEV3RX_PIN 			PD_2
	#define DEV0PC1_PIN 		PD_3
	#define DEV0PC0_PIN 		PD_4
	#define DEV0IRQ_PIN 		PD_5
	#define DEV0PA1_PIN 		PD_6
	#define DEV0PA2_PIN 		PD_7

	#define DEV0SCK_PIN 		PB_3
	#define DEV0MISO_PIN 		PB_4
	#define DEV0MOSI_PIN 		PB_5
	#define DEV0PC3_PIN 		PB_6
	#define DEV0CS_PIN 			PB_7
	#define DEV0PC2_PIN 		PB_8
	#define DEV3PC3_PIN 		PB_9
	#define DEV3PC2_PIN 		PE_0
	#define DEV3PB3_PIN 		PE_1
#endif

//void MB1616_CMD_help(void);

//uint8_t MB1616_DIDO_Finsh(char* string);
//uint8_t MB1616_DIDO_msh(char *string);

//uint8_t MB1616_FAL_Finsh(char* string);
//uint8_t MB1616_EEPROM_msh(char *string);

//void cube_hal_inits(void);

//extern SPI_HandleTypeDef hspi1;
//extern SPI_HandleTypeDef hspi2;
//extern SPI_HandleTypeDef hspi3;

//extern UART_HandleTypeDef huart4;
//extern UART_HandleTypeDef huart5;
//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;

//extern TIM_HandleTypeDef htim1;
//extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

//#ifdef __cplusplus
// extern "C" {
//#endif
//void _Error_Handler(char *, int);

//#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
//#ifdef __cplusplus
//}
//#endif

#endif
