
#include "inc_controller.h"	
#include "motion_control.h"
#include "tmc429_motion_cmd.h"
#include "app_button.h"

enum ResetStep {

no_need_home,
x_go_home, 
x_is_ok, 
y_go_home, 
y_is_ok, 
z_go_home, 
z_is_ok, 
all_is_ok,
	
};

uint8_t motorResetStep=no_need_home;


uint8_t motorsResetOK=0;
uint8_t motorsResetInOrder=0; 
uint8_t motorIsResetting=0;

void ButtonReset_process(void)
{
	motorsResetInOrder=TRUE;
	buttonRESETpressed=TRUE;
}

int motion(int argc, char **argv)
{
	int result = REPLY_OK;
	if (argc == 2 )	
	{
		if 	    (!strcmp(argv[1], "reset"))	  	{	motorsResetInOrder=TRUE;}
		else	if(!strcmp(argv[1], "stopreset"))	{	motorsResetInOrder=FALSE;}//motorSrop(n)}	
		else	if(!strcmp(argv[1], "reset?"))		{	rt_kprintf("motion reset=%d\n",motorsResetOK) ;}	
		else result = REPLY_CHKERR;
	}
	else result = REPLY_INVALID_CMD;
	if(result != REPLY_OK)
	{
		rt_kprintf("motion reset                 - motion reset\n");	
		rt_kprintf("motion stopreset             - motion reset\n");	
		rt_kprintf("motion reset?                - is fixture reset ok or not\n");					
	}
	return result;
}

void motorResetByProID( void )
{			
	if(motorsResetInOrder==TRUE)	//只配置一次，使能相关
	{
		motorsResetInOrder=FALSE;		//outputSet(1,0);			outputSet(2,1);	 
		
		motorsResetOK=FALSE;
		
		for(uint8_t i=0;i<N_O_MOTORS;i++)
		{
			StopMotorByRamp(i);
			HomeInfo[i].Homed=FALSE;
			HomeInfo[i].GoHome=FALSE;
			HomeInfo[i].GoLimit=FALSE;
		}	
		delay_ms(20);	
		if(g_tParam.Project_ID==OQC_FLEX || g_tParam.Project_ID==BUTTON_ROAD )
		{
				motorResetStep=z_go_home;		
		}
	}
	switch(motorResetStep)
	{
		case x_go_home:		motor_gohome_config(0,g_tParam.speed_home[0]);  motorResetStep=x_is_ok;					
			break;
		case y_go_home:		motor_gohome_config(1,-g_tParam.speed_home[1]);	motorResetStep=y_is_ok;		
			break;		
		case z_go_home:		motor_gohome_config(2,-g_tParam.speed_home[2]);	motorResetStep=z_is_ok;	
			break;
		
		case x_is_ok:		
			
				if(HomeInfo[0].Homed==TRUE)
				{
					if(g_tParam.Project_ID==BUTTON_ROAD)	motorResetStep=all_is_ok;				
				}				
			break;
		
		case y_is_ok:		
			   
				if(HomeInfo[1].Homed==TRUE) 	motorResetStep=all_is_ok;
			break;		
		
		case z_is_ok:	
			
				if(HomeInfo[2].Homed==TRUE)
				{
					if(g_tParam.Project_ID==OQC_FLEX)						  motorResetStep=all_is_ok;
					else if(g_tParam.Project_ID==BUTTON_ROAD)			motorResetStep=x_go_home;										
				}
			break;	
		
		case all_is_ok:	
						
						//buttonRESETpressed=FALSE;
						//outputSet(2,0);	
						rt_kprintf("motor auto reset ok\n");
						
						motorResetStep=no_need_home;	
						
						buttonRESETpressed=FALSE;
		
						motorsResetOK=TRUE;
			break;
		
		case no_need_home:	
			break;
		default:
			break;	
	}
}	










