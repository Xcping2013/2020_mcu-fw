#ifndef  __TMC429_MOTION_CMD_H
#define  __TMC429_MOTION_CMD_H

#include "bsp_defines.h"

void projectApp_init(void);

long ChangeVMaxToSpeed_ms(uint16_t VMaxReg)  ;
int16_t ChangeSpeed_msToVMax(long speed_ms)  ;

void SelectAMaxByVMax(uint8_t which_motor, uint32_t speed_Vmax);

#endif



