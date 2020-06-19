/*******************************************************************************
TMC4361_Arduino-master
*******************************************************************************/

/**
  \file TMC4361A.c
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC4361A Stepper controller functions

  This file provides all functions needed for easy
  access to the TMC4361A motion control IC.
*/

#include "TMC4361A.h"
#include "inc_mbkbcn1.h"	
#include "main.h"	

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define SPI1_DEV0_TMC4361A  0 

#define TMC4361_CS_PIN      6
#define TMC4361_CLK_PIN     5
#define TMC4361_RST_PIN     7
#define TMC4361_TARGET_PIN  18
#define TMC4361_START_PIN   17
#define TMC4361_INT_PIN     15
#define TMC4361_SCK_PIN     14
#define TMC4361_CLK_FREQ    8000000L

static int _defaultStepLength = 5; //us
static int _defaultDirSetupTime = 5; //us
static int _clockFreq = 16000000; //us

uint8_t _spiStatus=0;




#if 1			//MX_SPI1_Init	ReadWriteSPI

SPI_HandleTypeDef hspi1;

static void MX_SPI1_Init(void);
uint8_t ReadWriteSPI(uint8_t DeviceNumber, uint8_t Data, uint8_t LastTransfer);

static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;	//1.125MBits/S
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
uint8_t ReadWriteSPI(uint8_t DeviceNumber, uint8_t Data, uint8_t LastTransfer)
{
  uint8_t Result;
	
  switch(DeviceNumber)
  {
    case SPI1_DEV0_TMC4361A:
      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_RESET);
      break;
    default:
      return 0;
      break;
  }
	//数据收发
  switch(DeviceNumber)
  {
    case SPI1_DEV0_TMC4361A:				
			HAL_SPI_TransmitReceive(&hspi1, &Data, &Result, 1,1000);
			break;

    default:
      Result=0;
      break;
  }
  if(LastTransfer)
  {
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_SET);
  }
  return Result;
}

#endif 

void ReadWrite4361A(UCHAR *Read, UCHAR *Write)
{
  Read[0]=ReadWriteSPI(SPI1_DEV0_TMC4361A, Write[0], FALSE);
  Read[1]=ReadWriteSPI(SPI1_DEV0_TMC4361A, Write[1], FALSE);
  Read[2]=ReadWriteSPI(SPI1_DEV0_TMC4361A, Write[2], FALSE);
  Read[3]=ReadWriteSPI(SPI1_DEV0_TMC4361A, Write[3], FALSE);
	Read[4]=ReadWriteSPI(SPI1_DEV0_TMC4361A, Write[4], TRUE);
}
#if 0
long TMC4361_spiTransfer( byte address,  long data)
{
  long returnBuffer = 0;

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_RESET);
	
	HAL_SPI_TransmitReceive(&hspi1, &address, &_spiStatus, 1,1000);
	//Send data MSB first
  for (int i = 3; i >= 0; i--)
    returnBuffer |= (HAL_SPI_TransmitReceive((&hspi1,(data >> (i*8)) & 0xFF) << (i*8));

  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_SET);

  return returnBuffer;
}

long TMC4361::floatToFixedPoint(float value, int decimalPlaces)
{
  value *= (float)(1 << decimalPlaces);
  return (long)((value > 0.0) ? (value + 0.5) : (value - 0.5));
}

float TMC4361::fixedPointToFloat(long value, int decimalPlaces)
{
  return (float)(value) / (float)(1 << decimalPlaces);
}
#endif
//

#if 0    //copy from TMC429


/***************************************************************//**
   \fn ReadWrite4361A(UCHAR *Read, UCHAR *Write)
   \brief 40 bit SPI communication with TMC4361A
   \param Read   four byte array holding the data read from the TMC428
   \param Write  four byte array holding the data to write to the TMC428

   This is the low-level function that does all SPI communication with
   the TMC4361A. It sends a 32 bit SPI telegramme to the TMC4361A and
   receives the 32 bit answer telegramme from the TMC4361A.
   It also raises the motor current when there is write access to
   a register that could cause motor movement.
********************************************************************/



/***************************************************************//**
   \fn Write4361ABytes(UCHAR Address, UCHAR *Bytes)
   \brief TMC4361A write access
   \param Address  TMC4361A register address (see also TMC4361A.h)
   \param Bytes  Array holding three bytes to be written to the
                 TMC4361A register.

   This function writes an array of  three bytes to a TMC4361A register.
********************************************************************/
void Write4361ABytes(UCHAR Address, UCHAR *Bytes)
{
  UCHAR Write[5], Read[5];

  Write[0]=Address;
  Write[1]=Bytes[0];
  Write[2]=Bytes[1];
  Write[3]=Bytes[2];
	Write[4]=Bytes[3];
  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Write4361ADatagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte)
   \brief TMC4361A write access
   \param Address   TMC4361A register address
   \param HighByte  MSB of the TMC4361A register
   \param MidByte   mid byte of the TMC4361A register
   \param LowByte   LSB of the TMC4361A register

   This function write three bytes to a TMC4361A register.
********************************************************************/
void Write4361ADatagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte)
{
  UCHAR Write[5], Read[5];

  Write[0]=Address;
  Write[1]=HighByte;
  Write[2]=MidByte;
  Write[3]=LowByte;
  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Write4361AZero(UCHAR Address)
   \brief Write zero to a TMC4361A register
   \param Address  TMC4361A register address

   This function sets a TMC4361A register to zero. This can be useful
   e.g. to stop a motor quickly.
********************************************************************/
void Write4361AZero(UCHAR Address)
{
  UCHAR Write[5], Read[5];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=0;
  Write[3]=0;
	Write[4]=0;
  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Write4361AShort(UCHAR Address, int Value)
   \brief Write 16 bit value to a TMC4361A register
   \param Address  TMC4361A register address
   \param Value    Value to be written

   This function writes a 16 bit value to a TMC4361A register.
********************************************************************/
void Write4361AShort(UCHAR Address, int Value)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Write4361AInt(UCHAR Address, int Value)
   \brief Write 24 bit value to a TMC4361A register
   \param Address  TMC4361A register address
   \param Value    Value to be written

   This function writes a 24 bit value to a TMC4361A register.
********************************************************************/
void Write4361AInt(UCHAR Address, int Value)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Value >> 16;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Read4361AStatus
   \brief Read TMC4361A status byte

   \return TMC4361A status byte

   This functions reads just the status byte of the TMC4361A using
   a single byte SPI access which makes this a little bit faster.
********************************************************************/
UCHAR Read4361AStatus(UCHAR Which4361A)
{
  return ReadWriteSPI(SPI1_DEV0_TMC4361A, 0x01, TRUE);
}


/***************************************************************//**
   \fn Read4361ABytes(UCHAR Address, UCHAR *Bytes)
   \brief Read TMC4361A register
   \param Address  TMC4361A register address (see TMC4361A.h)
   \param Bytes  Pointer at array of three bytes

   \return TMC4361A status byte

   This functions reads a TMC4361A register and puts the result into
   an array of bytes. It also returns the TMC4361A status bytes.
********************************************************************/
UCHAR Read4361ABytes(UCHAR Address, UCHAR *Bytes)
{
  UCHAR Read[4], Write[4];

  Write[0]=Address|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

  Bytes[0]=Read[1];
  Bytes[1]=Read[2];
  Bytes[2]=Read[3];

  return Read[0];
}


/***************************************************************//**
   \fn Read4361ASingleByte(UCHAR Address, UCHAR Index)
   \brief Read TMC4361A register
   \param Address  TMC4361A register address (see TMC4361A.h)
   \param Index  TMC4361A register byte to be returned (0..3)

   \return TM4361A register byte

   This functions reads a TMC4361A register and returns the desired
   byte of that register.
********************************************************************/
UCHAR Read4361ASingleByte(UCHAR Address, UCHAR Index)
{
  UCHAR Read[4], Write[4];

  Write[0]=Address|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

//  return Read[Index+1];
	return Read[Index];
}


/***************************************************************//**
   \fn Read4361AShort(UCHAR Address)
   \brief Read TMC4361A register (12 bit)
   \param Address  TMC4361A register address (see TMC4361A.h)

   \return TMC4361A register value (sign extended)

   This functions reads a TMC4361A 12 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read4361AShort(UCHAR Address)
{
  UCHAR Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

  Result=(Read[2]<<8)|(Read[3]);
  if(Result & 0x00000800) Result|=0xfffff000;

  return Result;
}


/***************************************************************//**
   \fn Read4361AInt(UCHAR Address)
   \brief Read TMC4361A register (24 bit)
   \param Address  TMC4361A register address (see TMC4361A.h)

   \return TMC4361A register value (sign extended)

   This functions reads a TMC4361A 24 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read4361AInt(UCHAR Address)
{
  UCHAR Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

  Result=(Read[1]<<16)|(Read[2]<<8)|(Read[3]);
  if(Result & 0x00800000) Result|=0xff000000;

  return Result;
}


/***************************************************************//**
   \fn Set4361ARampMode(UCHAR Axis, UCHAR RampMode)
   \brief Set the ramping mode of an axis
   \param  Axis  Motor number (0, 1 or 2)
   \param  RampMode  ramping mode (RM_RAMP/RM_SOFT/RM_VELOCITY/RM_HOLD)

   This functions changes the ramping mode of a motor in the TMC4361A.
   It is some TMC4361A register bit twiddling.
********************************************************************/
void Set4361ARampMode(UCHAR Axis, UCHAR RampMode)
{
  UCHAR Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = Read[2];
  Write[3] = RampMode;
  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn Set4361ASwitchMode(UCHAR Axis, UCHAR SwitchMode)
   \brief Set the end switch mode
   \param  Axis  Motor number (0, 1 or 2)
   \param  SwitchMode  end switch mode

   This functions changes the end switch mode of a motor in the TMC4361A.
   It is some TMC4361A register bit twiddling.
********************************************************************/
void Set4361ASwitchMode(UCHAR Axis, UCHAR SwitchMode)
{
  UCHAR Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC4361A_READ;
  ReadWrite4361A( Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = SwitchMode;
  Write[3] = Read[3];
  ReadWrite4361A( Read, Write);
}


/***************************************************************//**
   \fn SetAMax(UCHAR Motor, UINT AMax)
   \brief Set the maximum acceleration
   \param Motor  motor number (0, 1, 2)
   \param AMax: maximum acceleration (1..2047)

   This function sets the maximum acceleration and also calculates
   the PMUL and PDIV value according to all other parameters
   (please see the TMC4361A data sheet for more info about PMUL and PDIV
   values).
********************************************************************/
UCHAR SetAMax(UCHAR Motor, UINT AMax)
{
  int pdiv, pmul, pm, pd;
  float p, p_reduced;
  int ramp_div;
  int pulse_div;
  UCHAR PulseRampDiv;
  UCHAR Data[3];

  AMax&=0x000007ff;
  Read4361ABytes(IDX_PULSEDIV_RAMPDIV|MOTOR_NUMBER(Motor)<<5, Data);
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
  Data[1]=(UCHAR) pm;
  Data[2]=(UCHAR) pd;
  Write4361ABytes((MOTOR_NUMBER(Motor)<<5)|IDX_PMUL_PDIV, Data);
  Write4361AShort((MOTOR_NUMBER(Motor)<<5)|IDX_AMAX, AMax);

  return 0;
}


/***************************************************************//**
   \fn HardStop(UINT Motor)
   \brief Stop a motor immediately
   \param Motor: motor number (0, 1, 2)

   This function stops a motor immediately (hard stop) by switching
   to velocity mode and then zeroing the V_TARGT and V_ACTUAL registers
   of that axis.
********************************************************************/
void HardStop(UINT Motor)
{
  Set4361ARampMode( MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write4361AZero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
  Write4361AZero((MOTOR_NUMBER(Motor)<<5)|IDX_VACTUAL);
}


/***************************************************************//**
   \fn Init4361A
   \brief TMC4361A initialization

   This function does all necessary initializations of the TMC4361A.
********************************************************************/
#endif
//
#if 0
void Init4361A(UCHAR Which4361A )
{
  UINT addr;
  UCHAR Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write4361AZero(SPI1_DEV0_TMC4361A, addr|(Motor<<5));
  }

	Write4361AInt(SPI1_DEV0_TMC4361A, IDX_IF_CONFIG_4361A, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);

	Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_SMGP, 0x00, 0x04, 0x02);

	if(Which4361A==SPI_DEV0_TMC4361A)
	{
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
	}
	else if(Which4361A==SPI_DEV1_TMC4361A)
	{
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[3].PulseDiv<<4)|(MotorConfig[3].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[4].PulseDiv<<4)|(MotorConfig[4].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[5].PulseDiv<<4)|(MotorConfig[5].RampDiv & 0x0f), 0x04);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
		Write4361ADatagram(SPI1_DEV0_TMC4361A, IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
	}
  Write4361AShort(SPI1_DEV0_TMC4361A, IDX_VMIN|MOTOR0, 1);
  Write4361AShort(SPI1_DEV0_TMC4361A, IDX_VMIN|MOTOR1, 1);
  Write4361AShort(SPI1_DEV0_TMC4361A, IDX_VMIN|MOTOR2, 1);

	if(Which4361A==SPI_DEV0_TMC4361A)
	{
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR0, MotorConfig[0].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 0, MotorConfig[0].AMax);
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 1, MotorConfig[1].AMax);
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 2, MotorConfig[2].AMax);
	}
	else if(Which4361A==SPI_DEV1_TMC4361A)
	{
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR0, MotorConfig[3].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 0, MotorConfig[3].AMax);
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR1, MotorConfig[4].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 1, MotorConfig[4].AMax);
		Write4361AInt(SPI1_DEV0_TMC4361A, IDX_VMAX|MOTOR2, MotorConfig[5].VMax);
		SetAMax(SPI1_DEV0_TMC4361A, 2, MotorConfig[5].AMax);
	}
}
//
#endif
//
//TMC4361_reset
//
//TMC4361_readRegister							TMC4361_readRegisterBit
//
//TMC4361_writeRegister							TMC4361_setRegisterBit				TMC4361_clearRegisterBit
//
//TMC4361_floatToFixedPoint					TMC4361_fixedPointToFloat

//TMC4361_checkFlag									TMC4361_clearEvents						TMC4361_checkEvent
//
//TMC4361_setOutputsPolarity				TMC4361_setOutputTimings
//
//TMC4361_getCurrentPosition				TMC4361_setCurrentPosition		TMC4361_isTargetReached
//
//TMC4361_getCurrentSpeed						TMC4361_setMaxSpeed

//TMC4361_setRampSpeeds							TMC4361_setAccelerations			TMC4361_getCurrentAcceleration

//TMC4361_getTargetPosition					TMC4361_setTargetPosition			TMC4361_setRampMode									TMC4361_setBowValues
//
//TMC4361_stop
#if 1 
long TMC4361_readRegister( uint8_t address)
{
	UCHAR Write[5], Read[5];
	//下次读取的是上次寄存器地址数据
	for(uint8_t i=0;i<2;i++)
	{
		Write[0]=address & 0x7F;
		Write[1]=0;
		Write[2]=0;
		Write[3]=0;
		Write[4]=0;
		ReadWrite4361A( Read, Write);
  }
  
  return (Read[1]<<24 | Read[2]<<16 | Read[3]<<8 | Read[4]);
}

uint8_t TMC4361_readRegisterBit( uint8_t address,  uint8_t bit)
{
  return bitRead(TMC4361_readRegister(address), bit);
}

void TMC4361_writeRegister( uint8_t address,  long data)
{
	UCHAR Write[5], Read[5];

  Write[0]=address | 0x80;
  Write[1]=data>>24;
  Write[2]=data>>16;
  Write[3]=data>>8;
	Write[4]=data & 0xff;
  ReadWrite4361A( Read, Write);
}
void TMC4361_setRegisterBit( uint8_t address,  uint8_t bit)
{
  uint32_t value = TMC4361_readRegister(address);
  bitSet(value, bit);
  TMC4361_writeRegister(address, value);
}
void TMC4361_clearRegisterBit( uint8_t address,  uint8_t bit)
{
  uint32_t value = TMC4361_readRegister(address);
  bitClear(value, bit);
  TMC4361_writeRegister(address, value);
}
long TMC4361_floatToFixedPoint(float value, int decimalPlaces)
{
  value *= (float)(1 << decimalPlaces);
  return (long)((value > 0.0) ? (value + 0.5) : (value - 0.5));
}
float TMC4361_fixedPointToFloat(long value, int decimalPlaces)
{
  return (float)(value) / (float)(1 << decimalPlaces);
}
void TMC4361_reset(void)
{
	 TMC4361_writeRegister(TMC4361_RESET_CLK_GATING_REGISTER, 0x525354 << 8);
}
uint8_t TMC4361_checkFlag(enum FlagType flag)
{
  return TMC4361_readRegisterBit(TMC4361_STATUS_REGISTER, flag);
}
uint8_t TMC4361_isTargetReached(void)
{
  return TMC4361_checkFlag(TARGET_REACHED_F);
}
void TMC4361_clearEvents(void)
{
  TMC4361_writeRegister(TMC4361_EVENTS_REGISTER, 0xFFFFFFFF);
}
uint8_t TMC4361_checkEvent(enum EventType event)
{
  uint8_t value = TMC4361_readRegisterBit(TMC4361_EVENTS_REGISTER, event);
  
  if (value)
    TMC4361_writeRegister(TMC4361_EVENTS_REGISTER, 1 << event);
    
  return value;
}
void TMC4361_setOutputsPolarity(uint8_t stepInverted, uint8_t dirInverted)
{
  long generalConfigReg = TMC4361_readRegister(TMC4361_GENERAL_CONFIG_REGISTER);

  bitWrite(generalConfigReg, 3, stepInverted);
  bitWrite(generalConfigReg, 5, dirInverted);

  TMC4361_writeRegister(TMC4361_GENERAL_CONFIG_REGISTER, generalConfigReg);
}
void TMC4361_setOutputTimings(int stepWidth, int dirSetupTime)
{
  long registerValue =
    ((stepWidth * _clockFreq / 1000000L - 1) & 0xFFFF) |
    (((dirSetupTime * _clockFreq / 1000000L) & 0xFFFF) << 16);

  TMC4361_writeRegister(TMC4361_STP_LENGTH_ADD, registerValue);
}
void TMC4361_setRampMode(enum RampMode mode, enum RampType type)
{
  TMC4361_writeRegister(TMC4361_RAMP_MODE_REGISTER, mode | type);
}
long TMC4361_getCurrentPosition(void)
{
  return TMC4361_readRegister(TMC4361_X_ACTUAL_REGISTER);
}
void TMC4361_setCurrentPosition(long position)
{
  TMC4361_writeRegister(TMC4361_X_ACTUAL_REGISTER, position);
}
float TMC4361_getCurrentSpeed(void)
{
  return (float)TMC4361_readRegister(TMC4361_V_ACTUAL_REGISTER);
}
float TMC4361_getCurrentAcceleration(void)
{
  return (float)TMC4361_readRegister(TMC4361_A_ACTUAL_REGISTER);
}
void TMC4361_setMaxSpeed(float speed)
{
  TMC4361_writeRegister(TMC4361_V_MAX_REGISTER, TMC4361_floatToFixedPoint(speed, 8));
}
void TMC4361_setRampSpeeds(float startSpeed, float stopSpeed, float breakSpeed)
{
  TMC4361_writeRegister(TMC4361_V_START_REGISTER, TMC4361_floatToFixedPoint(abs(startSpeed), 8));
  TMC4361_writeRegister(TMC4361_V_STOP_REGISTER, TMC4361_floatToFixedPoint(abs(stopSpeed), 8));
  TMC4361_writeRegister(TMC4361_V_BREAK_REGISTER, TMC4361_floatToFixedPoint(abs(breakSpeed), 8));
}
void TMC4361_setAccelerations(float maxAccel, float maxDecel, float startAccel, float finalDecel)
{
  TMC4361_writeRegister(TMC4361_A_MAX_REGISTER, TMC4361_floatToFixedPoint(abs(maxAccel), 2) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_D_MAX_REGISTER, TMC4361_floatToFixedPoint(abs(maxDecel), 2) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_A_START_REGISTER, TMC4361_floatToFixedPoint(abs(startAccel), 2) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_D_FINAL_REGISTER, TMC4361_floatToFixedPoint(abs(finalDecel), 2) & 0xFFFFFF);
}
void TMC4361_setBowValues(long bow1, long bow2, long bow3, long bow4)
{
  TMC4361_writeRegister(TMC4361_BOW_1_REGISTER, abs(bow1) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_BOW_2_REGISTER, abs(bow2) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_BOW_3_REGISTER, abs(bow3) & 0xFFFFFF);
  TMC4361_writeRegister(TMC4361_BOW_4_REGISTER, abs(bow4) & 0xFFFFFF);
}
long TMC4361_getTargetPosition(void)
{
  return TMC4361_readRegister(TMC4361_X_TARGET_REGISTER);
}
void TMC4361_setTargetPosition(long position)
{
  TMC4361_writeRegister(TMC4361_X_TARGET_REGISTER, position);
}
void TMC4361_stop(void)
{
  TMC4361_setMaxSpeed(0.0);
}
#endif
//
#if 1 //tmc4361_init
void tmc4361_init(void)
{
	MX_SPI1_Init();
	pinMode(PB_11,PIN_MODE_OUTPUT);
	
	TMC4361_reset();
	TMC4361_writeRegister(TMC4361_CLK_FREQ_REGISTER, _clockFreq);
	TMC4361_setOutputTimings(_defaultStepLength, _defaultDirSetupTime);
	// Protect all events from automatic clearing when reading EVENTS register. 
  // This way individual events can be checked at the price of reading the register once per check.
	TMC4361_writeRegister(TMC4361_EVENT_CLEAR_CONF_REGISTER, 0xFFFFFFFF);
  TMC4361_clearEvents();

  //TODO init FREEZE register
//	TMC4361_setRampMode(POSITIONING_MODE, TRAPEZOIDAL_RAMP);
//	TMC4361_setMaxSpeed(400);
	
//	TMC4361_setAccelerations(25, 10, 50, 20);
//	MC4361_setRampSpeeds(20, 40, 150);
//	TMC4361_setAccelerations(50, 20, 0, 0);
//	TMC4361_setTargetPosition(1000);
	
//	TMC4361_setRampMode(POSITIONING_MODE, S_SHAPED_RAMP);
//	TMC4361_setMaxSpeed(200);
//	TMC4361_setAccelerations(50, 50, 0, 20);
//	TMC4361_etBowValues(10, 40, 20, 100);
//	TMC4361_setTargetPosition(1000);
}
static rt_uint8_t MotorStack[ 512 ];
static struct	rt_thread MotorThread;
int Motor_thread_init(void);
static void Motor_Thread_entry(void *parameter)
{
	tmc4361_init();
	rt_kprintf("TMC4361_init ok\n");
	while (1)
	{  
		
		//rt_kprintf("CurrentSpeed=%d\n",TMC4361_getCurrentSpeed);
		if (TMC4361_isTargetReached())
		{
			rt_kprintf("TMC4361_isTargetReached\n");
			if (TMC4361_getCurrentPosition() < 0)
				TMC4361_setTargetPosition(4000);
			else
				TMC4361_setTargetPosition(-4000);
		}	
		 rt_thread_mdelay(100);		
	}		
}
int Motor_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&MotorThread,
                            "Motor",
                            Motor_Thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&MotorStack[0],
                            sizeof(MotorStack),
                            10,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&MotorThread);
    }
    return 0;
}

void readReg(uint8_t address)
{
	UCHAR Write[5], Read[5];

	for(uint8_t i=0;i<2;i++)
	{
		Write[0]=address;// & 0x7F;
		Write[1]=0;
		Write[2]=0;
		Write[3]=0;
		Write[4]=0;
		ReadWrite4361A( Read, Write);
  }
	rt_kprintf("status=0x%x,",Read[0]);

	int readRegVal=(Read[1]<<24 | Read[2]<<16 | Read[3]<<8 | Read[4]);
	rt_kprintf("reg[%d]=0x%x\n",address,readRegVal);
}

#if 1 //motion
#define MVP_ABS   0            
#define MVP_REL   1 

typedef struct
{
  UINT 	VMax;                   //!< maximum positioning velocity
  UINT 	AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              	//!< pulse divisor (TMC429)
  UCHAR RampDiv;               	//!< ramp divisor (TMC429)
	UCHAR SpeedChangedFlag;
} TMotorConfig;
TMotorConfig MotorConfig[1]=
{
  400,   //!< VMax
  100,   //!< AMax
  7,      
  7,      
	1,
};
#endif

MSH_CMD_EXPORT(Motor_thread_init, ......);
FINSH_FUNCTION_EXPORT(readReg, ......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_readRegisterBit, readRegBit,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_writeRegister, writeReg,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_setRegisterBit, setRegBit,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_clearRegisterBit, cleartRegBit,......);

FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_getCurrentPosition, getPosition,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_setCurrentPosition, setPosition,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_isTargetReached, reach,......);

FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_getCurrentSpeed, getSpeed,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_setMaxSpeed, setSpeed,......);

FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_setAccelerations, setACC,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_getCurrentAcceleration, getACC,......);

FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_setRampMode, setMode,......);
FINSH_FUNCTION_EXPORT_ALIAS(TMC4361_stop, stop,......);





/*
RW 0x20 32 2:0   RAMPMODE	3
RW 0x21 33 31:0	 XACTUAL
RW 0x37 55 32 	 XTARGET

R  0x22 34 31:0  VACTUAL
RW 0x24 36 31:0	 VMAX 			速度值单位始终定义为每秒脉冲数[pps]	加速和减速单位默认定义为每平方秒的脉冲[pps2]



*/
#endif
//全局配置好，剩下的主要关键参数再逐一配置

#if 0 //TMC4361 MSH 先保证单轴OK 再完善其他轴

enum axisParameter {

XTARGET, 
position_actual, 
position_reached,	
	
speed_next, 				//int
speed_actual,				//int 
speed_max, 					//int
acc_max, 						//int

rightLimit_SwS,
leftLimit_SwS,
allLimit_SwS,
rightLimit_disable,	
leftLimit_disable,
	
ramp_mode,

ramp_divisor,
pulse_divisor,	

limitSignal,
home_SenS,

is_homed,
is_stop,
is_reach,
is_reset,

Type_none,

};

typedef struct
{           
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC4361_Parameter;
//
TMC4361_Parameter TMC4361_ParameterRW;

int motor(int argc, char **argv)
{
	int result = REPLY_OK;
	
	static uint8_t TMC4361_inited=0;
	
	if(TMC4361_inited==0)
	{
		TMC4361_inited=1;

		TMC4361_hw_init();						
	}		
	if		  (argc == 1 )  									{	printf_cmdList_motor();	}
	else if (argc == 2 )					
	{
		if 			 (!strcmp(argv[1], "set"))		{ printf_cmdList_motor_set();}
		else	if (!strcmp(argv[1], "get"))		{	printf_cmdList_motor_get();}
		else																	{ result = REPLY_INVALID_CMD;}
	}
	else if (argc == 3 )	
	{
		CMDGetFromUart.Motor=atoi(argv[2]);
		
		if (CMDGetFromUart.Motor > N_O_MOTORS) result = REPLY_INVALID_VALUE;
			
		else if(!strcmp(argv[1], "stop"))
		{
			StopMotorByHW(CMDGetFromUart.Motor);
		}
		else result = REPLY_INVALID_CMD;
	}
	else if (argc == 4)
	{	
		if (!strcmp(argv[1], "get"))
		{
			if      (!strcmp(argv[2], "speed")) 	 				CMDGetFromUart.Type=speed_actual;
			else if (!strcmp(argv[2], "position")) 				CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart.Type=is_homed;
			else if (!strcmp(argv[2], "is_stop")) 				CMDGetFromUart.Type=is_stop;
			else if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart.Type=is_reach;

			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;		
			else if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart.Type=speed_max;
			else if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=acc_max;
			else if (!strcmp(argv[2], "position_reached"))CMDGetFromUart.Type=position_reached;		
			
			else if (!strcmp(argv[2], "rightLimit")) 			CMDGetFromUart.Type=rightLimit_SwS;
			else if (!strcmp(argv[2], "leftLimit")) 			CMDGetFromUart.Type=leftLimit_SwS;
			else if (!strcmp(argv[2], "limit")) 			    CMDGetFromUart.Type=allLimit_SwS;
			
			else if (!strcmp(argv[2], "homeSensor")) 			CMDGetFromUart.Type=home_SenS;
			
			else if (!strcmp(argv[2], "rightLimit?"))			CMDGetFromUart.Type=rightLimit_disable;
			else if (!strcmp(argv[2], "leftLimit?")) 			CMDGetFromUart.Type=leftLimit_disable;
			
			else if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
			else if (!strcmp(argv[2], "pulse_div")) 			CMDGetFromUart.Type=pulse_divisor;	
				
			if(CMDGetFromUart.Type!=Type_none)
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				else TMC4361_GetAxisParameter(CMDGetFromUart.Motor,CMDGetFromUart.Type);	
			}
			else 
			{
				result=REPLY_WRONG_TYPE;
				printf_cmdList_motor_get();			
			}
		}		
		else 
		{
			CMDGetFromUart.Motor=atoi(argv[2]);
			CMDGetFromUart.Value.Int32=atol(argv[3]);	
													
			if (CMDGetFromUart.Motor >=N_O_MOTORS) result = REPLY_INVALID_VALUE;
			
			else if (!strcmp(argv[1], "rotate"))
			{						
				TMC4361_MotorRotate(CMDGetFromUart.Motor,ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32))	;
				
				rt_kprintf("motor%d is rotate at %d pulse per sec\n",CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);				
			}
			else if (!strcmp(argv[1], "move") )
			{			
				TMC4361_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32);
				rt_kprintf("motor[%d] is start to make relative motion\n",CMDGetFromUart.Motor);
			}
			else if (!strcmp(argv[1], "moveto"))
			{
				rt_kprintf("motor[%d] is start to make absolute motion\n",CMDGetFromUart.Motor);
				TMC4361_MoveToPosition(CMDGetFromUart.Motor, MVP_ABS, CMDGetFromUart.Value.Int32);
			}
			else if (!strcmp(argv[1], "gohome"))
			{
				rt_kprintf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);		
				motor_gohome_config(CMDGetFromUart.Motor, ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));		
			}
			else if (!strcmp(argv[1], "golimit"))
			{	
				rt_kprintf("motor[%d] is start go home by searching limit sensor\n",CMDGetFromUart.Motor);	
				motor_golimit_config(CMDGetFromUart.Motor,ChangeSpeed_msToVMax(CMDGetFromUart.Value.Int32));							
			}
			else result = REPLY_INVALID_CMD;
		}	
	}
	else if (argc == 5)
	{
		if (!strcmp(argv[1], "set"))
		{
			//user
			if (!strcmp(argv[2], "speed")) 					 		  CMDGetFromUart.Type=speed_max;
			//debug
			else if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=speed_next;		
			else if (!strcmp(argv[2], "next_position")) 	CMDGetFromUart.Type=position_next;
			else if (!strcmp(argv[2], "actual_position")) CMDGetFromUart.Type=position_actual;
			else if (!strcmp(argv[2], "speed_actual")) 	 	CMDGetFromUart.Type=speed_actual;
			else if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart.Type=speed_max;
			else if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=acc_max;
			
			else if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart.Type=rightLimit_disable;
			else if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_disable;	  		
			else if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart.Type=limitSignal;		
			else if (!strcmp(argv[2], "ramp_div")) 			  	CMDGetFromUart.Type=ramp_divisor;
			else if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;

			if(CMDGetFromUart.Type!=Type_none)	
			{
				CMDGetFromUart.Motor=atoi(argv[3]);
				CMDGetFromUart.Value.Int32=atoi(argv[4]);
				if (CMDGetFromUart.Motor >= N_O_MOTORS) result = REPLY_INVALID_VALUE;
				else TMC4361_SetAxisParameter(CMDGetFromUart.Motor, CMDGetFromUart.Type, CMDGetFromUart.Value.Int32);
			}
			else 
			{
				result =REPLY_WRONG_TYPE;				
				printf_cmdList_motor_set();													
			}				
		}
	}		
	if(	result == REPLY_INVALID_VALUE )
	{
		rt_kprintf("motor number must be 0~5\n");
	}
	else if( result == REPLY_INVALID_CMD )	{	printf_cmdList_motor();	}

	return result;
}




#endif
//





