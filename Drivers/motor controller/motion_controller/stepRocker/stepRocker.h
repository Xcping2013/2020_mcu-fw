/*******************************************************************************
  Project: stepRocker Mini-TMCL

  Module:  stepRocker.h
           Definitions of globally used data types and macros

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
  \file stepRocker.h
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief Basic type and macro definitions

  This file contains basic type and macro definitions needed by all
  modules of this project.
*/
#ifndef __STEP_ROCKER_H
#define __STEP_ROCKER_H

#include "bsp_defines.h"

#define SW_TYPE_HIGH 0x04                      //!< module number (1110) high byte
#define SW_TYPE_LOW  0x56                      //!< module number (1110) low byte

#define SW_VERSION_HIGH 0x01                   //!< software version high byte
#define SW_VERSION_LOW  0x00                   //!< software version low byte

#define N_O_MOTORS 6                           //!< number of motors supported by this module
#define MOTOR_NUMBER(a) (a)                    //!< extract TMC429 motor number (for future extensions)
#define WHICH_262(a) (a)                       //!< extract TMC262 motor number (for future extensions)

#define DISABLE_DRIVERS() 	
#define ENABLE_DRIVERS()  	

#define LED1_ON()       	
#define LED1_OFF()      	
#define LED1_TOGGLE()   	

#define LED2_ON()      	
#define LED2_OFF()     	
#define LED2_TOGGLE()   	

#define SPI2_DEV0_TMC429  0                      //!< SPI device number of TMC429
#define SPI1_DEV1_TMC429  1                      //!< SPI device number of TMC429

#define SPI_DEV_TMC262  2                      //!< SPI device number of TMC262
#define SPI_DEV_EEPROM  3                      //!< SPI device number of the EEPROM


//! Global module settings (here only RS485 settings)
typedef struct
{
  UCHAR SerialBitrate;         //!< RS485 baud rate (0..7, 0=9600bps)
  UCHAR SerialModuleAddress;   //!< RS485 TMCL module address
  UCHAR SerialHostAddress;     //!< RS485 TMCL reply address
} TModuleConfig;

//! Motor configuration data
typedef struct
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)
	
	UCHAR SpeedChangedFlag;
	
  UCHAR IRun;                  //!< run current (0..255)
  UCHAR IStandby;              //!< stand-by current(0..255)
  UINT StallVMin;              //!< minimum speed for stallGuard
  UINT FreewheelingDelay;      //!< freewheeling delay time (*10ms)
  UINT SettingDelay;           //!< delay for switching to stand-by current (*10ms)
} TMotorConfig;

//! coolStep configuration data
typedef struct
{
  UINT ThresholdSpeed;         //!< coolStep threshold speed
  UCHAR SlowRunCurrent;        //!< coolStep slow run current
  UCHAR HysteresisStart;       //!< coolStep hysteresis start
} TCoolStepConfig;

/********************************************************************/
//				User
/********************************************************************/
typedef struct 
{
	uint8_t Homed;
	int HomeSpeed;
	uint8_t GoHome;
	uint8_t GoLimit;
	uint8_t homeSensorPin;

}THomeInfo;

#endif






