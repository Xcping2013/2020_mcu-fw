
#ifndef _FBIO0808_H
#define _FBIO0808_H

#include "pca95xx.h"	

extern uint8_t FBIO0808_is_inited;

extern pca95xx_t fbio0808_pca95xx;

void FBIO0808_Init(void);

uint8_t FBIO0808GetP0(void);
uint8_t FBIO0808GetP0Bit(uint8_t BitPosition);

void    FBIO0808SetP1(uint8_t P1data);
void 		FBIO0808SetP1Bit(uint8_t BitPosition,  uint8_t Bitdata);

uint8_t FBIO0808GetP1(void);
uint8_t FBIO0808GetP1Bit(uint8_t BitPosition);

#endif
