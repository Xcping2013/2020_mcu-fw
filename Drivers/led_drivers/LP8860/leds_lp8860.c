// SPDX-License-Identifier: GPL-2.0-only
/*
 * TI LP8860 4-Channel LED Driver
 *
 * Copyright (C) 2014 Texas Instruments
 *
 * Author: Dan Murphy <dmurphy@ti.com>
 */


#include "leds_lp8860.h"


#define LP8860_DISP_CL1_BRT_MSB		0x00		//CL1 13B
#define LP8860_DISP_CL1_BRT_LSB		0x01
/*
 If output 1 is used in display mode, the Brightness/Cluster Output 1 Brightness Control register is used for all outputs in display mode (16-bits register)
 Otherwise it is the Brightness Control register for cluster output 1. 
 For cluster bit control is 13 bit, most significant bit are used.
*/
#define LP8860_DISP_CL1_CURR_MSB	0x02
/*
Reset value loaded during start-up from EEPROM REG0
Display/Cluster current control 12BIT
*/
#define LP8860_DISP_CL1_CURR_LSB	0x03
/*
Reset value loaded during start-up from EEPROM REG1
*/
#define LP8860_CL2_BRT_MSB		0x04
#define LP8860_CL2_BRT_LSB		0x05
#define LP8860_CL2_CURRENT		0x06
#define LP8860_CL3_BRT_MSB		0x07
#define LP8860_CL3_BRT_LSB		0x08
#define LP8860_CL3_CURRENT		0x09
#define LP8860_CL4_BRT_MSB		0x0a
#define LP8860_CL4_BRT_LSB		0x0b
#define LP8860_CL4_CURRENT		0x0c
#define LP8860_CONFIG			0x0d
#define LP8860_STATUS			0x0e
#define LP8860_FAULT			0x0f
#define LP8860_LED_FAULT		0x10
#define LP8860_FAULT_CLEAR		0x11
#define LP8860_ID			0x12
#define LP8860_TEMP_MSB			0x13
#define LP8860_TEMP_LSB			0x14
#define LP8860_DISP_LED_CURR_MSB	0x15
#define LP8860_DISP_LED_CURR_LSB	0x16
#define LP8860_DISP_LED_PWM_MSB		0x17
#define LP8860_DISP_LED_PWM_LSB		0x18
#define LP8860_EEPROM_CNTRL		0x19
#define LP8860_EEPROM_UNLOCK		0x1a

#define LP8860_EEPROM_REG_0		0x60
#define LP8860_EEPROM_REG_1		0x61
#define LP8860_EEPROM_REG_2		0x62
#define LP8860_EEPROM_REG_3		0x63
#define LP8860_EEPROM_REG_4		0x64
#define LP8860_EEPROM_REG_5		0x65
#define LP8860_EEPROM_REG_6		0x66
#define LP8860_EEPROM_REG_7		0x67
#define LP8860_EEPROM_REG_8		0x68
#define LP8860_EEPROM_REG_9		0x69
#define LP8860_EEPROM_REG_10		0x6a
#define LP8860_EEPROM_REG_11		0x6b
#define LP8860_EEPROM_REG_12		0x6c
#define LP8860_EEPROM_REG_13		0x6d
#define LP8860_EEPROM_REG_14		0x6e
#define LP8860_EEPROM_REG_15		0x6f
#define LP8860_EEPROM_REG_16		0x70
#define LP8860_EEPROM_REG_17		0x71
#define LP8860_EEPROM_REG_18		0x72
#define LP8860_EEPROM_REG_19		0x73
#define LP8860_EEPROM_REG_20		0x74
#define LP8860_EEPROM_REG_21		0x75
#define LP8860_EEPROM_REG_22		0x76
#define LP8860_EEPROM_REG_23		0x77
#define LP8860_EEPROM_REG_24		0x78

#define LP8860_LOCK_EEPROM		0x00
#define LP8860_UNLOCK_EEPROM		0x01
#define LP8860_PROGRAM_EEPROM		0x02
#define LP8860_EEPROM_CODE_1		0x08
#define LP8860_EEPROM_CODE_2		0xba
#define LP8860_EEPROM_CODE_3		0xef

#define LP8860_CLEAR_FAULTS		0x01

#define LP8860_NAME			"lp8860"

#define LP8860_ADDR			0x5A

IIC_GPIO	lp8860_i2c_pins;

struct lp8860_reg {
	uint8_t reg;
	uint8_t value;
};

static const struct lp8860_reg lp8860_reg_defs[] = {
	{ LP8860_DISP_CL1_BRT_MSB, 0x00},
	{ LP8860_DISP_CL1_BRT_LSB, 0x00},
	{ LP8860_DISP_CL1_CURR_MSB, 0x00},
	{ LP8860_DISP_CL1_CURR_LSB, 0x00},
	{ LP8860_CL2_BRT_MSB, 			0x00},
	{ LP8860_CL2_BRT_LSB,			  0x00},
	{ LP8860_CL2_CURRENT, 			0x00},
	{ LP8860_CL3_BRT_MSB, 			0x00},
	{ LP8860_CL3_BRT_LSB,			  0x00},
	{ LP8860_CL3_CURRENT, 			0x00},
	{ LP8860_CL4_BRT_MSB, 			0x00},
	{ LP8860_CL4_BRT_LSB, 			0x00},
	{ LP8860_CL4_CURRENT, 			0x00},
	{ LP8860_CONFIG,		  			0x00},
	{ LP8860_FAULT_CLEAR, 			0x00},
//	{ LP8860_EEPROM_CNTRL, 			0x80},
//	{ LP8860_EEPROM_UNLOCK,		  0x00},
};


void lp8860_init(void)
{
	lp8860_i2c_pins.scl_pin = PB_13;
	lp8860_i2c_pins.sda_pin = PB_15;	
	
	SoftI2c.pin_init(lp8860_i2c_pins);
	//I=100mA
//	uint8_t lp8860_brt_regdata[14]={ 0x0f,0xff,0x0f,0xff,   0x0f,0xff,0xff,  0x0f,0xff,0xff,  0x0f,0xff,0xff,	 0x05};
//	
//	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, 0,(uint8_t *)&lp8860_brt_regdata,14);
}
void lp8860_current_set(uint8_t current_ch1, uint8_t current_ch2, uint8_t current_ch3, uint8_t current_ch4)
{
	uint16_t current_reg_ch[2]={0};
	uint16_t current_reg_ch1,current_reg_ch2,current_reg_ch3,current_reg_ch4;
	
	if(current_ch1>88)	current_ch1=88;
	if(current_ch2>88)	current_ch2=88;
	if(current_ch3>88)	current_ch3=88;
	if(current_ch4>88)	current_ch4=88;	

	current_reg_ch1= current_ch1*4095/100;
	current_reg_ch2= current_ch2*255/100;
	current_reg_ch3= current_ch3*255/100;
	current_reg_ch4= current_ch3*255/100;
	
	current_reg_ch[0]= ( current_reg_ch1 & 0xff00) >> 8 ;
	current_reg_ch[1]= ( current_reg_ch1 & 0xff) ;


	uint8_t lp8860_brt_regdata[14]={ 0x0f,0xff,current_reg_ch[0],current_reg_ch[1],   0x0f,0xff,current_reg_ch2,  0x0f,0xff,current_reg_ch3,  0x0f,0xff,current_reg_ch4,	 0x05};
	
	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, 0,(uint8_t *)&lp8860_brt_regdata,14);
	
//	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, LP8860_DISP_CL1_CURR_MSB,(uint8_t *)&current_reg_ch,2);

//	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, LP8860_CL2_CURRENT,(uint8_t *)&current_reg_ch2,1);
//		
//	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, LP8860_CL3_CURRENT,(uint8_t *)&current_reg_ch3,1);
//	
//	SoftI2c.writes(lp8860_i2c_pins, 1, LP8860_ADDR, LP8860_CL4_CURRENT,(uint8_t *)&current_reg_ch4,1);
}

INIT_APP_EXPORT(lp8860_init);

//MSH_CMD_EXPORT_ALIAS(lp8860_current, current,...);

//FINSH_FUNCTION_EXPORT_ALIAS(lp8860_current_set, setcurrent,...);



#if 0
struct lp8860_led
{
	uint8_t 	lock;
	IIC_GPIO	pins;
	uint8_t   devAddress;
};

struct lp8860_reg {
	uint8_t reg;
	uint8_t value;
};
static struct lp8860_reg lp8860_eeprom_disp_regs[] = {
	{ LP8860_EEPROM_REG_0, 0xed },
	{ LP8860_EEPROM_REG_1, 0xdf },
	{ LP8860_EEPROM_REG_2, 0xdc },
	{ LP8860_EEPROM_REG_3, 0xf0 },
	{ LP8860_EEPROM_REG_4, 0xdf },
	{ LP8860_EEPROM_REG_5, 0xe5 },
	{ LP8860_EEPROM_REG_6, 0xf2 },
	{ LP8860_EEPROM_REG_7, 0x77 },
	{ LP8860_EEPROM_REG_8, 0x77 },
	{ LP8860_EEPROM_REG_9, 0x71 },
	{ LP8860_EEPROM_REG_10, 0x3f },
	{ LP8860_EEPROM_REG_11, 0xb7 },
	{ LP8860_EEPROM_REG_12, 0x17 },
	{ LP8860_EEPROM_REG_13, 0xef },
	{ LP8860_EEPROM_REG_14, 0xb0 },
	{ LP8860_EEPROM_REG_15, 0x87 },
	{ LP8860_EEPROM_REG_16, 0xce },
	{ LP8860_EEPROM_REG_17, 0x72 },
	{ LP8860_EEPROM_REG_18, 0xe5 },
	{ LP8860_EEPROM_REG_19, 0xdf },
	{ LP8860_EEPROM_REG_20, 0x35 },
	{ LP8860_EEPROM_REG_21, 0x06 },
	{ LP8860_EEPROM_REG_22, 0xdc },
	{ LP8860_EEPROM_REG_23, 0x88 },
	{ LP8860_EEPROM_REG_24, 0x3E },
};


static const struct lp8860_reg lp8860_reg_defs[] = {
	{ LP8860_DISP_CL1_BRT_MSB, 0x00},
	{ LP8860_DISP_CL1_BRT_LSB, 0x00},
	{ LP8860_DISP_CL1_CURR_MSB, 0x00},
	{ LP8860_DISP_CL1_CURR_LSB, 0x00},
	{ LP8860_CL2_BRT_MSB, 0x00},
	{ LP8860_CL2_BRT_LSB, 0x00},
	{ LP8860_CL2_CURRENT, 0x00},
	{ LP8860_CL3_BRT_MSB, 0x00},
	{ LP8860_CL3_BRT_LSB, 0x00},
	{ LP8860_CL3_CURRENT, 0x00},
	{ LP8860_CL4_BRT_MSB, 0x00},
	{ LP8860_CL4_BRT_LSB, 0x00},
	{ LP8860_CL4_CURRENT, 0x00},
	{ LP8860_CONFIG, 0x00},
	{ LP8860_FAULT_CLEAR, 0x00},
	{ LP8860_EEPROM_CNTRL, 0x80},
	{ LP8860_EEPROM_UNLOCK, 0x00},
};

//
typedef struct regmap_config
{
	uint8_t reg_bits;
	uint8_t val_bits;
	uint8_t max_register;
	uint8_t reg_defaults;
	uint8_t num_reg_defaults;
};


/************************************************************************************************************/
static int lp8860_init(struct lp8860_led *led);
static int lp8860_unlock_eeprom(struct lp8860_led *led, uint8_t lock);
static int lp8860_fault_check(struct lp8860_led *led);
static int lp8860_brightness_set(struct led_classdev *led_cdev, enum led_brightness brt_val);
/************************************************************************************************************/
/************************************************************************************************************/
static int lp8860_unlock_eeprom(struct lp8860_led *led, uint8_t lock)
{
	int ret;

	mutex_lock(&led->lock);

	if (lock == LP8860_UNLOCK_EEPROM) {
		ret = regmap_write(led->regmap,
			LP8860_EEPROM_UNLOCK,
			LP8860_EEPROM_CODE_1);
		if (ret) {
			dev_err(&led->client->dev, "EEPROM Unlock failed\n");
			goto out;
		}

		ret = regmap_write(led->regmap,
			LP8860_EEPROM_UNLOCK,
			LP8860_EEPROM_CODE_2);
		if (ret) {
			dev_err(&led->client->dev, "EEPROM Unlock failed\n");
			goto out;
		}
		ret = regmap_write(led->regmap,
			LP8860_EEPROM_UNLOCK,
			LP8860_EEPROM_CODE_3);
		if (ret) {
			dev_err(&led->client->dev, "EEPROM Unlock failed\n");
			goto out;
		}
	} else {
		ret = regmap_write(led->regmap,
			LP8860_EEPROM_UNLOCK,
			LP8860_LOCK_EEPROM);
	}

out:
	mutex_unlock(&led->lock);
	return ret;
}

static int lp8860_fault_check(struct lp8860_led *led)
{
	int ret, fault;
	unsigned int read_buf;

	ret = regmap_read(led->regmap, LP8860_LED_FAULT, &read_buf);
	if (ret)
		goto out;

	fault = read_buf;

	ret = regmap_read(led->regmap, LP8860_FAULT, &read_buf);
	if (ret)
		goto out;

	fault |= read_buf;

	/* Attempt to clear any faults */
	if (fault)
		ret = regmap_write(led->regmap, LP8860_FAULT_CLEAR,
			LP8860_CLEAR_FAULTS);
out:
	return ret;
}

static int lp8860_brightness_set(struct led_classdev *led_cdev,
				enum led_brightness brt_val)
{
	struct lp8860_led *led =
			container_of(led_cdev, struct lp8860_led, led_dev);
	int disp_brightness = brt_val * 255;
	int ret;

	mutex_lock(&led->lock);

	ret = lp8860_fault_check(led);
	if (ret) {
		dev_err(&led->client->dev, "Cannot read/clear faults\n");
		goto out;
	}

	ret = regmap_write(led->regmap, LP8860_DISP_CL1_BRT_MSB,
			(disp_brightness & 0xff00) >> 8);
	if (ret) {
		dev_err(&led->client->dev, "Cannot write CL1 MSB\n");
		goto out;
	}

	ret = regmap_write(led->regmap, LP8860_DISP_CL1_BRT_LSB,
			disp_brightness & 0xff);
	if (ret) {
		dev_err(&led->client->dev, "Cannot write CL1 LSB\n");
		goto out;
	}
out:
	mutex_unlock(&led->lock);
	return ret;
}

static int lp8860_init(struct lp8860_led *led)
{
	lp8860_led.lock=1;
	lp8860_led.pins->scl_pin=PB_13;
	lp8860_led.pins->sda_pin=PB_15;
	SoftI2c.pin_init(lp8860_led.pins);
	SoftI2c.devAddress=0x5A;
	
	ret = lp8860_fault_check(led);
	
	SoftI2c.reads(led.pins, 1, led.devAddress, address ,data,length);
	SoftI2c.writes(led.pins, 1, led.devAddress, address,data,length);
	

	ret = regmap_read(led->regmap, LP8860_STATUS, &read_buf);
	if (ret)
	{
		
	}

	ret = lp8860_unlock_eeprom(led, LP8860_UNLOCK_EEPROM);
	if (ret) 
	{
	}
	reg_count = ARRAY_SIZE(lp8860_eeprom_disp_regs) / sizeof(lp8860_eeprom_disp_regs[0]);
	for (i = 0; i < reg_count; i++)
	{
		ret = regmap_write(led->eeprom_regmap,
				lp8860_eeprom_disp_regs[i].reg,
				lp8860_eeprom_disp_regs[i].value);
		if (ret)
		{
			dev_err(&led->client->dev, "Failed writing EEPROM\n");
			break;
		}
	}

	ret = lp8860_unlock_eeprom(led, LP8860_LOCK_EEPROM);
	if (ret)
		goto out;

	ret = regmap_write(led->regmap,
			LP8860_EEPROM_CNTRL,
			LP8860_PROGRAM_EEPROM);
	if (ret) {
		dev_err(&led->client->dev, "Failed programming EEPROM\n");
		goto out;
	}

}




static const struct regmap_config lp8860_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,

	.max_register = LP8860_EEPROM_UNLOCK,
	.reg_defaults = lp8860_reg_defs,
	.num_reg_defaults = ARRAY_SIZE(lp8860_reg_defs),
};
static const struct reg_default lp8860_eeprom_defs[] = {
	{ LP8860_EEPROM_REG_0, 0x00 },
	{ LP8860_EEPROM_REG_1, 0x00 },
	{ LP8860_EEPROM_REG_2, 0x00 },
	{ LP8860_EEPROM_REG_3, 0x00 },
	{ LP8860_EEPROM_REG_4, 0x00 },
	{ LP8860_EEPROM_REG_5, 0x00 },
	{ LP8860_EEPROM_REG_6, 0x00 },
	{ LP8860_EEPROM_REG_7, 0x00 },
	{ LP8860_EEPROM_REG_8, 0x00 },
	{ LP8860_EEPROM_REG_9, 0x00 },
	{ LP8860_EEPROM_REG_10, 0x00 },
	{ LP8860_EEPROM_REG_11, 0x00 },
	{ LP8860_EEPROM_REG_12, 0x00 },
	{ LP8860_EEPROM_REG_13, 0x00 },
	{ LP8860_EEPROM_REG_14, 0x00 },
	{ LP8860_EEPROM_REG_15, 0x00 },
	{ LP8860_EEPROM_REG_16, 0x00 },
	{ LP8860_EEPROM_REG_17, 0x00 },
	{ LP8860_EEPROM_REG_18, 0x00 },
	{ LP8860_EEPROM_REG_19, 0x00 },
	{ LP8860_EEPROM_REG_20, 0x00 },
	{ LP8860_EEPROM_REG_21, 0x00 },
	{ LP8860_EEPROM_REG_22, 0x00 },
	{ LP8860_EEPROM_REG_23, 0x00 },
	{ LP8860_EEPROM_REG_24, 0x00 },
};

static const struct regmap_config lp8860_eeprom_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,

	.max_register = LP8860_EEPROM_REG_24,
	.reg_defaults = lp8860_eeprom_defs,
	.num_reg_defaults = ARRAY_SIZE(lp8860_eeprom_defs),
};
#endif

//
