/*******************************************************************************
  Project: stepRocker Mini-TMCL

  Module:  TMC429.c
           TMC429 library

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
  \file TMC429.c
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC429 Stepper controller functions

  This file provides all functions needed for easy
  access to the TMC429 motion control IC.
*/

#include "TMC429.h"

/***************************************************************//**
   \fn ReadWrite429(uint8_t *Read, uint8_t *Write)
   \brief 32 bit SPI communication with TMC429
   \param Read   four byte array holding the data read from the TMC428
   \param Write  four byte array holding the data to write to the TMC428

   This is the low-level function that does all SPI communication with
   the TMC429. It sends a 32 bit SPI telegramme to the TMC429 and
   receives the 32 bit answer telegramme from the TMC429.
   It also raises the motor current when there is write access to
   a register that could cause motor movement.
********************************************************************/
void ReadWrite429(struct tmc429_dev *dev, uint8_t *Read, uint8_t *Write)
{
	struct rt_spi_message msg1;

	msg1.send_buf   = Write;
	msg1.recv_buf   = Read;
	msg1.length     = 4;
	msg1.cs_take    = 1;
	msg1.cs_release = 1;
	msg1.next       = RT_NULL;

	rt_spi_transfer_message(dev->spi_dev_tmc429, &msg1);

}


/***************************************************************//**
   \fn Write429Bytes(uint8_t Address, uint8_t *Bytes)
   \brief TMC429 write access
   \param Address  TMC429 register address (see also TMC429.h)
   \param Bytes  Array holding three bytes to be written to the
                 TMC429 register.

   This function writes an array of  three bytes to a TMC429 register.
********************************************************************/
void Write429Bytes(struct tmc429_dev *dev, uint8_t Address, uint8_t *Bytes)
{
  uint8_t Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Bytes[0];
  Write[2]=Bytes[1];
  Write[3]=Bytes[2];
  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Write429Datagram(uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
   \brief TMC429 write access
   \param Address   TMC429 register address
   \param HighByte  MSB of the TMC429 register
   \param MidByte   mid byte of the TMC429 register
   \param LowByte   LSB of the TMC429 register

   This function write three bytes to a TMC429 register.
********************************************************************/
void Write429Datagram(struct tmc429_dev *dev, uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
{
  uint8_t Write[4], Read[4];

  Write[0]=Address;
  Write[1]=HighByte;
  Write[2]=MidByte;
  Write[3]=LowByte;
  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Write429Zero(uint8_t Address)
   \brief Write zero to a TMC429 register
   \param Address  TMC429 register address

   This function sets a TMC429 register to zero. This can be useful
   e.g. to stop a motor quickly.
********************************************************************/
void Write429Zero(struct tmc429_dev *dev, uint8_t Address)
{
  uint8_t Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=0;
  Write[3]=0;

  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Write429Short(uint8_t Address, int Value)
   \brief Write 16 bit value to a TMC429 register
   \param Address  TMC429 register address
   \param Value    Value to be written

   This function writes a 16 bit value to a TMC429 register.
********************************************************************/
void Write429Short(struct tmc429_dev *dev, uint8_t Address, int Value)
{
  uint8_t Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Write429Int(uint8_t Address, int Value)
   \brief Write 24 bit value to a TMC429 register
   \param Address  TMC429 register address
   \param Value    Value to be written

   This function writes a 24 bit value to a TMC429 register.
********************************************************************/
void Write429Int(struct tmc429_dev *dev, uint8_t Address, int Value)
{
  uint8_t Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Value >> 16;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Read429Status
   \brief Read TMC429 status byte

   \return TMC429 status byte

   This functions reads just the status byte of the TMC429 using
   a single byte SPI access which makes this a little bit faster.
********************************************************************/
uint8_t Read429Status(struct tmc429_dev *dev)
{
	uint8_t sendbuf=0x01;
	uint8_t recvbuf=0;
	rt_spi_send_then_recv(dev->spi_dev_tmc429,&sendbuf,1,&recvbuf,1);
  return recvbuf;
}


/***************************************************************//**
   \fn Read429Bytes(uint8_t Address, uint8_t *Bytes)
   \brief Read TMC429 register
   \param Address  TMC429 register address (see TMC429.h)
   \param Bytes  Pointer at array of three bytes

   \return TMC429 status byte

   This functions reads a TMC429 register and puts the result into
   an array of bytes. It also returns the TMC429 status bytes.
********************************************************************/
uint8_t Read429Bytes(struct tmc429_dev *dev, uint8_t Address, uint8_t *Bytes)
{
  uint8_t Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(dev, Read, Write);

  Bytes[0]=Read[1];
  Bytes[1]=Read[2];
  Bytes[2]=Read[3];

  return Read[0];
}


/***************************************************************//**
   \fn Read429SingleByte(uint8_t Address, uint8_t Index)
   \brief Read TMC429 register
   \param Address  TMC429 register address (see TMC429.h)
   \param Index  TMC429 register byte to be returned (0..3)

   \return TM429 register byte

   This functions reads a TMC429 register and returns the desired
   byte of that register.
********************************************************************/
uint8_t Read429SingleByte(struct tmc429_dev *dev, uint8_t Address, uint8_t Index)
{
  uint8_t Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(dev, Read, Write);

//  return Read[Index+1];
	return Read[Index];
}


/***************************************************************//**
   \fn Read429Short(uint8_t Address)
   \brief Read TMC429 register (12 bit)
   \param Address  TMC429 register address (see TMC429.h)

   \return TMC429 register value (sign extended)

   This functions reads a TMC429 12 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read429Short(struct tmc429_dev *dev, uint8_t Address)
{
  uint8_t Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(dev, Read, Write);

  Result=(Read[2]<<8)|(Read[3]);
  if(Result & 0x00000800) Result|=0xfffff000;

  return Result;
}


/***************************************************************//**
   \fn Read429Int(uint8_t Address)
   \brief Read TMC429 register (24 bit)
   \param Address  TMC429 register address (see TMC429.h)

   \return TMC429 register value (sign extended)

   This functions reads a TMC429 24 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read429Int(struct tmc429_dev *dev, uint8_t Address)
{
  uint8_t Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(dev, Read, Write);

  Result=(Read[1]<<16)|(Read[2]<<8)|(Read[3]);
  if(Result & 0x00800000) Result|=0xff000000;

  return Result;
}


/***************************************************************//**
   \fn Set429RampMode(uint8_t Axis, uint8_t RampMode)
   \brief Set the ramping mode of an axis
   \param  Axis  Motor number (0, 1 or 2)
   \param  RampMode  ramping mode (RM_RAMP/RM_SOFT/RM_VELOCITY/RM_HOLD)

   This functions changes the ramping mode of a motor in the TMC429.
   It is some TMC429 register bit twiddling.
********************************************************************/
void Set429RampMode(struct tmc429_dev *dev, uint8_t Axis, uint8_t RampMode)
{
  uint8_t Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC429_READ;
  ReadWrite429(dev, Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = Read[2];
  Write[3] = RampMode;
  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn Set429SwitchMode(uint8_t Axis, uint8_t SwitchMode)
   \brief Set the end switch mode
   \param  Axis  Motor number (0, 1 or 2)
   \param  SwitchMode  end switch mode

   This functions changes the end switch mode of a motor in the TMC429.
   It is some TMC429 register bit twiddling.
********************************************************************/
void Set429SwitchMode(struct tmc429_dev *dev, uint8_t Axis, uint8_t SwitchMode)
{
  uint8_t Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC429_READ;
  ReadWrite429(dev, Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = SwitchMode;
  Write[3] = Read[3];
  ReadWrite429(dev, Read, Write);
}


/***************************************************************//**
   \fn SetAMax(uint8_t Motor, uint32_t AMax)
   \brief Set the maximum acceleration
   \param Motor  motor number (0, 1, 2)
   \param AMax: maximum acceleration (1..2047)

   This function sets the maximum acceleration and also calculates
   the PMUL and PDIV value according to all other parameters
   (please see the TMC429 data sheet for more info about PMUL and PDIV
   values).
********************************************************************/
uint8_t SetAMax(struct tmc429_dev *dev, uint8_t Motor, uint32_t AMax)
{
  int pdiv, pmul, pm, pd;
  float p, p_reduced;
  int ramp_div;
  int pulse_div;
  uint8_t PulseRampDiv;
  uint8_t Data[3];

  AMax&=0x000007ff;
  Read429Bytes(dev, IDX_PULSEDIV_RAMPDIV|MOTOR_NUMBER(Motor)<<5, Data);
  PulseRampDiv=Data[1];
  pulse_div=PulseRampDiv>>4;
  ramp_div=PulseRampDiv & 0x0f;

  pm=-1; pd=-1; // -1 indicates : no valid pair found

  if(ramp_div>=pulse_div)
    p = AMax / ( 128.0 * (1<<(ramp_div-pulse_div)));  //Exponent positive or 0
  else
    p = AMax / ( 128.0 / (1<<(pulse_div-ramp_div)));  //Exponent negative

  p_reduced = p*0.988;

  for (pdiv=0; pdiv<=13; pdiv++)
  {
    pmul = (int)(p_reduced * 8.0 * (1<<pdiv)) - 128;

    if ( (0 <= pmul) && (pmul <= 127) )
    {
      pm = pmul + 128;
      pd = pdiv;
    }
  }

  Data[0]=0;
  Data[1]=(uint8_t) pm;
  Data[2]=(uint8_t) pd;
  Write429Bytes(dev, (MOTOR_NUMBER(Motor)<<5)|IDX_PMUL_PDIV, Data);
  Write429Short(dev, (MOTOR_NUMBER(Motor)<<5)|IDX_AMAX, AMax);

  return 0;
}


/***************************************************************//**
   \fn HardStop(uint32_t Motor)
   \brief Stop a motor immediately
   \param Motor: motor number (0, 1, 2)

   This function stops a motor immediately (hard stop) by switching
   to velocity mode and then zeroing the V_TARGT and V_ACTUAL registers
   of that axis.
********************************************************************/
void HardStop(struct tmc429_dev *dev, uint32_t Motor)
{
  Set429RampMode(dev, MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero(dev, (MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
  Write429Zero(dev, (MOTOR_NUMBER(Motor)<<5)|IDX_VACTUAL);
}


/***************************************************************//**
   \fn Init429
   \brief TMC429 initialization

   This function does all necessary initializations of the TMC429.
********************************************************************/
void tmc429_init(struct tmc429_dev *dev )
{
  uint32_t addr;
  uint8_t Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)		 Write429Zero(dev, addr|(Motor<<5));
  }

  Write429Int(dev, IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT);
  Write429Datagram(dev, IDX_SMGP, 0x00, 0x00, 0x02);
	
	Write429Datagram(dev, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (dev->settings[0].PulseDiv<<4)|(dev->settings[0].RampDiv & 0x0f), 0x06);
	Write429Datagram(dev, IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (dev->settings[1].PulseDiv<<4)|(dev->settings[1].RampDiv & 0x0f), 0x06);
	Write429Datagram(dev, IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (dev->settings[2].PulseDiv<<4)|(dev->settings[2].RampDiv & 0x0f), 0x06);
	Write429Datagram(dev, IDX_REFCONF_RM|MOTOR0, 0x00, NO_REF, 0x00);
	Write429Datagram(dev, IDX_REFCONF_RM|MOTOR1, 0x00, NO_REF, 0x00);
	Write429Datagram(dev, IDX_REFCONF_RM|MOTOR2, 0x00, NO_REF, 0x00);


  Write429Short(dev, IDX_VMIN|MOTOR0, 1);
  Write429Short(dev, IDX_VMIN|MOTOR1, 1);
  Write429Short(dev, IDX_VMIN|MOTOR2, 1);

	Write429Int(dev, IDX_VMAX|MOTOR0, dev->settings[0].VMax);
	SetAMax(dev, 0, dev->settings[0].AMax);
	Write429Int(dev, IDX_VMAX|MOTOR1, dev->settings[1].VMax);
	SetAMax(dev, 1, dev->settings[1].AMax);
	Write429Int(dev, IDX_VMAX|MOTOR2, dev->settings[2].VMax);
}

int tmc429_spi_init(struct tmc429_dev *dev)
{
  rt_hw_spi_device_attach(dev->spi_name, dev->dev_name, dev->cs_gpio, dev->cs_gpio_pin);
	
	dev->spi_dev_tmc429 = (struct rt_spi_device *)rt_device_find(dev->dev_name);
	
 if (!dev->spi_dev_tmc429)
	{
			rt_kprintf("rt_hw_spi_tmc429_init failed! can't find %s device!\n", dev->dev_name);
	}
	else 
	{
		rt_kprintf("rt_device_find ok\n");
		
		struct rt_spi_configuration cfg;

		cfg.data_width = 8;
		cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
		cfg.max_hz = 12 * 100 *1000;                           /* 1.2M */

		rt_spi_configure(dev->spi_dev_tmc429, &cfg);

		tmc429_init(dev);
		
		rt_kprintf("rt_hw_spi_tmc429_init ok\n");	
		
	}
   return RT_EOK;
}
