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
#ifndef __TMC429_H
#define __TMC429_H

#include "tmc429_interface.h"

#define MOTOR0 0
#define MOTOR1 0x20
#define MOTOR2 0x40

#define IDX_XTARGET 0x00
#define IDX_XACTUAL 0x02
#define IDX_VMIN 0x04
#define IDX_VMAX 0x06
#define IDX_VTARGET 0x08
#define IDX_VACTUAL 0x0A
#define IDX_AMAX 0x0C
#define IDX_AACTUAL 0x0E
#define IDX_AGTAT_ALEAT 0x10
#define IDX_PMUL_PDIV 0x12
#define IDX_REFCONF_RM 0x14
#define IDX_IMASK_IFLAGS 0x16
#define IDX_PULSEDIV_RAMPDIV 0x18
#define IDX_DX_REFTOLERANCE 0x1A
#define IDX_XLATCHED 0x1C
#define IDX_USTEP_COUNT_429 0x1E

#define IDX_LOW_WORD 0x60
#define IDX_HIGH_WORD 0x62
#define IDX_COVER_POS_LEN 0x64
#define IDX_COVER_DATA 0x66

#define IDX_IF_CONFIG_429 0x68
#define IDX_POS_COMP_429 0x6A
#define IDX_POS_COMP_INT_429 0x6C
#define IDX_TYPE_VERSION_429 0x72

#define IDX_REF_SWITCHES 0x7c
#define IDX_SMGP 0x7e

#define TMC429_READ 0x01

#define RM_RAMP 0
#define RM_SOFT 1
#define RM_VELOCITY 2
#define RM_HOLD 3

#define NO_REF 	 									 0x03
#define HARD_REF 									 0x00 //Ӳ����λ
#define SOFT_REF 								   0x04	//������λ

#define NO_LIMIT 0
#define HARD_LIMIT 1
#define SOFT_LIMIT 2

#define REFSW_LEFT 0x02
#define REFSW_RIGHT 0x01

#define M0_POS_REACHED 0x01
#define M1_POS_REACHED 0x04
#define M2_POS_REACHED 0x10
#define TMC429_STATUS_CDGW 0x40
#define TMC429_STATUS_INT  0x80

#define IFLAG_POS_REACHED     0x01
#define IFLAG_REF_WRONG       0x02
#define IFLAG_REF_MISS        0x04
#define IFLAG_STOP            0x08
#define IFLAG_STOP_LEFT_LOW   0x10
#define IFLAG_STOP_RIGHT_LOW  0x20
#define IFLAG_STOP_LEFT_HIGH  0x40
#define IFLAG_STOP_RIGHT_HIGH 0x80

#define IFCONF_INV_REF        0x0001
#define IFCONF_SDO_INT        0x0002
#define IFCONF_STEP_HALF      0x0004
#define IFCONF_INV_STEP       0x0008
#define IFCONF_INV_DIR        0x0010
#define IFCONF_EN_SD          0x0020
#define IFCONF_POS_COMP_0     0x0000
#define IFCONF_POS_COMP_1     0x0040
#define IFCONF_POS_COMP_2     0x0080
#define IFCONF_POS_COMP_OFF   0x00C0
#define IFCONF_EN_REFR        0x0100

extern void Init429(UCHAR Which429);

void ReadWrite429(UCHAR Which429, UCHAR *Read, UCHAR *Write);
void Write429Zero(UCHAR Which429, UCHAR Address);
void Write429Bytes(UCHAR Which429, UCHAR Address, UCHAR *Bytes);
void Write429Datagram(UCHAR Which429, UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte);
void Write429Short(UCHAR Which429, UCHAR Address, int Value);
void Write429Int(UCHAR Which429, UCHAR Address, int Value);
UCHAR Read429Status(UCHAR Which429 );
UCHAR Read429Bytes(UCHAR Which429, UCHAR Address, UCHAR *Bytes);
UCHAR Read429SingleByte(UCHAR Which429, UCHAR Address, UCHAR Index);
int Read429Short(UCHAR Which429, UCHAR Address);
int Read429Int(UCHAR Which429, UCHAR Address);
void Set429RampMode(UCHAR Which429, UCHAR Axis, UCHAR RampMode);
void Set429SwitchMode(UCHAR Which429, UCHAR Axis, UCHAR SwitchMode);
UCHAR SetAMax(UCHAR Which429, UCHAR Motor, UINT AMax);
void HardStop(UCHAR Which429, UINT Motor);




#endif

