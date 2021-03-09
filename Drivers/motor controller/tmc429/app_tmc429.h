
#ifndef __APP_TMC429_H__
#define __APP_TMC429_H__

#include "tmc429.h"

extern  uint8_t tmc429_inited;
void tmc429_hw_init(void);

extern uint8_t getTMC429_DEV(uint8_t motor_number);

void motor_gohome_config(uint8_t motor_number, int home_speed);
void motor_golimit_config(uint8_t motor_number, int home_speed);
int32_t ChangeSpeedPerSecondToVMax(int32_t pulsePerSecond) ;
int32_t ChangeVMaxToSpeed_ms(int32_t VMaxReg) ;

#endif
