/****************************************************************************************/
#include "app_timer.h"
#include "app_eeprom_24xx.h"
#include "app_button.h"
#include "app_motor_home.h"
/****************************************************************************************/
static rt_timer_t timer_5ms;
static rt_timer_t timer_10ms;

static int  timer_5ms_init(void);
static int  timer_10ms_init(void);

static void timeout_5ms(void *parameter);
static void timeout_10ms(void *parameter);

/*********************************************************************************************************************/
static int timer_init(void)
{
	timer_5ms = rt_timer_create("timer_5ms", 	timeout_5ms,		RT_NULL, 5,	RT_TIMER_FLAG_PERIODIC);
	
	timer_10ms = rt_timer_create("timer_10ms", timeout_10ms, 	RT_NULL, 10,RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER  );
	
  if (timer_5ms != RT_NULL) 	rt_timer_start(timer_5ms);		
  if (timer_10ms != RT_NULL) 	rt_timer_start(timer_10ms);	
	
	rt_kprintf("timer_init()...ok\n");
	return 0;
}
static void timeout_5ms(void *parameter)
{
//	for(uint8_t i=0;i<N_O_MOTORS;i++)
//	{			
//		MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
//		MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
//	}		
}
static void timeout_10ms(void *parameter)
{

}
/****************************************************************************/
void start_timer_5ms(void)
{
	rt_timer_start(timer_5ms);	
}
void stop_timer_5ms(void)
{
	rt_timer_stop(timer_5ms);	
}

void timer_hw_init(void)
{
	timer_5ms_init();
	timer_10ms_init();
}
//INIT_APP_EXPORT(timer_5ms_init);
//INIT_APP_EXPORT(timer_10ms_init);



