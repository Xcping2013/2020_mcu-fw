#include "tmc429.h"

/*
//__HAL_AFIO_REMAP_SWJ_NOJTAG();

*/

#if 1   //DBG_ENABLE
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
	#endif
#endif

const float Vmax_factor = 1.43051147;			// 1 /( 0.429153/60*200)		
const float Amax_factor_slow = 0.125;			//800/80/80
const float Amax_factor_fast = 0.0056689; //250/210/210

uint8_t OriginSensorPin[6]=
{	
	PB_9,PE_0,PE_1,
	PB_9,PE_0,PE_1
};
uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};

void TMC429_DefaulSetting(void)	
{
  uint8_t i;
		
	for(i=0; i<N_O_MOTORS; i++) 					
	{		
		pinMode(OriginSensorPin[i],PIN_MODE_INPUT_PULLUP);
		
		motorSetting.saved=DATA_SAVED;
		
		motorSetting.limit_level_valid=1;
		motorSetting.orgin_level_valid=0;
		
		motorSetting.SpeedChangedFlag[i]= TRUE;		
		
		motorSetting.VMax[i]=840;
		motorSetting.AMax[i]=1000;
		
		motorSetting.PulseDiv[i]=7;
		motorSetting.RampDiv[i]=7;
		
		motorHoming.Homed[i]				=	FALSE;
		motorHoming.GoHome[i]				=	FALSE;
		motorHoming.GoLimit[i]			=	FALSE;
		motorHoming.HomeSpeed[i]		=	2000;
	}
}






