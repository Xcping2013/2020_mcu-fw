/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "inc_controller.h"
/* defined the LED0 pin: PD7 */
/*MB1616DEV6 PB2*/
/*MBTMC429	PD7*/
//#define LED0_PIN    GET_PIN(B, 2)

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    while (count++)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(300);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(300);
    }

    return RT_EOK;
}
