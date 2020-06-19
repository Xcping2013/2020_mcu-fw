#ifndef  __DMM_H__
#define  __DMM_H__

#include "ad7124.h"

#define lenof(a) (sizeof(a)/sizeof((a)[0]))

//#define USING_EX_CURRENT_SOURCE		1
//OHM_1R_4W		DMM_FUNC_OHM_1R_4W
//DMM_FUNC_TYPE

enum 
{
	DMM_FUNC_UNKNOW				,
		
	DMM_FUNC_DCV_100mV		,
	DMM_FUNC_DCV_1V				,
	
	
	DMM_FUNC_DC1V_5V			,
	DMM_FUNC_DC2V_5V			,
	
	DMM_FUNC_DCV_10V			,
	DMM_FUNC_DCV_25V			,
	
	

	DMM_FUNC_DCI_100uA		,	
	DMM_FUNC_DCI_1mA			,
	
	DMM_FUNC_DCI_100mA		  ,	
//	DMM_FUNC_DCI_75mA		  ,	
//	DMM_FUNC_DCI_285mA		,	
//	DMM_FUNC_OHM_R100_4W	,	
//	MM_FUNC_OHM_R150_4W	  ,	
	
	DMM_FUNC_DCI_1A				,	
	
	DMM_FUNC_OHM_10R_4W		,	
	
	DMM_FUNC_OHM_1K_4W		,	
	DMM_FUNC_OHM_10K_4W		,	
		
	DMM_FUNC_COUNT		
};
							   
#define FILTER_NONE				0
#define FILTER_5_AVAGE 		5
#define FILTER_10_AVAGE 	10
#define FILTER_25_AVAGE 	25
#define FILTER_50_AVAGE 	50

extern float KTable[ DMM_FUNC_COUNT ];
extern float CTable[ DMM_FUNC_COUNT ];



extern uint32_t ad7124DataCH[2];


extern struct ad7124_dev my_ad7124;

extern struct ad7124_dev *ad7124_handler;
void ad7124_init(void);
double AD7124_ReadAverData(struct ad7124_dev *device);

int32_t AD7124_ReadADAverValue(struct ad7124_dev *device);
double AD7124_AdValueToVotage(int32_t ADvalue);



void ad7124_getSetK(void);
void ADCChannelConvert( int _Func );
void AD7124ChannelConfigToReg(void);
void print_dmm_value( int _Func );

void AD7124_SYS_CALIBRATION( uint8_t mode, int _Func );
void AD7124_INT_CALIBRATION( uint8_t mode, int _Func );

void Reg_IdentifyRam(void);
void AD7124_CALIBRATION_RamToEeprom(void);
void AD7124_CALIBRATION_EepromToRam(void);

void ad7124_inter_calibration(void);


void DMM_ad7124Init(void);
void DMM_GetCHData(void);
double DMM_GetAverVoltage(uint8_t ch,uint8_t times);
	
#endif//__DMM_H__

