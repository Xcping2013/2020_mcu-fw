

#ifndef __USER_COMMANDS_H
#define	__USER_COMMANDS_H

#include "bsp_defines.h"

extern char UserCommands[FINSH_CMD_SIZE+20];

extern void user_show_version(void);

uint8_t  Command_analysis_User(char *string);

#endif

