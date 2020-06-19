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

#include "tmc429_defs.h"

void ReadWrite429(struct tmc429_dev *dev, uint8_t *Read, uint8_t *Write);
void Write429Zero(struct tmc429_dev *dev, uint8_t Address);
void Write429Bytes(struct tmc429_dev *dev, uint8_t Address, uint8_t *Bytes);
void Write429Datagram(struct tmc429_dev *dev, uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte);
void Write429Short(struct tmc429_dev *dev, uint8_t Address, int Value);
void Write429Int(struct tmc429_dev *dev, uint8_t Address, int Value);
uint8_t Read429Status(struct tmc429_dev *dev );
uint8_t Read429Bytes(struct tmc429_dev *dev, uint8_t Address, uint8_t *Bytes);
uint8_t Read429SingleByte(struct tmc429_dev *dev, uint8_t Address, uint8_t Index);
int Read429Short(struct tmc429_dev *dev, uint8_t Address);
int Read429Int(struct tmc429_dev *dev, uint8_t Address);
void Set429RampMode(struct tmc429_dev *dev, uint8_t Axis, uint8_t RampMode);
void Set429SwitchMode(struct tmc429_dev *dev, uint8_t Axis, uint8_t SwitchMode);
uint8_t SetAMax(struct tmc429_dev *dev, uint8_t Motor, uint32_t AMax);
void HardStop(struct tmc429_dev *dev, uint32_t Motor);

void tmc429_init(struct tmc429_dev *dev);

int tmc429_spi_init(struct tmc429_dev *dev);

#endif


