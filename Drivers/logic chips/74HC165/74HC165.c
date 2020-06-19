
#include "bsp_include.h"

#define HC165_DATA PC_5
#define HC165_CLK  PA_1
#define HC165_SCK  PB_0
#define HC165_LOAD PC_4
 
static void hc165_init(void)
{
	rt_pin_mode(HC165_DATA ,	 PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(HC165_CLK ,	 	 PIN_MODE_OUTPUT);
	rt_pin_mode(HC165_LOAD ,   PIN_MODE_OUTPUT);
	rt_pin_mode(HC165_SCK , 	 PIN_MODE_OUTPUT);
	
	rt_pin_write(HC165_SCK , 	PIN_HIGH);
	rt_pin_write(HC165_LOAD , PIN_HIGH);
	rt_pin_write(HC165_CLK ,  PIN_HIGH);
	rt_pin_write(HC165_SCK ,  PIN_LOW);
}

static void hc165_read(uint8_t *data, uint8_t data_length, uint8_t bit_oder)
{
	u8 i;
	for(i=0;i<data_length;i++)
	{
		rt_pin_write(HC165_LOAD,PIN_LOW);
		rt_hw_us_delay(2);
		rt_pin_write(HC165_LOAD,PIN_HIGH);
		rt_hw_us_delay(2);
		*data++=shift_in(HC165_DATA, HC165_CLK, bit_oder);
	}
}

#ifdef INC_MB1616003

uint8_t data_read[20];

static void input_init(void)
{
	rt_pin_mode(PE_0,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_1,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_2,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_3,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_4,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_5,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_6,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PE_7,PIN_MODE_INPUT_PULLUP);
	
	hc165_init();
}
//

static void input_gets(void)
{
	uint8_t i;
	uint8_t	hc165_in;
	data_read[0]=rt_pin_read(PE_0)? '1':'0';
	data_read[1]=rt_pin_read(PE_1)? '1':'0';
	data_read[2]=rt_pin_read(PE_2)? '1':'0';
	data_read[3]=rt_pin_read(PE_3)? '1':'0';
	data_read[4]=rt_pin_read(PE_4)? '1':'0';
	data_read[5]=rt_pin_read(PE_5)? '1':'0';
	data_read[6]=rt_pin_read(PE_6)? '1':'0';
	data_read[7]=rt_pin_read(PE_7)? '1':'0';
	hc165_read(&hc165_in,1,LSB_FIRST);
	for(i=0;i<8;i++)	data_read[i+8]=(hc165_in>>(7-i))&0x01 ? '1':'0';		
	
//	rt_kprintf("<%.16s>",data_read);

	
}
static void input_reads(void)
{
	input_gets();
	rt_kprintf("<%.16s>",data_read);
}

static void input_read(uint8_t ch)
{
	if(0<ch && ch<17) 
	{
		input_gets();
		rt_kprintf("<%d>",data_read[ch-1]=='0' ? 0:1);	
	}
}

INIT_APP_EXPORT(input_init);

FINSH_FUNCTION_EXPORT_ALIAS(input_reads, inputs,read all input on[1] or off[0].)
FINSH_FUNCTION_EXPORT_ALIAS(input_read, input,read input[1 - 16] on[1] or off[0].)

#endif




