/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion for FinSH
 */

#ifndef __USER_FINSH_
#define __USER_FINSH_

#include <rtthread.h>

extern uint8_t cmds_lower_flag;

#ifndef FINSH_USING_MSH_ONLY

	#include "finsh.h"

	void finsh_run_line(struct finsh_parser *parser, const char *line);

#endif

//user
extern void cmd_mshMode_exit(void);
extern void cmd_mshMode_enter(void);

int msh_exec(char *cmd, rt_size_t length);

uint8_t cmds_lowercase(void);
int UserMsh_Commands(char *cmd, rt_size_t length);
uint8_t UserFinsh_Commands(struct finsh_parser *parser, char *line, uint8_t position);


#endif
