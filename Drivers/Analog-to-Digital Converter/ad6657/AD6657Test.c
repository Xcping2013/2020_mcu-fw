/********************************************************************************

********************************************************************************/
#include "mcu_gpio.h"
#include "AD6657.h"

//---------------------------------------------------------------
//The format of the  16bit instruction word read as follows
// |R/W|W1|W0|A12|A11|A10||A9|A8|A7|A6|D5|D4|D3||D2|D1|D0|
//---------------------------------------------------------------


int AD6657_app()
{ 

	unsigned char buf;
	unsigned int  cstr;
	unsigned char pBuf[4] = {0,0,0,0};


	InitializeAD6657();

	//-------------------------------------------------
	//write 16bit command to the select register
	//-------------------------------------------------	
	cstr = WRITE | ONE_BYTE | CHANNEL_INDEX ;
	WriteCmdDataToAD6657ViaSpi(cstr);

	//-----------------------------------------------------------
	//write 8bit value to selected register
	//-----------------------------------------------------------
	pBuf[0] = ENABLE_CHANNEL_CD_OUTPUT_PORT  | ENABLE_CHANNEL_C | ENABLE_CHANNEL_D;
	WriteRegDataToAD6657ViaSpi(pBuf,1);
	
	//-----------------------------------------------------------------
	//read back selected register's 8bit value through CommAssistant
	//-----------------------------------------------------------------	
	cstr = READ | ONE_BYTE | CHANNEL_INDEX ;
	WriteCmdDataToAD6657ViaSpi(cstr);
	ReadFromAD6657ViaSpi(&buf); 

	//putchar(buf);
	
	
    while(1){;}
	
}

