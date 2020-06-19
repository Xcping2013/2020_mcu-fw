
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SYS_CONTROL_H
#define __APP_SYS_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "bsp_mcu_delay.h"	
#include "app_eeprom_24xx.h"
#include "app_gpio_dido.h"	
#include "app_button.h"	
#include "app_tmc429_motion.h"
#include "app_motor_home.h"
#include "app_serial.h"	
#include "app_timer.h"	

extern UCHAR SpeedChangedFlag[N_O_MOTORS];
	 
void 			xstrcat(char *str1,char *str2);
uint16_t  Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


#ifdef __cplusplus
}
#endif
#endif /*__APP_SYS_CONTROL_H */


