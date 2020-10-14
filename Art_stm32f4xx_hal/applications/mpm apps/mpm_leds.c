

#include <mpm.h>

uint8_t led_rgb_pin[3]=
{
    LED_RED_PIN,
    LED_GREEN_PIN,
    LED_BLUE_PIN
};

void led_rgb_hw_init(void)
{
    for(uint8_t i=0;i<3;i++)
    {
        rt_pin_mode(led_rgb_pin[i], PIN_MODE_OUTPUT);
        rt_pin_write(led_rgb_pin[i], PIN_LOW);
    }
}

int led_rgb(int argc, char *argv[])
{
    static uint8_t led_rgb_inited=0;

    uint8_t ret = RT_EOK;

    if(led_rgb_inited==0)
    {
        led_rgb_inited=1;
        led_rgb_hw_init();
    }

    if (argc == 1)
    {
        rt_kprintf("led_rgb <r> <g> <b>\n");
				rt_kprintf("led_rgb <1|0> <1|0> <1|0>\n");
    }
    else if (argc == 4)
    {
        for(uint8_t i=0;i<3;i++)
        {
             rt_pin_write(led_rgb_pin[i],  atoi(argv[i+1]) ? PIN_HIGH : PIN_LOW);
        }   
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(led_rgb, led test);
