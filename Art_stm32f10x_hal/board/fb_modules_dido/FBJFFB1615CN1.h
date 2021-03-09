
#ifndef _FBJFFB1615CN1_H
#define _FBJFFB1615CN1_H

#include "pca95xx.h"	

extern uint8_t FBJFFB1615CN1_is_inited;
extern pca95xx_t FBJFFB1615CN1_pca95xx1;
extern pca95xx_t FBJFFB1615CN1_pca95xx2;

void FBJFFB1615CN1_Init(void);

uint8_t FBJFFB1615CN1GetP0(uint8_t whichIIC);
uint8_t FBJFFB1615CN1GetP0Bit(uint8_t whichIIC, uint8_t BitPosition);

void FBJFFB1615CN1SetP1(uint8_t whichIIC, uint8_t P0data);
void FBJFFB1615CN1SetP1Bit(uint8_t whichIIC, uint8_t BitPosition,  uint8_t Bitdata);

uint8_t FBJFFB1615CN1GetP1(uint8_t whichIIC);
uint8_t FBJFFB1615CN1GetP1Bit(uint8_t whichIIC , uint8_t BitPosition);

#endif
