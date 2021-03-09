/*	Low-voltage 16-bit I2C-bus I/O port with interrupt and reset

1.VDD range of 1.65 V to 5.5 V
2.open-drain interrupt (INT)   
	indicate that an input state has changed
	if there is incoming data on its ports without having to communicate via the I2C-bus
3.The device outputs have 25 mA sink capabilities
4.Power-up with all channels configured as inputs

*/
/*
	Device address
	
	slave address
	1 1 1 0 1 A1 A0	R/W
		
		W:0xE8
		R:0xE9	
*/
/*	Registers

	ADDR (1Byte) + CMD(1Byte):

	Pointer register bits	B7 B6 B5 B4 B3 B2 B1 B0

	I0.7 I0.6 I0.5 I0.4 I0.3 I0.2 I0.1 I0.0
	O0.7 O0.6 O0.5 O0.4 O0.3 O0.2 O0.1 O0.0
	N0.7 N0.6 N0.5 N0.4 N0.3 N0.2 N0.1 N0.0

four register pairs

	0		read port0				
	1		read port1
	2		output port 0
	3		output port 1
	4		Polarity Inversion port 0
	5		Polarity Inversion port 1
	6		Configuration port 0		1(default) input or 0 output
	7		Configuration port 1
	
The Polarity Inversion Register (register 2) allows polarity inversion of pins defined as inputs by the Configuration
Register. If a bit in this register is set (written with 1), the polarity of the corresponding port pin is inverted
in this register is cleared (written with a 0), the original polarity of the corresponding port pin is retained
*/


#ifndef _PCA95XX_H
#define _PCA95XX_H

#include "bsp_mcu_softi2c.h"

#define PCA95xx_REG_INPUT_P0                  0x00
#define PCA95xx_REG_INPUT_P1                  0x01
#define PCA95xx_REG_OUTPUT_P0                 0x02
#define PCA95xx_REG_OUTPUT_P1                 0x03
#define PCA95xx_REG_POLARITY_P0               0x04
#define PCA95xx_REG_POLARITY_P1               0x05
#define PCA95xx_REG_CONFIG_P0                 0x06
#define PCA95xx_REG_CONFIG_P1                 0x07

typedef struct 
{
	IIC_GPIO	pins;
	uint8_t devAddress;	//volatile
	uint8_t in_data;	
	uint8_t invertFlag;//所有IO都可以取反
	uint8_t out_data;	
	
}pca95xx_t;

extern uint8_t pca95xx_devAddR[4];

extern uint8_t pca95xx_init(pca95xx_t *dev);

#endif
