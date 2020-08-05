#ifndef  __DMM_H__
#define  __DMM_H__

#include "bsp_defines.h"

#define lenof(a) (sizeof(a)/sizeof((a)[0]))

#define SAMPLE_RES_OHM  0.15 			//0.15R
#define SAMPLE_RES_GAIN  10

enum
{
	data		,
	sps			,
	averTimes,
	gain		,
	offset	,
};

enum
{
	DMM_FUNC_UNKNOW				,
		
	DMM_FUNC_DCV_100mV		,
	DMM_FUNC_DCV_1V				,
	DMM_FUNC_DCV_10V			,
	DMM_FUNC_DCV_25V			,
 	
	DMM_FUNC_OHM_100R_2W	,
	DMM_FUNC_OHM_10K_2W		,
  DMM_FUNC_OHM_1M_2W		,
	
	DMM_FUNC_OHM_10R_4W		,	
	DMM_FUNC_OHM_10K_4W		,	
	DMM_FUNC_OHM_1M_4W		,	

	DMM_FUNC_DCI_100mA		,		
	DMM_FUNC_DCI_500mA		,	
	DMM_FUNC_DCI_1A				,	
	
	DMM_FUNC_COUNT	
};
typedef struct	
{																	// Bytes	114
	uint8_t saved;									// 1
	uint8_t averTimes;							// 1			//平均次数
	float KTable[DMM_FUNC_COUNT];		// 4*15  56
	float CTable[DMM_FUNC_COUNT];		// 4*15  56
}
DMM_Calibration_T;

extern DMM_Calibration_T DMM;

extern uint8_t  dmm_fun_type;

#define A2DFLAG_OK  	0	// A2D 正常
#define A2DFLAG_ORP 	1	// + over range 
#define A2DFLAG_ORN 	2	// - over range

#define FILTER_NONE			0

#define FILTER_5_AVAGE 		5
#define FILTER_10_AVAGE 	10
#define FILTER_25_AVAGE 	25
#define FILTER_50_AVAGE 	50

// 一次转换记录
struct REC
{
	 float 			value;	 				// 采样值
	 float 			ds;		 					// 采样值的均方差（如果平均采样的话）
	 unsigned int	time;	 				// 转换时刻时间

	 unsigned char 	flag;	 			// 转换标记
	 unsigned char 	func;	 			// 功能和量程
 	 unsigned char 	filter;	 		// 滤波器类型
 	 unsigned char 	isUpdated;	// 是否需要更新 UI和记录 （平均值采样，并不是每次转换都要更新的）
};

extern void DMM_SetFilter( unsigned char _Type );
extern unsigned char DMM_GetFilter( void );
extern void	DMM_FillFilter( float _v );
//-----------------------------------------
extern void DMM_Init( void );
//-----------------------------------------
extern void DMM_CatchNull( void );
extern void DMM_CancelNull( void );
extern void DMM_ToogleAuto( void );
extern void	DMM_ToogleFilter( void );
extern void DMM_ToogleFilter_Inv( void );
extern void DMM_RangeUp( void );
extern void DMM_RangeDown( void );
extern void DMM_Convert( struct REC* _pRec );
extern void DMM_SetFunc( unsigned char _s );
extern unsigned char DMM_GetFunc( void );
extern void DMM_SetAuto( unsigned char _s );
extern unsigned char DMM_GetAuto( void );
//

extern float Cal_GetK( int _Func );
extern void Cal_SetK( int _Func , float _K );
extern float Cal_GetC( int _Func );
extern void Cal_SetC( int _Func , float _C );
extern void CalDefault_IdentifyRam( void );
extern void Cal_RamToEEPROM( void );
extern void Cal_EEPROMToRam( void );


//
void dmm_init(void);

extern void adc_hw_init(void);
extern void adc_print_data( int _Func );

extern uint32_t adc_sps_rw(uint8_t rw_cs, uint16_t sps_value );
extern void adc_set_sps(  int _Func , uint16_t sps_set );
//
#endif//__DMM_H__





