#ifndef  __APP_MOTION_H
#define  __APP_MOTION_H

#include "bsp_tmc429.h"

void tmc429_hw_init(void);


uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
uint8_t TMC429_MotorStop(uint8_t motor_number);
void 		StopMotorByRamp(UINT Motor);

int 		MotorLimitCheck_thread_init(void);
int 		motor(int argc, char **argv);




#endif


