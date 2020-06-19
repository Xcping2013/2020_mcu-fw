/********************************************************************************
 Author             : CAST (China Applications Support Team) 

 Date               : 4-21-2011

 File name          : AD6657.c

 Description        : Using ADuC7026 to control AD6657 in software SPI form through GPIO;
			          

 Hardware plateform : ADuC7026 + AD6657 	
********************************************************************************/
#include "mcu_gpio.h"
#include "AD6657.h"

#define AD6657_CS			PB12
#define AD6657_SCLK			PB13
#define AD6657_SDI			PB14
#define AD6657_SDO			PB15	
/********************************************************************************
Function that writes to the AD6657 via the SPI port.
********************************************************************************/

void InitializeAD6657(void)
{
	pinMode(AD6657_CS,	PIN_OUT_PP);
	pinMode(AD6657_SCLK,PIN_OUT_PP);
	pinMode(AD6657_SDI,	PIN_OUT_PP);
	pinMode(AD6657_SDO,	PIN_INPUT_PULLUP);
	
	pinSet(AD6657_CS,1);
}

void WriteCmdDataToAD6657ViaSpi(unsigned short CmdData)
{
	unsigned char i;
	
	pinSet(AD6657_CS,0);
	drv_udelay(10);

	//Write out the ControlWord
	
	for(i=0; i<16; i++)
	{	
		pinSet(AD6657_SCLK,0);
		
		if(0x8000 == (CmdData & 0x8000))
		{
			pinSet(AD6657_SDI,1);	  //Send one to DIN pin
		}
		else
		{
			pinSet(AD6657_SDI,0);	  //Send zero to DIN pin
		}
		
		pinSet(AD6657_SCLK,1);
		
		CmdData <<= 1;	//Rotate data
		

	}
		
	pinSet(AD6657_CS,1);
}

void WriteRegDataToAD6657ViaSpi(unsigned char *pBuf, unsigned char N)
{
	unsigned char i,j;
	unsigned char TempData;
	
	pinSet(AD6657_CS,0);
	drv_udelay(10);

	//Write out the ControlWord
	for(j=0; j<N; j++)
	{
		TempData = pBuf[j];
		for(i=0; i<8; i++)
		{	
			pinSet(AD6657_SCLK,0);
			
			if(0x80 == (TempData & 0x80))
			{
				pinSet(AD6657_SDI,1);	  //Send one to DIN pin
			}
			else
			{
				pinSet(AD6657_SDI,0);	  //Send zero to DIN pin
			}
		
			pinSet(AD6657_SCLK,1);
		
			TempData <<= 1;	//Rotate data		
		}
	}
	
	pinSet(AD6657_CS,1);	
}

/********************************************************************************
Function that read from the AD6657 via the SPI port.
********************************************************************************/


void ReadFromAD6657ViaSpi(unsigned char *RegisterData)
{

	
	unsigned	int	    i= 0;
	unsigned	int  	iTemp = 0;

	pinSet(AD6657_CS,0);
	drv_udelay(10);

	// SPI start
	for(i=0; i<8; i++)
	{
		
		pinSet(AD6657_SCLK,1); //THE FALLING EDGE CLOCK OUT THE REGISTERS' VALUE
		
		pinSet(AD6657_SCLK,0);

		iTemp<<=1;
			 
		iTemp |= pinRead(AD6657_SDO);			// P4.4->SDout


	}

	pinSet(AD6657_CS,1);
  
	*RegisterData=iTemp;

	

}

