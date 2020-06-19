/*******************************************************************************
  Project: stepRocker Mini-TMCL

  Module:  IO.c
           I/O routines and some other useful stuff

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
  \file IO.c
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief I/O functions

  This file provides functions for intializing and
  using the I/O ports and some other miscellaneous
  stuff.
*/


#include <stdlib.h>
#include "bsp_defines.h"
#include "stepRocker.h"
#include "TMC262.h"
#include "cube_gpio.h"

/*********************************************//**
  \fn InitIO(void)
  \brief Initialize I/O

  This function initalizes all I/O port pins of
  the CPU that are not initialized in somewhere
  else in other initialization functions.
*************************************************/
void InitIO(void)
{
	MX_GPIO_Init();
}


/***************************************************************//**
   \fn EnableInterrupts(void)
   \brief Enable all interrupts

  This function globally enables all interrupts.
********************************************************************/
void EnableInterrupts(void)
{
 
}


/***************************************************************//**
   \fn DisableInterrupts(void)
   \brief Disable all interrupts

  This function globally disables all interrupts.
********************************************************************/
void DisableInterrupts(void)
{
  
}


/***************************************************************//**
   \fn SetMotorCurrent(UCHAR Motor, UCHAR Current)
   \brief Set motor current
   \param Motor    Motor number (with stepRocker always 0)
   \param Current  Motor current scaled to 0..255

   This function sets the motor current, scaled to 0..255 as
   usual in TMCL.
********************************************************************/
void SetMotorCurrent(UCHAR Motor, UCHAR Current)
{
  Set262StallGuardCurrentScale(WHICH_262(Motor), Current>>3);
}


/***************************************************************//**
   \fn GetStallState(UCHAR Motor)
   \brief Get stall flag state of motor driver
   \param Motor  Motor number (with stepRocker always 0)
   \return  TRUE stall\n
            FALSE no stall

   This function reads the state of the SG_TEST pin of the TMC262.
********************************************************************/
UCHAR GetStallState(UCHAR Motor)
{
  if(Motor==0)
  {
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1))
      return TRUE;
    else
      return FALSE;
  }
  else return FALSE;
}
