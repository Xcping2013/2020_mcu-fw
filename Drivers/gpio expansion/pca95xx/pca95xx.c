#include "pca95xx.h"	

//1 1 1 0  1	 A1 A0  RW	  9539
//0 1 0 0 1/A2 A1 A0	R/W   9555
#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
	#endif
#endif

uint8_t pca95xx_devAddR[4]={0xEC,0x4C,0x48,0xE8,};

/*	debug info

		DBG_TRACE("out_data=%d\n",dev.out_data);
		DBG_TRACE("&out_data=%d\n",&dev.out_data);
		DBG_TRACE("(uint8_t *)dev.out_data=%d\n",(uint8_t *)dev.out_data);
		DBG_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
*/

//函数的参数不能变改变
uint8_t pca95xx_init(pca95xx_t *dev)					//default		P0-INPUT 	P1-OUTPUT
{
	uint8_t reg_data=0xff; 												
	
	SoftI2c.pin_init(dev->pins);
	
	for(uint8_t i=0;i<2;i++)								 //如果同样的IIC接多个设备，会重复初始化设备1
	{		
		if (SoftI2c.check(dev->pins,dev->devAddress) == REPLY_OK)																					//应答正常
		{
			DBG_TRACE("search dev.devAddress=0x%x\n",dev->devAddress);
			reg_data=0xff; 
			SoftI2c.writes(dev->pins,1,dev->devAddress,PCA95xx_REG_CONFIG_P0,	 &reg_data,1); 					//Set p0 as input			
			SoftI2c.writes(dev->pins,1,dev->devAddress,PCA95xx_REG_POLARITY_P0,&dev->invertFlag,1 );		//set input Polarity
			
			reg_data=0;		
			SoftI2c.writes(dev->pins,1,dev->devAddress,PCA95xx_REG_OUTPUT_P1,&reg_data,1);						//pre-set all outputs off
			SoftI2c.writes(dev->pins,1,dev->devAddress,PCA95xx_REG_CONFIG_P1,&reg_data,1);						//set p1 as output

			return REPLY_OK;
		}	
		else														//以下赋值并不会传入到参数的结构体中
		{
			if(dev->devAddress==0x48) 			{dev->devAddress=0xE8;}																				 //轮询地址查找2种芯片2种可能的硬件地址
			else if(dev->devAddress==0x4C) {dev->devAddress=0xEC;}			
			else if(dev->devAddress==0xE8) {dev->devAddress=0x48;}																				 //轮询地址查找2种芯片2种可能的硬件地址
			else if(dev->devAddress==0xEC) {dev->devAddress=0x4C;}	
			
			DBG_TRACE("change dev.devAddress=0x%x\n",dev->devAddress);
		}
	}
	return REPLY_CHKERR;	
}

uint8_t pca95xx_init0(pca95xx_t dev)					//default		P0-INPUT 	P1-OUTPUT
{
	uint8_t reg_data=0xff; 												
	
	SoftI2c.pin_init(dev.pins);
	
	for(uint8_t i=0;i<2;i++)								 //如果同样的IIC接多个设备，会重复初始化设备1
	{		
		if (SoftI2c.check(dev.pins,dev.devAddress) == REPLY_OK)																					//应答正常
		{
			DBG_TRACE("search dev.devAddress=0x%x\n",dev.devAddress);
			reg_data=0xff; 
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_CONFIG_P0,	 &reg_data,1); 					//Set p0 as input			
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_POLARITY_P0,&dev.invertFlag,1 );		//set input Polarity
			
			reg_data=0;		
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_OUTPUT_P1,&reg_data,1);						//pre-set all outputs off
			SoftI2c.writes(dev.pins,1,dev.devAddress,PCA95xx_REG_CONFIG_P1,&reg_data,1);						//set p1 as output

			return REPLY_OK;
		}	
		else														//以下赋值并不会传入到参数的结构体中,参数被赋值需要使用指针！！！！
		{
			if(dev.devAddress==0x48) 			{dev.devAddress=0xE8;}																				 //轮询地址查找2种芯片2种可能的硬件地址
			else if(dev.devAddress==0x4C) {dev.devAddress=0xEC;}			
			else if(dev.devAddress==0xE8) {dev.devAddress=0x48;}																				 //轮询地址查找2种芯片2种可能的硬件地址
			else if(dev.devAddress==0xEC) {dev.devAddress=0x4C;}	
			
			DBG_TRACE("change dev.devAddress=0x%x\n",dev.devAddress);
		}
	}
	return REPLY_CHKERR;	
}
//
