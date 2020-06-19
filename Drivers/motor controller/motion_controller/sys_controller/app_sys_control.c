/* 	C内置宏
ANSI C标准中有几个标准预定义宏（也是常用的）：

__DATE__：在源文件中插入当前的编译日期

__TIME__：在源文件中插入当前编译时间；

__FILE__：在源文件中插入当前源文件名；

__LINE__：在源代码中插入当前源代码行号；

__FUNCTION__:在源代码中插入当前所在函数名称；

__STDC__：当要求程序严格遵循ANSI C标准时该标识被赋值为1；

__cplusplus：当编写C++程序时该标识符被定义
*/
//rt_show_version
#include "app_sys_control.h"
/*******************************************************************************************************/
static void press_overhigh_check(void *args);
static void init_input8_as_exti(void);
/***************************************************************************/
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

void xstrcat(char *str1,char *str2)
{
	int i,len1;
	for(i=0;str1[i]!='\0';i++);
	len1=i;
	for(i=0;str2[i]!='\0';i++)
	str1[i+len1]=str2[i];
}

/*
Bx35_button: Y轴先里后出 ---> Z轴先上后下 ---> X轴先左后右

B445_button: Z轴先上后下回原点后再往上运行一段位置放置传感器撞到Holder---> Y轴先里后出---> X轴先左后右

ROAD			 ：Z轴先上后下				
	
*/

static void hw_in_out_debug(void)
{
	u8 ret_ok=1;
													if(INPUT1==1) 	{rt_kprintf("input1 or rgb_5v ............err\n");ret_ok=0;}
	RGB_R=1;RGB_G=0;RGB_B=0;delay_ms(10);
													if(INPUT2==1) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}
	RGB_R=0;RGB_G=1;RGB_B=0;delay_ms(10);										        
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==1) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}
	RGB_R=0;RGB_G=0;RGB_B=1;delay_ms(10);												     
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==1) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}	
														
	OUTPUT1=1;delay_ms(10);	if(INPUT5==1) 	{rt_kprintf("input5 or output1 ...........err\n");		ret_ok=0;}
	OUTPUT1=0;delay_ms(10);	if(INPUT5==0) 	{rt_kprintf("input5 or output1 ...........err\n");		ret_ok=0;}
	                                                                  
	OUTPUT2=1;delay_ms(10);	if(INPUT6==1) 	{rt_kprintf("input6 or output2 ...........err\n");   ret_ok=0;}
	OUTPUT2=0;delay_ms(10);	if(INPUT6==0) 	{rt_kprintf("input6 or output2 ...........err\n");   ret_ok=0;}
	                                                                   
	OUTPUT3=1;delay_ms(10);	if(INPUT7==1) 	{rt_kprintf("input7 or output3 ...........err\n");   ret_ok=0;}
	OUTPUT3=0;delay_ms(10);	if(INPUT7==0) 	{rt_kprintf("input7 or output3 ...........err\n");   ret_ok=0;}
	                                                                  
	OUTPUT4=1;delay_ms(10);	if(INPUT8==1) 	{rt_kprintf("input8 or output4 ...........err\n");   ret_ok=0;}
	OUTPUT4=0;delay_ms(10);	if(INPUT8==0) 	{rt_kprintf("input8 or output4 ...........err\n");   ret_ok=0;}
	          
	OUTPUT5=1;delay_ms(10);	if(INPUT9==1) 	{rt_kprintf("input9 or output5 ...........err\n");   ret_ok=0;}
	OUTPUT5=0;delay_ms(10);	if(INPUT9==0) 	{rt_kprintf("input9 or output5 ...........err\n");   ret_ok=0;}
            
	OUTPUT6=1;delay_ms(10);	if(INPUT10==1) 	{rt_kprintf("input10 or output6 ..........err\n");   ret_ok=0;}
	OUTPUT6=0;delay_ms(10);	if(INPUT10==0) 	{rt_kprintf("input10 or output6 ..........err\n");   ret_ok=0;}
	          
	OUTPUT7=1;delay_ms(10);	if(INPUT11==1) 	{rt_kprintf("input11 or output7 ..........err\n");   ret_ok=0;}
	OUTPUT7=0;delay_ms(10);	if(INPUT11==0) 	{rt_kprintf("input11 or output7 ..........err\n");   ret_ok=0;}
																									 
	OUTPUT8=1;delay_ms(10);	if(INPUT12==1) 	{rt_kprintf("input12 or output8 ..........err\n");   ret_ok=0;}   
	OUTPUT8=0;delay_ms(10);	if(INPUT12==0) 	{rt_kprintf("input12 or output8 ..........err\n");   ret_ok=0;}   
	if(ret_ok)	rt_kprintf("inputs and outputs test......ok\n");	
	
}

static void hw_eeprom_debug(void)
{
	u8 *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,100,save_info,len);
	delay_ms(10);
	at24cxx.read(at24c256 , 100, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\n");
  }	
	else rt_kprintf("ok\n");
	
}

static void hw_motor_debug(void)
{
	rt_kprintf("tmc429 test..................");
	if(Read429SingleByte(IDX_REF_SWITCHES, 3)==0) 
		rt_kprintf("err");

	else 
	{		

		rt_kprintf("ok\n\n最后:检测限位采集电路");
		rt_kprintf("\n先发送motor get leftLimit 1 采集数据再把L1接GND后发送motor get leftLimit 1返回值有变化才ok\n");	

	}
}

static void board_debug(void)
{
	rt_kprintf("\n");
	hw_in_out_debug();
	hw_eeprom_debug();
	hw_motor_debug();	
}

//
static void press_overhigh_check(void *args)
{
	if(INPUT8==0 && buttonRESETpressed==0 && homeInfo.GoHome[2]==FALSE)	 	
	{			
		closeSerial();

		HardStop(2);//HardStop(1);HardStop(0);

		homeInfo.Homed[2]=FALSE;
		homeInfo.GoHome[2]=FALSE;	
		homeInfo.GoLimit[2]=FALSE;	

		rt_kprintf("stop motor z due to pressure overhigh!!!\n");	
	} 
}
static void init_input8_as_exti(void)
{
	rt_pin_attach_irq(inputs_pin_num[8-1], PIN_IRQ_MODE_FALLING, press_overhigh_check, RT_NULL);
	rt_pin_irq_enable(inputs_pin_num[8-1], PIN_IRQ_ENABLE);
	
	rt_kprintf("init_input8_as_exti()...ok\n");
}




MSH_CMD_EXPORT(board_debug, list device in system);
//INIT_APP_EXPORT(init_input8_as_exti);
//





