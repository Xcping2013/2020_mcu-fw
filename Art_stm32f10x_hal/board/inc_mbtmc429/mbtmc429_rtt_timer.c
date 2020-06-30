/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mbtmc429_rtt_timer.h"

static rt_timer_t timer1;
static rt_uint8_t out_flash_on[9]={0};
static rt_uint16_t out_flash_delay[9]={0};
static rt_uint16_t out_flash_cnt[9]={0};
static int timer_10ms_init(void);
static void timeout_10ms(void *parameter);

static int timer_10ms_init(void)
{
	timer1 = rt_timer_create("timer_10ms", timeout_10ms,
                             RT_NULL, 10,
                             RT_TIMER_FLAG_PERIODIC);
  if (timer1 != RT_NULL) 
		rt_timer_start(timer1);	
	return 0;
}
static void timeout_10ms(void *parameter)
{
	//ButtonProcess();

	//DEBUG_TRACE("timeout_10ms enter\n");	
//	for(uint8_t i=0;i<9;i++)
//	{
//		if ((out_flash_on[i]==1) && (out_flash_cnt[i]++>out_flash_delay[i]))
//		{
//			if(i==8) pinToggle(rgb_pin_num[1]);
//			else
//			{
//				pinToggle(outputs_pin_num[i]);
//			}		
//			//DEBUG_TRACE("out%d is flashing\n",i+1);	
//			out_flash_cnt[i]=0;
//		}
//	}	
}
//
