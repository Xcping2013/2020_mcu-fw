#include "isl29125.h"
#include "bsp_mcu_gpio.h"

/*
BYTE WRITE SEQUENCE

	START		DEV_ADDR_W		ACK		REG_ADDR		ACK		DATA		ACK		STOP
					0x88		
					
BYTE READ SEQUENCE

	START		DEV_ADDR_W		ACK		REG_ADDr		ACK		SRART 	DEV_ADDR_R	ACK		DATA	STOP
					0x88

BURST READ SEQUENCE

	START		DEV_ADDR_W		ACK		REG_ADDr		ACK		SRART 	DEV_ADDR_R	ACK		DATA1 ACK DATAN	STOP
					0x88
*/

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
//#include <rtthread.h>
#define DEBUG printf
#else
#define DEBUG(...)
#endif


/** @} */
/***********************************************************************
 * public API implementation
 **********************************************************************/

uint8_t isl29125_init(isl29125_t *dev)
{
		DEBUG("isl29125_init\n");
     /* initialize the I2C bus */	
		SoftI2c.pin_init(dev->pin);

    /* configuration 1: operation mode, range, resolution */
    uint8_t conf1 = 0x00;
    conf1 |= dev->mode;
    conf1 |= dev->range;
    conf1 |= dev->res;
    conf1 |= ISL29125_CON1_SYNCOFF; /* TODO: implement SYNC mode configuration */

    /* TODO: implement configuration 2: infrared compensation configuration */
   
    /* verify the device ID */
    uint8_t reg_id;
    int ret = SoftI2c.reads(dev->pin,1,ISL29125_I2C_ADDRESS, ISL29125_REG_ID, &reg_id,1);
    if ((reg_id == ISL29125_ID) && (ret == 0)) 
		{
        DEBUG("isl29125_init: ID=%d successfully verified\n",reg_id);
    }
    else 
		{
			DEBUG("isl29125_init: ID could not be verified, ret: %i\nid: %i\n", ret,reg_id);
        return 1;
    }
		uint8_t cmd=ISL29125_CMD_RESET;
    /* configure and enable the sensor */
    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_RESET)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_RESET, &cmd,1);
    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_CONF1)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, &conf1,1);
    DEBUG("isl29125_init: success\n");
    return 0;
}

int isl29125_init_int(isl29125_t *dev)                                 
{
    /* configuration 3: interrupt mode configuration */
    uint8_t conf3 = 0x00;
	
		pinMode(dev->int_pin,GPIO_MODE_INPUT);
	
    conf3 |= dev->interrupt_status;
    conf3 |= dev->interrupt_persist;
    conf3 |= dev->interrupt_conven;

    /* Lower and higher interrupt threshold registers. */
    uint8_t lthlb = 0x00;
    uint8_t lthhb = 0x00;
    uint8_t hthlb = 0x00;
    uint8_t hthhb = 0x00;
    uint16_t max_range = 10000;

    if (dev->range == 0x00) {
        max_range = 375;
    }

    if ((dev->higher_threshold <= max_range) && (dev->lower_threshold < dev->higher_threshold)) 
		{
        dev->lower_threshold *= (uint16_t) (65535 / max_range);
        lthlb = (uint8_t)(dev->lower_threshold & 0xff);
        lthhb = (uint8_t)(dev->lower_threshold >> 8);
        dev->higher_threshold *= (uint16_t) (65535 / max_range);
        hthlb = (uint8_t)(dev->higher_threshold & 0xff);
        hthhb = (uint8_t)(dev->higher_threshold >> 8);
    }

    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_CONF3)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF3, &conf3,1);

    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_LTHLB)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_LTHLB, &lthlb,1);

    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_LTHHB)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_LTHHB, &lthhb,1);

    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_HTHLB)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_HTHLB, &hthlb,1);

    DEBUG("isl29125_init: SoftI2c.writes(ISL29125_REG_HTHHB)\n");
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_HTHHB, &hthhb,1);

    return 0;
}

void isl29125_read_rgb_lux(const isl29125_t *dev, isl29125_rgb_t *dest)
{
    /* read values */
    uint8_t bytes[6];
    (void) SoftI2c.reads(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_GDLB, bytes, 6);
    /* possibly shift by 4 to normalize 12 to 16 bit */
    int resfactor = (dev->res == ISL29125_RESOLUTION_12) ? 4 : 0;
    /* parse and normalize readings */
    uint16_t green = (bytes[0] | (bytes[1] << 8)) << resfactor;
    uint16_t red   = (bytes[2] | (bytes[3] << 8)) << resfactor;
    uint16_t blue  = (bytes[4] | (bytes[5] << 8)) << resfactor;

		//DEBUG("%d %d %d %d %d %d\n",bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5]);
    //DEBUG("isl29125_read_rgb: adjusted, unconverted readings: (%5i / %5i / %5i) \n", red, green, blue);
    //DEBUG(":%5i %5i %5i\n", red, green, blue);

    /* convert readings to lux */
    float luxfactor = (dev->range == ISL29125_RANGE_10K) ? 10000.0 / 65535.0 : 375.0 / 65535.0;
    dest->red = red * luxfactor;
    dest->green = green * luxfactor;
    dest->blue = blue * luxfactor;
}

void isl29125_read_rgb_color(const isl29125_t *dev, color_rgb_t *dest)
{
    /* read values */
    uint8_t bytes[6];
    (void) SoftI2c.reads(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_GDLB, bytes, 6);

    /* factor normalize 12 or 16 bit to 8 bit */
    int normfactor = (dev->res == ISL29125_RESOLUTION_12) ? 4 : 8;
    /* parse and normalize readings */
    dest->g = (bytes[0] | (bytes[1] << 8)) >> normfactor;
    dest->r = (bytes[2] | (bytes[3] << 8)) >> normfactor;
    dest->b = (bytes[4] | (bytes[5] << 8)) >> normfactor;
}

void isl29125_set_mode(const isl29125_t *dev, isl29125_mode_t mode)
{
    uint8_t conf1;

    (void) SoftI2c.reads(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, &conf1,1);
    conf1 &= ~ISL29125_CON1_MASK_MODE;
    conf1 |= mode;
    (void) SoftI2c.writes(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, &conf1,1);
}

int isl29125_read_irq_status(const isl29125_t *dev)
{
    uint8_t irq_status;
    (void) SoftI2c.reads(dev->pin, 1, ISL29125_I2C_ADDRESS, ISL29125_REG_STATUS, &irq_status,1);
    /* return bit 0 (RGBTHF)*/
    return (irq_status & 0x01);
}
