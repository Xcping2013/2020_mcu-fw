
#include "bsp_mcu_gpio.h"	
#include "bsp_mcu_delay.h"	

#include "bsp_tlc5973.h"	

//PD03
#define  SDI_PIN	PD_3

#define pTime 1					//in useconds
int tCycle = pTime * 6; // time for one bit. Per datasheet tCycle is between 0.33 useconds and 10 useconds
												// 8
int numLeds = 1; 				//number of LEDs

static void writeCommTimer(void) ;
static void writeCommand(void);
static void waitEOS(void);
static void waitGSLAT(void);
static void writeData(uint8_t data);
static void write12BITData(uint16_t data) ;

void bsp_tlc5973_init() 
{
	pinMode(SDI_PIN,PIN_MODE_OUTPUT);
	pinReset(SDI_PIN);

/*
The TLC5973 measures the time between the first and second SDI rising edges either after the device is
powered up or when the GS data latch sequence is executed
5us
*/	
//	delay_us(3);
//  pinSet(SDI_PIN);	
//  delay_us(3);
//  pinReset(SDI_PIN);	
//  delay_us(2); 
//  pinSet(SDI_PIN);
//	delay_us(3); 
}

void writeLED1(uint8_t ledNum, uint16_t redValue, uint16_t greenValue, uint16_t blueValue) 
{
  //for (int i = 0; i< numLeds; i++) 
	{
    writeCommTimer();
    
   // if (i = ledNum)
		{
			
      writeCommand();
      writeData(redValue);
      writeData(greenValue);
      writeData(blueValue);
    }	
    waitEOS();
  }
	waitGSLAT();   
}
void writeLED(uint8_t ledNum, uint16_t redValue, uint16_t greenValue, uint16_t blueValue) 
{
  //for (int i = 0; i< numLeds; i++) 
	{
    //writeCommTimer();
    
    //if (i = ledNum)
		{
			
//      writeCommand();
//      writeData(redValue);
//      writeData(greenValue);
//      writeData(blueValue);
			
			write12BITData(0x3AA);
      write12BITData(redValue);
      write12BITData(greenValue);
      write12BITData(blueValue);
    }
		//pinReset(SDI_PIN);
		
    waitEOS();
  }
//	if(MeasureOnce==0) 
//	{
//		MeasureOnce=1;
//		waitGSLAT();   //20*7 US 140uS
//	}
	waitGSLAT();
}

//This time must be between t x 0.9tCycle and t x 2.0tCycle
//tCycle is between 0.33 useconds and 10 useconds	
//void writeZero() 		
//{
//  pinSet(SDI_PIN);	
//  delay_us(1);
//  pinReset(SDI_PIN);	
//  delay_us(2);
//	
//	delay_us(2);
//}
void writeZero() 		//delay_us issue rt_hw_us_delay¡Ö3us t=17us 	
{
  pinSet(SDI_PIN);	
//	delay_us(pTime);
  pinReset(SDI_PIN);	
	pinReset(SDI_PIN);	
	pinReset(SDI_PIN);
	pinReset(SDI_PIN);		

//	delay_us(pTime);
//	delay_us(pTime);
//	delay_us(pTime);
//	delay_us(pTime);

}
void writeOne() 	//4us
{
  pinSet(SDI_PIN);	
  //delay_us(pTime);
  pinReset(SDI_PIN);	
  //delay_us(pTime); 
  pinSet(SDI_PIN);
//  delay_us(pTime);
  pinReset(SDI_PIN);	
	pinReset(SDI_PIN);	
	//t = 0.5 x tSDI CYCLE
//	delay_us(pTime);
//	delay_us(pTime);
//	delay_us(pTime);
	
}
void writeOne1() 
{
  pinSet(SDI_PIN);	
  delay_us(1);
  pinReset(SDI_PIN);	
  delay_us(1); 
  pinSet(SDI_PIN);
  delay_us(1);
  pinReset(SDI_PIN);	
	//t = 0.5 x tSDI CYCLE
	
	delay_us(2);
	
}

static void writeCommTimer(void) 
{
  //first two zeroes determine the timing (tcycle) after device is powered up or after a GSLAT
  writeZero();
  writeZero();
}

static void writeCommand(void) 
{
  //write command in hex is 3AA, binary is 0011 1010 1010
//	writeZero();
//	writeZero();
	
  writeOne();
  writeOne();
  
  writeOne();
  writeZero();
  writeOne();
  writeZero();

  writeOne();
  writeZero();
  writeOne();
  writeZero();  
}

//end of sequence (for a single driver chip)
static void waitEOS(void) 
{
 for(uint8_t i=0;i<25;i++)
 {
	pinReset(SDI_PIN);	

 }
//  pinReset(SDI_PIN);	
//  delay_us(tCycle*4); // min 3.5 to max 5.5 times tCycle
}

//grayscale latch (for the end of a chain of driver chips)
static void waitGSLAT(void) 
{
 for(uint8_t i=0;i<40;i++)
 {
	pinReset(SDI_PIN);	

 }
//  pinReset(SDI_PIN);	
//  delay_us(tCycle*10); // minimum 8 time tCycle
}
  
static void writeData(uint8_t data) 
{
  for (uint8_t i = 0; i<8; i++)
	{
    if(data & 0x80) 
		{
			writeOne();
		}
    else
		{
			writeZero();
		}
    data <<= 1;
  }
  //pad the end of each 8 bit number with four more 
  //bits (use 1010 because that's the end of the
  //write command so it doesn't have to have an entirely
  //separate function) to make a 12 bit number
  writeZero();
  writeZero();
  writeZero();
  writeZero();
}

static void write12BITData(uint16_t data) 
{
  for (uint8_t i = 0; i<12; i++)
	{
    if(data & 0x0800) 
		{
			writeOne();
		}
    else
		{
			writeZero();
		}
    data <<= 1;
  }
  //pad the end of each 8 bit number with four more 
  //bits (use 1010 because that's the end of the
  //write command so it doesn't have to have an entirely
  //separate function) to make a 12 bit number
//  writeZero();
//  writeZero();
//  writeZero();
//  writeZero();
}








