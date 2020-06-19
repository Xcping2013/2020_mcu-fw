/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-03-30     Bernard      the first verion for FinSH
 */

#ifndef _INC_DIDO_
#define _INC_DIDO_

#include "bsp_defines.h"

extern void dido_hw_init(void);
extern uint8_t inputGet(uint8_t channel);
extern void    outputSet(uint8_t channel, uint8_t setval);

#endif
