#include "tca6424a.h"	

#if 1
	#define TCA6424A_DEBUG 0

	#if  (	TCA6424A_DEBUG )
	#define TCA6424A_TRACE         rt_kprintf
	#else
	#define TCA6424A_TRACE(...)  				     
	#endif
#endif

uint8_t TCA6424A_init(TCA6424A_t *dev)
{
	SoftI2c.pin_init(dev->pins);
	
	if (SoftI2c.check(dev->pins,dev->devAddress) == 1)
	{
		TCA6424A_TRACE("iic device TCA6424A addr=%d......no found\n",dev->devAddress);
		return REPLY_CHKERR;
	}
	else
	{
		TCA6424A_TRACE("iic device TCA6424A addr=%d......found\n",dev->devAddress);
		TCA6424A_TRACE("out_data=%d\n",dev->out_data);
		TCA6424A_TRACE("&out_data=%d\n",&dev->out_data);
		TCA6424A_TRACE("(uint8_t *)dev->out_data=%d\n",(uint8_t *)dev->out_data);
		TCA6424A_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
		
		u8 reg_data=0xff; SoftI2c.writes(dev->pins,1,dev->devAddress,6,&reg_data,1); //Set p0 as input
		
		reg_data=0;		 SoftI2c.writes(dev->pins,1,dev->devAddress,4,&reg_data,1);		 //set input Polarity
									 SoftI2c.writes(dev->pins,1,dev->devAddress,3,dev->out_data,1);//set all outputs off
		reg_data=0;		 SoftI2c.writes(dev->pins,1,dev->devAddress,7,&reg_data,1);		 //set p1 as output
	}
	return REPLY_OK;
}

