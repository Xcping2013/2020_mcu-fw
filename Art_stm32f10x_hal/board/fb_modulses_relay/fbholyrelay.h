
#ifndef _FBHOLYRELAY_H
#define _FBHOLYRELAY_H

#include "tca6424a.h"		

extern uint8_t FBHOLYRELAY_is_inited;

extern TCA6424A_t FBHOLYRELAY_tca6424;

void FBHOLYRELAY_Init(void);

void FBHOLYRELAYSetPort(uint32_t Pdata);

uint8_t FBHOLYRELAYGetP1(void);
uint8_t FBHOLYRELAYGetP1Bit(uint8_t BitPosition);

#endif
