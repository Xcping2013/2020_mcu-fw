
#include "bsp_mcu_gpio.h"

/**********************************************************************************************************************/
static void mcu_udelay(uint32_t us);

/**********************************************************************************************************************/
static void mcu_udelay(uint32_t us)
{
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
/**********************************************************************************************************************/
void pinMode(uint8_t pin, uint8_t mode)
{
	rt_pin_mode(pin,mode);
}
//
void pinSet(uint8_t pin)
{
	rt_pin_write(pin, PIN_HIGH);
}
void pinReset(uint8_t pin)
{
	rt_pin_write(pin, PIN_LOW);
}
void pinWrite(uint8_t pin,uint8_t val)
{
	 if(val == 0)   pinReset(pin);
	 else						pinSet(pin);
}
uint8_t pinRead(uint8_t pin)
{
		if(rt_pin_read(pin)) return  1;
		else 								 return  0;
}
void pinToggle(uint8_t pin)
{
	rt_pin_write(pin, 1 - rt_pin_read(pin));
}

uint8_t shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
       if (bit_order == LSB_FIRST)      value |= pinRead(data_pin) << i;
       else									            value |= pinRead(data_pin) << (7 - i);
       pinWrite(	clock_pin,	LOW		);	mcu_udelay(1);
			 pinWrite(	clock_pin,	HIGH	);	mcu_udelay(1);
    }
    return value;
}

void shift_out(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
			if (bit_order == LSB_FIRST)
					pinWrite(	data_pin,	(!!(val & (1 << i))));
			else
					pinWrite(	data_pin,(!!(val & (1 << (7 - i))))	);

			pinWrite(	clock_pin,	HIGH	);
			pinWrite(	clock_pin,	LOW		);
	}
}
//


