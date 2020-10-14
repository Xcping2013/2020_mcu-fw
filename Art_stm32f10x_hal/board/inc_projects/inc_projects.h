#ifndef __INC_PROJECTS__H
#define __INC_PROJECTS__H

#include "bsp_defines.h"

#define EEPROM_APP_EN_ADDR 				 255*64+0
#define EEPROM_APP_CS_ADDR 				 255*64+1

enum ProjectApps 
{
	BUTTON_ONLINE,
	BUTTON_OFFLINE,	
	LID_OPEN,
	
	NMEI_IN3COLA	,
	NMEI_IN3VEGE	,

	COMMON_USE,
	
};

extern uint8_t WhichProject;

void ProjectAppInit(void);

//void ProjectAppRun(void);

#endif
