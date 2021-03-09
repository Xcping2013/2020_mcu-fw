#ifndef __INC_PROJECTS__H
#define __INC_PROJECTS__H

#include "bsp_defines.h"

#define EEPROM_APP_EN_ADDR 				 255*64+0
#define EEPROM_APP_CS_ADDR 				 255*64+1

enum DEVn_Type 
{
	NONE,
	FBIO0808,
	FBHOLYRELAY,
	FBJFFB1615CN1,
	FBTMC429,
	FB29125,
};

enum ProjectApps 
{
	BUTTON_ONLINE,
	BUTTON_OFFLINE,	
	LID_OPEN,
	
	NMEI_IN3COLA	,
	NMEI_IN3VEGE	,

	COMMON_USE,
	
	ATLAS_QT1,
	
	CLICK_RATIO,
	
	B495_QT1,
	
};

extern uint8_t OriginSensorPin[6];
extern uint8_t OriginSensorON[6];

extern uint8_t WhichProject;
extern uint8_t IDC_DEV[6];

extern void user_show_version(void);

void ProjectAppInit(void);

void IDC_DEV_Init(uint8_t whichDev, uint8_t devType);
//void ProjectAppRun(void);

#endif
