#include "bsp_include.h"
 
#define HC595_DATA PB_15
#define HC595_CLK  PB_13
#define HC595_LCK  PA_4

uint8_t 	hc595_out[2]={0xff,0};

void hc595_init(void)
{
	rt_pin_mode(HC595_DATA, PIN_MODE_OUTPUT);
	rt_pin_mode(HC595_CLK, PIN_MODE_OUTPUT);
	rt_pin_mode(HC595_LCK, PIN_MODE_OUTPUT);
	
	rt_pin_write(HC595_DATA, PIN_HIGH);
	rt_pin_write(HC595_CLK, PIN_HIGH);
	rt_pin_write(HC595_LCK, PIN_HIGH);
}

//Updates the output register
void hc595_write(uint8_t *data, uint8_t data_length, uint8_t bit_oder)
{
	int i;
	rt_pin_write(HC595_LCK, PIN_LOW);
	for(i = 0; i < data_length; i++)
	{
		shift_out(HC595_DATA, HC595_CLK, bit_oder, data[i]);
	}
}
//Enable the output register
void hc595_updata(void)
{
	rt_pin_write(HC595_LCK, PIN_LOW);
	rt_hw_us_delay(2);
	rt_pin_write(HC595_LCK,PIN_HIGH);
}

#ifdef INC_MB1616003

static void output_init(void)
{
	rt_pin_mode(PB_11,PIN_MODE_OUTPUT);
	rt_pin_mode(PB_10,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_13,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_14,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_15,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_0,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_1,PIN_MODE_OUTPUT);
	rt_pin_mode(PC_2,PIN_MODE_OUTPUT);
	
	hc595_init();
	hc595_write(&hc595_out[0],2,MSB_FIRST);
	hc595_updata();		
}
static void output_write(uint8_t ch , uint8_t state)
{
	if(0<ch && ch< 17)
	{
		switch(ch)
		{
			case 1:			rt_pin_write(PB_11,state);
				break;	            
			case 2:			rt_pin_write(PB_10,state);
				break;	               
			case 3:			rt_pin_write(PC_13,state);
				break;	               
			case 4:			rt_pin_write(PC_14,state);
				break;				       
			case 5:			rt_pin_write(PC_15,state);
				break;	              
			case 6:			rt_pin_write(PC_0,state);
				break;		             
			case 7:			rt_pin_write(PC_1,state);
				break;                 
			case 8:			rt_pin_write(PC_2,state);
				break;	
			
			case 9:			
			case 10:		
			case 11:		
			case 12:						
			case 13:			
			case 14:				
			case 15:			
			case 16:		if(state) hc595_out[1] |= (0x80>>(16-ch));		//0000 0001 ~ 1000 0000 置位设置通道
									else   		hc595_out[1] &= ((0x7f >> (16-ch)) | (0x7f << (8 -(16-ch))));//循环右移 1111 1110 ~ 0111 1111 复位设置通道
									hc595_write(&hc595_out[0],2,MSB_FIRST);				//2个595传输设置值
									hc595_updata();																//输出设置值
				break;
			
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
								if(state==0)   hc595_out[0] |= (0x80>>(24-ch));
									else   		hc595_out[0] &= ((0x7f >> (24-ch)) | (0x7f << (8 -(24-ch))));//循环右移
									hc595_write(&hc595_out[0],2,MSB_FIRST);
									hc595_updata();
				break;
		
			default:
				break;		
		}
		rt_kprintf("<ok>");
	}
}
static void output_writeAll(uint8_t * states)
{
	uint8_t i;
	if(strlen(states)==16)
	{
		rt_pin_write(PB_11,states[0]-'0');
		rt_pin_write(PB_10,states[1]-'0');
		rt_pin_write(PC_13,states[2]-'0');
		rt_pin_write(PC_14,states[3]-'0');
		rt_pin_write(PC_15,states[4]-'0');
		rt_pin_write(PC_0,states[5]-'0');
		rt_pin_write(PC_1,states[6]-'0');
		rt_pin_write(PC_2,states[7]-'0');
		for(i=0;i<8;i++)
		{
			if(states[8+i]=='1') 	hc595_out[0] |= (0x80>>(7-i));
			else   								hc595_out[0] &= (0x7f>>(7-i));
			hc595_write(&hc595_out[0],1,MSB_FIRST);	
			hc595_updata();
		}
		rt_kprintf("<ok>");
	}
}
INIT_APP_EXPORT(output_init);

FINSH_FUNCTION_EXPORT_ALIAS(output_write, output, write output[1 - 16] on[1] or off[0].)
FINSH_FUNCTION_EXPORT_ALIAS(output_writeAll, outputs, write outputs on["1"] or off["0"].)

#endif

//
