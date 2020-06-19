#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

#include "bsp_defines.h"

extern uint8_t buttonSTARTpressed;
extern uint8_t buttonRESETpressed;


void buttonSTART_process(uint8_t inCh, uint8_t outCh);
void buttonRESET_process(uint8_t inCh, uint8_t outCh);

#endif

