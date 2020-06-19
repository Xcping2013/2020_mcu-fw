/********************************************************************************
 Author : CAC(Customer Applications Center) 

 Date :  June, 2013

 File name :  ADT7420Test.c

 Description :	Configure the regisiters of ADt7420, and read the convention data. 

 Hardware plateform : 	ADUc7026 DEMO-B
********************************************************************************/

#include "ADt7420.h"


int ADt7420_printT(void)
{
  static uint8_t adt7420_inited=0;
	
	char  tempString [10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	/*! Variables holding information about the device */
	unsigned char currentMode = 0;
	double temperature = 0;
	
	extern unsigned char resolutionSetting;
	
	if(adt7420_inited==0)
	{
    if(ADT7420_Init())
    {
      rt_kprintf("ADT7420 OK\n");
			
			currentMode=ADT7420_OP_MODE_CONT_CONV;
			ADT7420_SetOperationMode(currentMode);
		
			ADT7420_SetResolution(1);
			
			adt7420_inited=1;
    }		
	  else
    {
        rt_kprintf("ADT7420 Error\n");
    }
		
	}
		delay_ms(500);
		temperature = ADT7420_GetTemperature();
		
		sprintf(tempString,"%lf",temperature);			
		rt_kprintf("Temperature=%s  deg C\n",tempString);
}

MSH_CMD_EXPORT_ALIAS(ADt7420_printT,Temperature,...);
