/*	

TCA6424A 
					低电压24位I2C和系统管理总线 (SMBus) 输入输出 (I/O) 扩展器具有中断输出、 复位和配置寄存器
					双向电压电平转换
					低电平有效复位输入 (RESET)

1.VDD range of 1.65 V to 5.5 V
2.open-drain interrupt (INT)   
	indicate that an input state has changed
	if there is incoming data on its ports without having to communicate via the I2C-bus
3.Power-up with all channels configured as inputs

*/
/*
	Device address
	
	slave address
	0 1 0 0 0 1 A	R/W
		
		W:0x44
		R:0x45	
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


#ifndef _TCA6424A_H
#define _TCA6424A_H

#include "bsp_mcu_softi2c.h"

#define INPUT_P0  0x00
#define INPUT_P1  0x01
#define INPUT_P2  0x02

#define OUTPUT_P0  0x04
#define OUTPUT_P1  0x05
#define OUTPUT_P2  0x06

/*
The Polarity Inversion registers (registers 8, 9 and 10) allow polarity inversion of pins defined as inputs by the Configuration register. 
If a bit in these registers is set (written with 1), the corresponding port pin's polarity is inverted.
If a bit in these registers is cleared (written with a 0), the corresponding port pin's original polarity is retained.

*/
#define POLINV_P0  0x08
#define POLINV_P1  0x09
#define POLINV_P2  0x0A

// If a bit in these registers is cleared to 0, the corresponding port pin is enabled as an output
#define CONFIG_P0  0x0C
#define CONFIG_P1  0x0D
#define CONFIG_P2  0x0E

#define AUTOINCR_ON 	0x80
#define AUTOINCR_OFF 	0x00

#define TCA6424A_DEFAULT_ADDRESS        0x44

// WRITE SEQUENCE (See page 20 of the datasheet for TCA2464)
// 1: Send the Slave Address (I2C Address of the IO Expansion Board)

// 2: Send the Command Byte

// 3: Send the Data
typedef struct 
{
	IIC_GPIO	pins;
	uint8_t devAddress;
	uint8_t in_data[3];	
	uint8_t out_data[3];	
	
}TCA6424A_t;

uint8_t TCA6424A_init(TCA6424A_t *dev);

#endif
