/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion for FinSH
 */

#ifndef __USER_RTT__
#define __USER_RTT__

#include <rtthread.h>

//#include "user_msh.h"	

extern void cmd_mshMode_exit(void);
extern void cmd_mshMode_enter(void);

int msh_exec(char *cmd, rt_size_t length);

	#ifndef FINSH_USING_MSH_ONLY

		#include "finsh.h"

		void finsh_run_line(struct finsh_parser *parser, const char *line);

	#endif


#endif
