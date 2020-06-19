#ifndef __CAL_H__
#define __CAL_H__


#define USING_GAIN_AD_VALUE				1
//#define USING_GAIN_VOLTAGE_VALUE	1
// 得到比例校准数据
extern float Cal_GetK( int _Func );
// 设置比例校准数据
extern void Cal_SetK( int _Func , float _K );

// 得到常数校准数据
extern float Cal_GetC( int _Func );
// 设置常数校准数据
extern void Cal_SetC( int _Func , float _C );

// 将校准表归一化
extern void Cal_IdentifyRam( void );
// 将RAM中的校准数据保存到Flash
void Cal_RamToFlash( void );
// 将flash 中的数据加载到
extern void Cal_FlashToRam( void );


typedef struct
{
	float    				Gain[4];	
	float    				Offset[4];	
  unsigned char	  filterTimes;
}
DMM_KCTable;

extern DMM_KCTable DMM_CAL_Table;

void printGainOffset( void );

void DMM_CAL_KCTable_RamToEeprom(void);
void DMM_CAL_KCTable_EepromToRam(void);

void SetGainViaInputSignal( int _Func ,	int milliValue);
void SetOffsetViaInputSignal( int _Func ,	int milliValue);

#endif//__CAL_H__

