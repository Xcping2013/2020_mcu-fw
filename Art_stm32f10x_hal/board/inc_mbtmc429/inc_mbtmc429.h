#ifndef __INC_MBTMC429__H
#define __INC_MBTMC429__H

#include "bsp_defines.h"

#define IN8_Pin GPIO_PIN_2
#define IN8_GPIO_Port GPIOE
#define IN9_Pin GPIO_PIN_3
#define IN9_GPIO_Port GPIOE
#define IN10_Pin GPIO_PIN_4
#define IN10_GPIO_Port GPIOE
#define IN11_Pin GPIO_PIN_5
#define IN11_GPIO_Port GPIOE
#define IN12_Pin GPIO_PIN_6
#define IN12_GPIO_Port GPIOE
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC
#define TMC429_CLK1_Pin GPIO_PIN_0
#define TMC429_CLK1_GPIO_Port GPIOB
#define POSCMP1_Pin GPIO_PIN_12
#define POSCMP1_GPIO_Port GPIOE
#define INTOUT1_Pin GPIO_PIN_13
#define INTOUT1_GPIO_Port GPIOE
#define OUT1_Pin GPIO_PIN_15
#define OUT1_GPIO_Port GPIOA
#define OUT2_Pin GPIO_PIN_10
#define OUT2_GPIO_Port GPIOC
#define OUT3_Pin GPIO_PIN_11
#define OUT3_GPIO_Port GPIOC
#define OUT4_Pin GPIO_PIN_12
#define OUT4_GPIO_Port GPIOC
#define OUT5_Pin GPIO_PIN_0
#define OUT5_GPIO_Port GPIOD
#define OUT6_Pin GPIO_PIN_1
#define OUT6_GPIO_Port GPIOD
#define OUT7_Pin GPIO_PIN_2
#define OUT7_GPIO_Port GPIOD
#define OUT8_Pin GPIO_PIN_3
#define OUT8_GPIO_Port GPIOD
#define RGB_B_Pin GPIO_PIN_4
#define RGB_B_GPIO_Port GPIOD
#define RGB_G_Pin GPIO_PIN_5
#define RGB_G_GPIO_Port GPIOD
#define RGB_R_Pin GPIO_PIN_6
#define RGB_R_GPIO_Port GPIOD
#define LED_SYS_Pin GPIO_PIN_7
#define LED_SYS_GPIO_Port GPIOD
#define IIC_SDA_Pin GPIO_PIN_3
#define IIC_SDA_GPIO_Port GPIOB
#define IIC_SCL_Pin GPIO_PIN_4
#define IIC_SCL_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_5
#define IN1_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_6
#define IN2_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_7
#define IN3_GPIO_Port GPIOB
#define IN4_Pin GPIO_PIN_8
#define IN4_GPIO_Port GPIOB
#define IN5_Pin GPIO_PIN_9
#define IN5_GPIO_Port GPIOB
#define IN6_Pin GPIO_PIN_0
#define IN6_GPIO_Port GPIOE
#define IN7_Pin GPIO_PIN_1
#define IN7_GPIO_Port GPIOE

//OSE 8MHZ  USE UART2	SPI1
//#define LED_SYS_PIN			PD_7
//#define	MCU_TX					PA_9
//#define	MCU_RX					PA_10
//#define	SWDIO						PA_13
//#define	SWCLK						PA_14
//#define	IIC_SDA					PB_3
//#define	IIC_SCL					PB_4
//#define	SPI1_CS					PB_4
//#define	SPI1_SCK				PA_5
//#define	SPI1_MISO				PA_6
//#define	SPI1_MOSI				PA_7
//#define	TMC429_CLK1			PB_0
//#define	TMC429_CLK2			PB_1		//IN13
//#define	POSCMP1					PE_12
//#define	INTOUT1					PE_13
//#define	INTOUT2					PE_14		//OUT13
//#define	POSCMP2					PE_15		//OUT14
//#define	SPI2_CS					PB_12		//LED2	H-ON
//#define	SPI2_SCK				PB_13		//IN18
//#define	SPI2_MISO				PB_14		//OUT9
//#define	SPI2_MOSI				PB_15		//IN17
//#define	TMC262_CS1			PD_8		//IN14
//#define	TMC262_CS2			PD_9		//IN16
//#define	TMC262_CS3			PD_10		//OUT10
//#define	TMC262_EN1			PD_11		//OUT12
//#define	TMC262_EN2			PD_12		//IN15
//#define	TMC262_EN3			PD_13		//OUT11



//MBTMC429
//at24cxx_t at24c256=
//{
//	{0},
//	{PB_4,PB_3},	//SDA SCL
//	0xA0,
//};
//#define LED_PIN								 PD_7


#endif
