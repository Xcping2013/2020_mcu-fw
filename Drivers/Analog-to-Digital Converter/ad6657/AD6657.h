/********************************************************************************
 Author              : CAST (China Applications Support Team) 

 Date                : 5-4-2011

 File name           : AD6657.h

 Description         : Using ADuC7026 to control AD6657 in software SPI form through GPIO

 Hardware plateform  : ADuC7026 	+ AD6657
********************************************************************************/
#ifndef AD6657_H_
#define AD6657_H_

void InitializeAD6657(void);
void WriteCmdDataToAD6657ViaSpi(unsigned short CmdData);
void WriteRegDataToAD6657ViaSpi(unsigned char *pBuf, unsigned char N);
void ReadFromAD6657ViaSpi(unsigned char *RegisterData);

#define WRITE                     0x00
#define READ                     (0x01<<15)

//---------------------------------
//REGISTER MAP
//--------------------------------

//CHIP CONFIGURATION REGISTERS
#define SPI_PORT_CONFIGURATION    0x00
#define CHIP_ID                   0x01

//CHANNEL INDEX AND TRANSFER REGISITERS
#define CHANNEL_INDEX             0x05
#define TRANSFER                  0xFF

//ADC FUNCTION REGISTERS
#define POWER_MODES               0x08
#define CLOCK_DIVIDE              0x0B
#define SHUFFLE_MODE              0x0C
#define TEST_MODE                 0x0D
#define BIST_ENABLE               0x0E
#define OFFSET_ADJUST             0x10
#define OUTPUT_MODE               0x14
#define OUTPUT_ADJUST             0x15
#define CLOCK_PHASE_CONTROL       0x16
#define DCO_OUTPUT_DELAY          0x17
#define VREF_SELECT               0x18
#define BIST_SIGNATURE_LSB        0x24
#define BIST_SIGNATURE_MSB        0x25
#define SYNC_CONTROL              0x3A
#define NSR_CONTROL               0x3C
#define NSR_TUNING_WORD           0x3E

//------------------------------
// REGISTERS' FUNCTION BITS
//------------------------------
#define ENABLE_CHANNEL_CD_OUTPUT_PORT         0x80
#define ENABLE_CHANNEL_AB_OUTPUT_PORT         0x40
#define ENABLE_CHANNEL_D                      0x08
#define ENABLE_CHANNEL_C                      0x04
#define ENABLE_CHANNEL_B                      0x02
#define ENABLE_CHANNEL_A                      0x01

#define ONE_BYTE                  0x00
#define TWO_BYTE                 (0x01<<13)
#define THREE_BYTE               (0x01<<14)
#define STREAMING_MODE           (0x11<<13)

#endif


