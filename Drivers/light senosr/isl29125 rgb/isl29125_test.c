/*
 * Copyright 2015 Ludwig Knüpfer
 * Copyright 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the ISL29125 RGB light sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Martin Heusmann <martin.heusmann@haw-hamburg.de>
 * @author      Cenk Gündoğan <mail-github@cgundogan.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

//#include "xtimer.h"
#include "bsp_mcu_delay.h"
#include "isl29125.h"
//#include "isl29125_params.h"

#define SLEEP       (250 * 1000U)

void cb(void *arg)
{
    (void) arg;
    printf("INT: external interrupt\n");
}

int isl29125_test(void)
{
    isl29125_t dev;
    isl29125_rgb_t data;
    color_rgb_t data8bit;
    memset(&data, 0x00, sizeof(data));

    /* Parameters for testing, change if needed. */
    uint16_t lower_threshold = 0;
    uint16_t higher_threshold = 8000;

		dev.int_pin=PD_5;
		dev.pin.scl_pin=PD_6;
		dev.pin.sda_pin=PD_7;
	
		dev.mode=ISL29125_MODE_RGB;
		dev.range=ISL29125_RANGE_375;		//ISL29125_RANGE_10K			ISL29125_RANGE_375
		dev.res=ISL29125_RESOLUTION_16;	//ISL29125_RESOLUTION_12	ISL29125_RESOLUTION_16
	
		dev.interrupt_status=ISL29125_INTERRUPT_STATUS_RED;
		dev.interrupt_persist=ISL29125_INTERRUPT_PERSIST_1;
		dev.interrupt_conven=ISL29125_INTERRUPT_CONV_DIS;
		dev.higher_threshold=higher_threshold;
		dev.lower_threshold=lower_threshold;
	
	
    printf("ISL29125 light sensor test application\n");
    //printf("Initializing ISL29125 sensor at I2C_%i... ", isl29125_params[0].i2c);
    if (isl29125_init(&dev) == 0) {
        printf("[OK]\n");
    }
    else {
        printf("[Failed]");
        return 1;
    }

    printf("Initializing interrupt");

    if (isl29125_init_int(&dev) != 0) {
        printf("[Failed]");
        return 1;
    }

    /* try out some modes */
    static const isl29125_mode_t modes[] = {
        ISL29125_MODE_DOWN, ISL29125_MODE_STANDBY, ISL29125_MODE_RGB,
        ISL29125_MODE_R, ISL29125_MODE_G, ISL29125_MODE_B,
        ISL29125_MODE_RG, ISL29125_MODE_GB};
    static const char* mode_names[] = {
        "ISL29125_MODE_DOWN", "ISL29125_MODE_STANDBY", "ISL29125_MODE_RGB",
        "ISL29125_MODE_R", "ISL29125_MODE_G", "ISL29125_MODE_B",
        "ISL29125_MODE_RG", "ISL29125_MODE_GB"};

//while(1)
{	
				
    for (size_t i = 0; i < ARRAY_SIZE(modes); i++) {
        printf("Setting mode %s\n", mode_names[i]);
        isl29125_set_mode(&dev, modes[i]);
        drv_mdelay(200);
        isl29125_read_rgb_color(&dev, &data8bit);
        printf("RGB value: (%3i / %3i / %3i) 8 bit\n",
                data8bit.r, data8bit.g, data8bit.b);
    }
	}
    printf("Resetting mode to RGB and reading continuously");
    isl29125_set_mode(&dev, ISL29125_MODE_RGB);
    drv_mdelay(100);
    while (1) {
        isl29125_read_rgb_lux(&dev, &data);
        printf("RGB value: (%5i / %5i / %5i) lux\n",
                (int)data.red, (int)data.green, (int)data.blue);
        drv_mdelay(200);
        printf("IRQ-Status: %i \n", isl29125_read_irq_status(&dev));		
				drv_mdelay(200);
			        isl29125_read_rgb_color(&dev, &data8bit);
        printf("RGB value: (%3i / %3i / %3i) 8 bit\n",
                data8bit.r, data8bit.g, data8bit.b);
				drv_mdelay(200);
    }

    return 0;
}


//RGB value: (  0 /  50 / 225) 8 bit
//79 50 0 0 7 224
//isl29125_read_rgb: adjusted, unconverted readings: (    0 / 12879 / 57351) 
//RGB value: (    0 /  1965 /  8751) lux


//+ok@rgb.startstrm(3, 1)
//+msg@rgb.adc(00000,65533,65534)
//MSH_CMD_EXPORT(isl29125_test,...);

