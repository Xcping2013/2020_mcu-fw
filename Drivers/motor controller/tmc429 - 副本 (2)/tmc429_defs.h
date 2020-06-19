/*******************************************************************************
  Project: stepRocker Mini-TMCL

  Module:  TMC429.h
           Defintions for TMC429 register addresses and TMC429 support functions.

   Copyright (C) 2011 TRINAMIC Motion Control GmbH & Co KG
                      Waterloohain 5
                      D - 22769 Hamburg, Germany
                      http://www.trinamic.com/

   This program is free software; you can redistribute it and/or modify it
   freely.

   This program is distributed "as is" in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

/**
  \file TMC429.h
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC429 library definitions

  This file contains all macro and function definitions of the TMC429
  library.
*/
#ifndef __TMC429_DEFS_H
#define __TMC429_DEFS_H

#include "bsp_defines.h"
#include <rtdevice.h>
#include "drv_spi.h"
#include <drivers/spi.h>

#define MOTOR_NUMBER(a) (a) 

#define MOTOR0 											0
#define MOTOR1 											0x20
#define MOTOR2 											0x40

#define IDX_XTARGET 								0x00
#define IDX_XACTUAL 								0x02
#define IDX_VMIN 										0x04
#define IDX_VMAX 										0x06
#define IDX_VTARGET 								0x08
#define IDX_VACTUAL 								0x0A
#define IDX_AMAX 										0x0C
#define IDX_AACTUAL 								0x0E
#define IDX_AGTAT_ALEAT				 	    0x10
#define IDX_PMUL_PDIV 							0x12
#define IDX_REFCONF_RM 							0x14
#define IDX_IMASK_IFLAGS 						0x16
#define IDX_PULSEDIV_RAMPDIV 				0x18
#define IDX_DX_REFTOLERANCE 				0x1A
#define IDX_XLATCHED							  0x1C
#define IDX_USTEP_COUNT_429 				0x1E

#define IDX_LOW_WORD 								0x60
#define IDX_HIGH_WORD 							0x62
#define IDX_COVER_POS_LEN 					0x64
#define IDX_COVER_DATA 							0x66

#define IDX_IF_CONFIG_429 					0x68
#define IDX_POS_COMP_429 						0x6A
#define IDX_POS_COMP_INT_429 				0x6C
#define IDX_TYPE_VERSION_429 				0x72

#define IDX_REF_SWITCHES 						0x7c
#define IDX_SMGP 										0x7e

#define TMC429_READ 								0x01

#define RM_RAMP 										0
#define RM_SOFT 										1
#define RM_VELOCITY 								2
#define RM_HOLD 										3

#define NO_REF 											0x03
#define SOFT_REF 										0x04

#define NO_LIMIT 										0
#define HARD_LIMIT 									1
#define SOFT_LIMIT 									2

#define REFSW_LEFT 									0x02
#define REFSW_RIGHT 								0x01

#define M0_POS_REACHED 							0x01
#define M1_POS_REACHED 							0x04
#define M2_POS_REACHED						  0x10
#define TMC429_STATUS_CDGW 					0x40
#define TMC429_STATUS_INT  					0x80

#define IFLAG_POS_REACHED     			0x01
#define IFLAG_REF_WRONG       			0x02
#define IFLAG_REF_MISS        			0x04
#define IFLAG_STOP            			0x08
#define IFLAG_STOP_LEFT_LOW   			0x10
#define IFLAG_STOP_RIGHT_LOW  			0x20
#define IFLAG_STOP_LEFT_HIGH  			0x40
#define IFLAG_STOP_RIGHT_HIGH 			0x80

#define IFCONF_INV_REF        			0x0001
#define IFCONF_SDO_INT        			0x0002
#define IFCONF_STEP_HALF      			0x0004
#define IFCONF_INV_STEP       			0x0008
#define IFCONF_INV_DIR        			0x0010
#define IFCONF_EN_SD          			0x0020
#define IFCONF_POS_COMP_0     			0x0000
#define IFCONF_POS_COMP_1     			0x0040
#define IFCONF_POS_COMP_2     			0x0080
#define IFCONF_POS_COMP_OFF   			0x00C0
#define IFCONF_EN_REFR        			0x0100



typedef int8_t (*tmc429_com_fptr_t)(uint8_t reg_addr, uint8_t *data, uint16_t len);

struct tmc429_settings
{
    /*! maximum positioning velocity */
    uint32_t VMax;
    /*! maximum acceleration */
    uint32_t AMax;
    /*! pulse divisor */
    uint8_t PulseDiv;
    /*! ramp divisor */
    uint8_t RampDiv;
	
		uint8_t SpeedChangedFlag;
};
struct homeInfo_t
{
	uint8_t	org_gpio_pin;
	uint8_t org_actLv;
	int			homeSpeed;
	
	uint8_t goHome;
	uint8_t goLimit;
	uint8_t homed;
};

struct tmc429_dev
{
	GPIO_TypeDef* cs_gpio;
	uint16_t 			cs_gpio_pin;
	
	const char* spi_name;
	const char* dev_name;
	
	struct tmc429_settings settings[3];
	
	struct homeInfo_t HomeInfo[3];
	
	struct rt_spi_device *spi_dev_tmc429;
};

#endif


