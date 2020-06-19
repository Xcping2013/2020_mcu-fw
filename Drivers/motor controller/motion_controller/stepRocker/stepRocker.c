///*******************************************************************************
//  Project: stepRocker Mini-TMCL

//  Module:  stepRocker.c
//           Main program of the stepRocker open source Mini-TMCL

//   Copyright (C) 2011 TRINAMIC Motion Control GmbH & Co KG
//                      Waterloohain 5
//                      D - 22769 Hamburg, Germany
//                      http://www.trinamic.com/

//   This program is free software; you can redistribute it and/or modify it
//   freely.

//   This program is distributed "as is" in the hope that it will be useful, but
//   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//   or FITNESS FOR A PARTICULAR PURPOSE.
//*******************************************************************************/

///**
//  \file stepRocker.c
//  \author Trinamic Motion Control GmbH & Co KG
//  \version 1.00

//  \brief Main file

//  This file contains the main() function.
//*/

//#include <stdlib.h>
//#include "bsp_defines.h"

//#include "stepRocker.h"
//#include "SysTick.h"
//#include "RS485.h"
//#include "Commands.h"
//#include "Globals.h"
//#include "cube_spi.h"
//#include "TMC429.h"
//#include "TMC262.h"
//#include "IO.h"
//#include "SysControl.h"


//const UCHAR VersionString[]="1110V100";

///*********************************//**
//  \fn main(void)
//  \brief Main program
//  
//  This is the main function. It does
//  all necessary initializations and
//  then enters the main loop.
//*************************************/
//int stepRocker_main(void)
//{
//  EnableInterrupts();

//  ModuleConfig.SerialBitrate=0;
//  ModuleConfig.SerialModuleAddress=1;
//  ModuleConfig.SerialHostAddress=2;
//  MotorConfig[0].VMax=1000;
//  MotorConfig[0].AMax=500;
//  MotorConfig[0].PulseDiv=2;
//  MotorConfig[0].RampDiv=3;
//  MotorConfig[0].IRun=255;
//  MotorConfig[0].IStandby=16;
//  MotorConfig[0].StallVMin=0;
//  MotorConfig[0].FreewheelingDelay=0;
//  MotorConfig[0].SettingDelay=200;

//  InitIO();
//  InitSysTimer();
//  InitRS485(0);
//  InitSPI();
//  Init429();
//  InitMotorDrivers();

//  InitTMCL();
//  BlinkDelay=GetSysTimer();

//  SetUARTTransmitDelay(5);
//  
//  for(;;)
//  {
//  	ProcessCommand();
//  	SystemControl();

//  	if(abs(GetSysTimer()-BlinkDelay)>1000)
//  	{
//  		LED1_TOGGLE();
//  		BlinkDelay=GetSysTimer();
//  	}
//  	
//  	if(ExitTMCLFlag) return 0;
//  }
//}
