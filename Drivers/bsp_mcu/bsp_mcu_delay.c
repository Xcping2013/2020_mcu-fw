#include "bsp_mcu_delay.h"	

//rt_thread_delay
//rt_thread_sleep
//rt_hw_us_delay
//rt_thread_mdelay

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DEG_TRACE		rt_kprintf
	#else
	#define DEG_TRACE(...)		
#endif

#endif

////��Ҫ�߾��ȵ�ʱ��ʹ��Ӳ����ʱ��    �ж��ﲻ������rt_thread_delay

void drv_mdelay(uint16_t nms)
{
	drv_udelay(nms*1000);
}

void drv_udelay(uint32_t us)
{
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
//�����ڹر��жϺ����
void delay_us1(int us)
{
	rt_uint32_t delta;
	/* �����ʱ������tick�� */
	us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
	/* ��õ�ǰʱ�� */
	delta = SysTick->VAL;
	/* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
	while (SysTick->VAL-delta > us);
}
void delay_ms(u16 nms)
{
	//rt_thread_delay(nms/10);
	drv_udelay(nms*1000);
}
/*
void SysTick_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	SysTick->LOAD=9000;
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   						//����SYSTICK�ж�
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   						//����SYSTICK    

}

*/
//�����ڹر��жϺ����
void delay_us(int us)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//1ms��ʱ��Ҫ��LOADֵ 	T 
	ticks=us*72;//*9; 											//  		 
	tcnt=0;

	told=SysTick->VAL;        			  //�ս���ʱ�ļ�����ֵ		T0
	while(1)
	{
		tnow=SysTick->VAL;							//����ʱ�̵�ʵʱ��ֵ	  T1
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����T0-T1=��T1
			else tcnt+=reload-tnow+told;	//��T0=1	T1=9000	��T1=T0+T-T1   
			told=tnow;										//��T=��T1+��Tn
			if(tcnt>=ticks)break;				  //��Tʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};						    
}





#if 	( DBG_ENABLE )

#include "bsp_mcu_gpio.h"	

int delay_us_test(int argc, char **argv)
{
		pinSet(PD_3);
    if (argc == 2)
    {	
			 drv_udelay(atoi(argv[1]));		//22US  2.2US
			 pinReset(PD_3);							//2.5us
			
			 pinSet(PD_3);
			 rt_hw_us_delay(atoi(argv[1]));		//16US  1.6US
			 pinReset(PD_3);
			
       return RT_EOK;	
		}
		return RT_ERROR;
		
}
void getclock(void)
{
	
	rt_kprintf("SystemCoreClock=%d\n",SystemCoreClock);
	
}
MSH_CMD_EXPORT_ALIAS(getclock, Clock,...);

MSH_CMD_EXPORT_ALIAS(delay_us_test, delay,...);

#endif

//
