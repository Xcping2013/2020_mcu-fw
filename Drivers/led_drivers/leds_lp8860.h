#ifndef __LEDS_LP8860_H
#define __LEDS_LP8860_H

/*	Features
Current Matching 0.5% (typical)
LED String Current up to 150 mA per Channel
Dimming Ratio > 13 000:1 With External PWM Brightness Control
16-bit Dimming Control with SPI or I2C

Display Mode:Global Dimming
	Maximum current per string is set by RISET;
	alternatively, through a user-programmable EEPROM value
	Brightness is controlled with PWM input or I2C/SPI register writes
	PWM output frequency is set with EEPROM registers from 4.9 kHz to 39 kHz
	the higher the frequency, the lower the resolution
Cluster Mode:Independent Dimming
	Cluster mode does not support the PWM input pin, hybrid dimming, slope control or dither mode

Programmable Switching Frequency
*/

#include "bsp_mcu_softI2c.h"


void lp8860_current_set(uint8_t current_ch1, uint8_t current_ch2, uint8_t current_ch3, uint8_t current_ch4);












#endif
