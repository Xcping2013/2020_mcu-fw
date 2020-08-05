#ifndef  __AD7124_DMM_H__
#define  __AD7124_DMM_H__

#include "bsp_defines.h"

enum ad7124_sps
{
	sps_10=0	,
	sps_20		,
	sps_40		,
	sps_50		,
	sps_60		,
	sps_80		,
	sps_160		,
	sps_320		,
	sps_640		,
	sps_1280	,
	sps_2400	,
	sps_4800	,
	sps_9600	,
	sps_19200 ,
	
	sps_cnt		,
};

void ad7124_init(void);
	
void SetAD7124ToMatchFun( int _Func );



#endif





