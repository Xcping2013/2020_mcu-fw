
#include "bsp_mcu_delay.h"
#include "isl29125.h"
#include "inc_controller.h"

static uint8_t autoMode;

static const isl29125_mode_t modes[] = {
		ISL29125_MODE_RGB,
		ISL29125_MODE_R, ISL29125_MODE_G, ISL29125_MODE_B,
		ISL29125_MODE_RG, ISL29125_MODE_GB};

static const char* mode_names[] = {
		"ISL29125_MODE_RGB",
		"ISL29125_MODE_R", "ISL29125_MODE_G", "ISL29125_MODE_B",
		"ISL29125_MODE_RG", "ISL29125_MODE_GB"};
		
isl29125_t dev[2]=
{
	{
		.pin.scl_pin=PD_6,							//
		.pin.sda_pin=PD_7,							//
		.int_pin=PD_5,									//

		.mode=ISL29125_MODE_RGB,
		.range=ISL29125_RANGE_375,
		.res=ISL29125_RESOLUTION_16,
		
		.interrupt_status=ISL29125_INTERRUPT_STATUS_RED,
		.interrupt_persist=ISL29125_INTERRUPT_PERSIST_1,
		.interrupt_conven=ISL29125_INTERRUPT_CONV_DIS,
		.higher_threshold=8000,
		.lower_threshold=0,	
	},
	{
		.pin.scl_pin=PD_13,							//
		.pin.sda_pin=PD_12,							//
		.int_pin=PD_14,									//

		.mode=ISL29125_MODE_RGB,
		.range=ISL29125_RANGE_375,
		.res=ISL29125_RESOLUTION_16,
		
		.interrupt_status=ISL29125_INTERRUPT_STATUS_RED,
		.interrupt_persist=ISL29125_INTERRUPT_PERSIST_1,
		.interrupt_conven=ISL29125_INTERRUPT_CONV_DIS,
		.higher_threshold=8000,
		.lower_threshold=0,				
	}	
};

isl29125_rgb_t data[2];
color_rgb_t data8bit[2];

void isl29125_read_advaule(const isl29125_t *dev, isl29125_rgb_t *dest)
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
    printf(":%05i %05i %05i", red, green, blue);
}

int isl29125_devs_hw_init(void)
{
	memset(&data[0], 0x00, sizeof(data[0]));
	memset(&data[1], 0x00, sizeof(data[1]));

//	memcpy(&dev[1],&dev[0],sizeof(dev[0]));
//	dev[1].pin.scl_pin=PD_13;
//	dev[1].pin.sda_pin=PD_12;
//	dev[1].int_pin=PD_14;

	rt_kprintf("iic device isl29125 light sensor ");
	if (  (isl29125_init(&dev[0]) == 0) &&  (isl29125_init(&dev[1]) == 0) ) 
	{
		rt_kprintf("init......ok\n");
	}
	else
	{
		rt_kprintf("no found\n"); return 1;
	}
	isl29125_init_int(&dev[0]);
	isl29125_init_int(&dev[1]);
	return 0;
}
//ARRAY_SIZE(modes)
void lightSensor(int argc, char **argv)
{
    if (argc == 1)
    {
        rt_kprintf("\n");
			  rt_kprintf("lightSensor setMode <var>\n");
			  rt_kprintf("    var:0 ISL29125_MODE_RGB\n");
			  rt_kprintf("    var:1 ISL29125_MODE_R\n");
			  rt_kprintf("    var:2 ISL29125_MODE_G\n");
			  rt_kprintf("    var:3 ISL29125_MODE_B\n");
			  rt_kprintf("    var:4 ISL29125_MODE_RG\n");
		  	rt_kprintf("    var:5 ISL29125_MODE_GB\n");		
				rt_kprintf("lightSensor setRange <var>\n");
				rt_kprintf("    var:375 5.7m - 375 lux\n");
			  rt_kprintf("    var:10k 0.152 - 10,000 lux\n");
				rt_kprintf("lightSensor setResolution <var>\n");
				rt_kprintf("    var:12bit 0 - 4096\n");
			  rt_kprintf("    var:16bit 0 - 65536\n");
				rt_kprintf("lightSensor read_rgb_ADValue <num>\n");
			  rt_kprintf("    num:read data times\n");
				rt_kprintf("lightSensor read_rgb_lux <num>\n");
				rt_kprintf("    num:read data times\n");
				rt_kprintf("lightSensor read_rgb_color <num>\n");
				rt_kprintf("    num:read data times\n");
				rt_kprintf("lightSensor startStream <mode>\n");
				rt_kprintf("    mode:rgb\n");
				rt_kprintf("    mode:lux\n");
				rt_kprintf("    mode:ad\n");
				rt_kprintf("lightSensor stopStream\n");
        return ;
    }	
		else if (argc == 3)
		{
			if(!strcmp(argv[1], "setMode"))
			{
				uint8_t mode_temp=atoi(argv[2]);
				if(mode_temp<ARRAY_SIZE(modes))
				{
					rt_kprintf("Setting mode %s\n", mode_names[mode_temp]);
					dev[0].mode=modes[mode_temp];
					dev[1].mode=modes[mode_temp];
					isl29125_set_mode(&dev[0], modes[mode_temp]);
					isl29125_set_mode(&dev[1], modes[mode_temp]);			
				}
			}
			else if(!strcmp(argv[1], "setRange"))
			{
				uint8_t rang_temp=ISL29125_RANGE_375;
				if(!strcmp(argv[2], "375"))	rang_temp=ISL29125_RANGE_375;
				if(!strcmp(argv[2], "10k"))	rang_temp=ISL29125_RANGE_10K;
				dev[0].range=rang_temp;			dev[1].range=rang_temp;
				isl29125_init(&dev[0]);isl29125_init(&dev[1]);
			}			
			else if(!strcmp(argv[1], "setResolution"))
			{
				uint8_t rang_temp=ISL29125_RANGE_375;
				if(!strcmp(argv[2], "12bit"))	rang_temp=ISL29125_RESOLUTION_12;
				if(!strcmp(argv[2], "16bit"))	rang_temp=ISL29125_RESOLUTION_16;
				dev[0].res=rang_temp;			dev[1].res=rang_temp;
				isl29125_init(&dev[0]);isl29125_init(&dev[1]);
			}		
			else if(!strcmp(argv[1], "read_rgb_ADValue"))
			{
				uint8_t times_temp=atoi(argv[2]);
				
			  for(uint8_t i=0; i<times_temp;i++)
				{
					printf("ad1");
					isl29125_read_advaule(&dev[0], &data[0]);	printf(" ");	
					printf("ad2");
					isl29125_read_advaule(&dev[1], &data[1]);	printf("\n");	
					delay_ms(160);
				}
			}	
			else if(!strcmp(argv[1], "read_rgb_lux"))
			{
				uint8_t times_temp=atoi(argv[2]);
				
			  for(uint8_t i=0; i<times_temp;i++)
				{
					isl29125_read_rgb_lux(&dev[0], &data[0]);
					isl29125_read_rgb_lux(&dev[1], &data[1]);
					printf("lux1:%05i %05i %05i,",(int)data[0].red, (int)data[0].green, (int)data[0].blue);
					printf("lux2:%05i %05i %05i\n",(int)data[1].red, (int)data[1].green, (int)data[1].blue);
					delay_ms(160);
				}
			}			
			else if(!strcmp(argv[1], "read_rgb_color"))
			{
				uint8_t times_temp=atoi(argv[2]);
				
			  for(uint8_t i=0; i<times_temp;i++)
				{
					isl29125_read_rgb_color(&dev[0], &data8bit[0]);
					isl29125_read_rgb_color(&dev[1], &data8bit[1]);
					printf("RGB1:%03i %03i %03i ", data8bit[0].r, data8bit[0].g, data8bit[0].b);
					printf("RGB2:%03i %03i %03i\n", data8bit[1].r, data8bit[1].g, data8bit[1].b);
					delay_ms(160);
				}
			}	
			else if(!strcmp(argv[1], "startStream"))
			{
				if(!strcmp(argv[2], "rgb"))	autoMode=1;
				if(!strcmp(argv[2], "lux"))	autoMode=2;
				if(!strcmp(argv[2], "ad"))	autoMode=3;
			}	
		}
		else if(!strcmp(argv[1], "stopStream"))
		{
			autoMode=0;	
		}	

}

static void light_sensosr_entry(void *parameter)
{
//	isl29125_devs_hw_init();
	
	while (1)
	{	

		if(autoMode==1)
		{
			isl29125_read_rgb_color(&dev[0], &data8bit[0]);
			isl29125_read_rgb_color(&dev[1], &data8bit[1]);
			printf("RGB1:%03i %03i %03i ", data8bit[0].r, data8bit[0].g, data8bit[0].b);
			printf("RGB2:%03i %03i %03i\n", data8bit[1].r, data8bit[1].g, data8bit[1].b);	
		}
		else if(autoMode==2)
		{
			isl29125_read_rgb_lux(&dev[0], &data[0]);
			isl29125_read_rgb_lux(&dev[1], &data[1]);
			printf("lux1:%05i %05i %05i,",(int)data[0].red, (int)data[0].green, (int)data[0].blue);
			printf("lux2:%05i %05i %05i\n",(int)data[1].red, (int)data[1].green, (int)data[1].blue);
		}
		else if(autoMode==3)
		{
			printf("ad1");
			isl29125_read_advaule(&dev[0], &data[0]);	printf(" ");	
			printf("ad2");
			isl29125_read_advaule(&dev[1], &data[1]);	printf("\n");	
		}
		rt_thread_mdelay(160);
	}
}

int light_sensosr_thread_init(void)
{
    rt_thread_t tid;
    rt_err_t ret = RT_EOK;

		//isl29125_devs_hw_init();
	
    tid = rt_thread_create("isl29125",
                           light_sensosr_entry,
                           RT_NULL,
                           512,
                           21,
                           20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}
//INIT_APP_EXPORT(isl29125_devs_hw_init);
//INIT_APP_EXPORT(light_sensosr_thread_init);
//MSH_CMD_EXPORT(lightSensor,...);
//INIT_APP_EXPORT(isl29125_devs_hw_init);





