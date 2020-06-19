/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_bh1750fvi		 
 * @{						
 *
 * @file
 * @brief       BH1750FVI ambient light sensor driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
/*
write: ST 01000110 ACK	OP ACK SP
read:	 ST 01000111 ACK  HB ACK LB NACK SP

*/
#include <stdio.h>

#include "bsp_mcu_delay.h"

#include "bh1750fvi.h"
#include "bh1750fvi_internal.h"

#define ENABLE_DEBUG        (1)

#if ENABLE_DEBUG
#define DEBUG printf
#else
#define DEBUG(...)
#endif

int bh1750fvi_init(bh1750fvi_t *dev, const bh1750fvi_params_t *params)
{
    int res;
	
    /* initialize the device descriptor */
    dev->i2c = params->i2c;
    dev->addr = params->addr;

    /* initialize the I2C bus */
    SoftI2c.pin_init(dev->i2c);

    /* send a power down command to make sure we can speak to the device */
    //res = i2c_write_byte(dev->i2c, dev->addr, OP_POWER_DOWN, 0);
	
    res = SoftI2c.writes(dev->i2c, 0, dev->addr, 0 , (uint8_t *)OP_POWER_DOWN, 1);

    //i2c_release(dev->i2c);
    if (res != 0) {
			//DEBUG("bh1750_init: fail\n");
        return BH1750FVI_ERR_I2C;
    }
    return BH1750FVI_OK;
}

uint16_t bh1750fvi_sample(const bh1750fvi_t *dev)
{
    uint32_t tmp;
    uint8_t raw[2];

    /* power on the device and send single H-mode measurement command */
    DEBUG("[bh1750fvi] sample: triggering a conversion\n");

    SoftI2c.writes(dev->i2c, 0, dev->addr, 0, (uint8_t *)OP_POWER_ON, 1);
    SoftI2c.writes(dev->i2c, 0, dev->addr, 0, (uint8_t *)OP_SINGLE_HRES1, 1);

    /* wait for measurement to complete */
    delay_ms(DELAY_HMODE);

    /* read the results */
    DEBUG("[bh1750fvi] sample: reading the results\n");
    SoftI2c.reads(dev->i2c, 0, dev->addr, 0, raw, 2);

    /* and finally we calculate the actual LUX value */
    tmp = ((uint32_t)raw[0] << 24) | ((uint32_t)raw[1] << 16);
    tmp /= RES_DIV;
    return (uint16_t)(tmp);
}
