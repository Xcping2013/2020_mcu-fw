#include "bsp_pca9539a.h"	

#if 1
	#define PCA9539A_DEBUG 0

	#if  (	PCA9539A_DEBUG )
	#define PCA9539A_TRACE         rt_kprintf
	#else
	#define PCA9539A_TRACE(...)  				     
	#endif
#endif

uint8_t pca95xx_devAddR[4]={0xEC,0x4C,0x48,0xE8,};

uint8_t pca9539a_init0(pca9539a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......no found\n",dev.devAddress);
		return REPLY_CHKERR;
	}
	else
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......found\n",dev.devAddress);
		PCA9539A_TRACE("out_data=%d\n",dev.out_data);
		PCA9539A_TRACE("&out_data=%d\n",&dev.out_data);
		PCA9539A_TRACE("(uint8_t *)dev.out_data=%d\n",(uint8_t *)dev.out_data);
		PCA9539A_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
		
		u8 reg_data=0xff; SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_POLARITY_P0,&reg_data,1); //Set p0 as input
		
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,4,&reg_data,1);		 //set input Polarity
									 SoftI2c.writes(dev.pins,1,dev.devAddress,3,&dev.out_data,1);//set all outputs off
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,7,&reg_data,1);		 //set p1 as output
	}
	return REPLY_OK;
}
/*	debug info

		PCA9539A_TRACE("out_data=%d\n",dev.out_data);
		PCA9539A_TRACE("&out_data=%d\n",&dev.out_data);
		PCA9539A_TRACE("(uint8_t *)dev.out_data=%d\n",(uint8_t *)dev.out_data);
		PCA9539A_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
	

*/
uint8_t pca9539a_init(pca9539a_t dev)
{
	uint8_t reg_data=0xff; 												
	
	SoftI2c.pin_init(dev.pins);
	
	for(uint8_t i=0;i<4;i++)
	{
		dev.devAddress=pca95xx_devAddR[i];																												//轮询地址查找2种芯片4种可能的硬件状态
		
		if (SoftI2c.check(dev.pins,dev.devAddress) != 1)																					//应答正常
		{
			reg_data=0xff; 
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_CONFIG_P0,	 &reg_data,1); 					//Set p0 as input			
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_POLARITY_P0,&dev.invertFlag,1 );		//set input Polarity
			
			reg_data=0;		
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_OUTPUT_P1,&reg_data,1);						//pre-set all outputs off
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_CONFIG_P1,&reg_data,1);						//set p1 as output

			return REPLY_OK;
		}
		
	}
	return REPLY_CHKERR;	
}
//
