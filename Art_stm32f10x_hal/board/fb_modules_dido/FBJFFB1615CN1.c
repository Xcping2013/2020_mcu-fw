
#include "FBJFFB1615CN1.h"	
	
#include "inc_dido.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif
pca95xx_t FBJFFB1615CN1_pca95xx1=
{
	//{DEV0PA1_PIN,DEV0PA2_PIN},
	//0xEC ---PCA9539
	//0x4C ---PCA9555
	{PD_6,PD_7},
	0x4C,	
	0,
	0,
	0,
};
pca95xx_t FBJFFB1615CN1_pca95xx2=
{
	{PD_4,PB_3},
	0xEC,	
	0,
	0,
	0,
};
uint8_t FBJFFB1615CN1_is_inited=0;
/****************************************************************************************/
void FBJFFB1615CN1_Init(void)
{ 
	if(FBJFFB1615CN1_is_inited)	{;}
	else 
	{
		if(	pca95xx_init(&FBJFFB1615CN1_pca95xx1)==REPLY_OK && pca95xx_init(&FBJFFB1615CN1_pca95xx2)==REPLY_OK  )
		{
			FBJFFB1615CN1_is_inited=1;
			rt_kprintf("two iic device pca95xx found on FBJFFB1615CN1\n");
		}
	}
}
//
uint8_t FBJFFB1615CN1GetP0(uint8_t whichIIC)
{
	uint8_t retdata=0;
	
	if(whichIIC==1)
	{
		SoftI2c.reads(FBJFFB1615CN1_pca95xx1.pins,1,FBJFFB1615CN1_pca95xx1.devAddress,PCA95xx_REG_INPUT_P0, &FBJFFB1615CN1_pca95xx1.in_data ,1);	
		retdata = FBJFFB1615CN1_pca95xx1.in_data;
	}
	else 	if(whichIIC==2)
	{
		SoftI2c.reads(FBJFFB1615CN1_pca95xx2.pins,1,FBJFFB1615CN1_pca95xx2.devAddress,PCA95xx_REG_INPUT_P0, &FBJFFB1615CN1_pca95xx2.in_data ,1);	
		retdata = FBJFFB1615CN1_pca95xx2.in_data;
	}
	return retdata;
}

uint8_t FBJFFB1615CN1GetP0Bit(uint8_t whichIIC, uint8_t BitPosition)
{
	return ((FBJFFB1615CN1GetP0(whichIIC)) & (1 << (7-BitPosition)))? 1:0;
}
void FBJFFB1615CN1SetP1(uint8_t whichIIC, uint8_t P0data)
{
	if(whichIIC==1)
	{
		FBJFFB1615CN1_pca95xx1.out_data=P0data;	
		SoftI2c.writes(FBJFFB1615CN1_pca95xx1.pins,1,FBJFFB1615CN1_pca95xx1.devAddress,PCA95xx_REG_OUTPUT_P1,&FBJFFB1615CN1_pca95xx1.out_data,1);	

	}
	else 	if(whichIIC==2)
	{
		FBJFFB1615CN1_pca95xx2.out_data=P0data;	
		SoftI2c.writes(FBJFFB1615CN1_pca95xx2.pins,1,FBJFFB1615CN1_pca95xx2.devAddress,PCA95xx_REG_OUTPUT_P1,&FBJFFB1615CN1_pca95xx2.out_data,1);	
	}	
}
void FBJFFB1615CN1SetP1Bit(uint8_t whichIIC, uint8_t BitPosition,  uint8_t Bitdata)
{
	if(whichIIC==1)
	{
		FBJFFB1615CN1_pca95xx1.out_data= Bitdata? FBJFFB1615CN1_pca95xx1.out_data | (1 << (7-BitPosition)) : FBJFFB1615CN1_pca95xx1.out_data & ((uint8_t) ~(1 << (7-BitPosition)));
		SoftI2c.writes(FBJFFB1615CN1_pca95xx1.pins,1,FBJFFB1615CN1_pca95xx1.devAddress,PCA95xx_REG_OUTPUT_P1,&FBJFFB1615CN1_pca95xx1.out_data,1);	
	}
	else if(whichIIC==2)
	{
		FBJFFB1615CN1_pca95xx2.out_data= Bitdata? FBJFFB1615CN1_pca95xx2.out_data | (1 << BitPosition) : FBJFFB1615CN1_pca95xx2.out_data & ((uint8_t) ~(1 << BitPosition));
		SoftI2c.writes(FBJFFB1615CN1_pca95xx2.pins,1,FBJFFB1615CN1_pca95xx2.devAddress,PCA95xx_REG_OUTPUT_P1,&FBJFFB1615CN1_pca95xx2.out_data,1);	
	}
}
uint8_t FBJFFB1615CN1GetP1(uint8_t whichIIC)
{
	if(whichIIC==1) 	return FBJFFB1615CN1_pca95xx1.out_data;
	if(whichIIC==2) 	return FBJFFB1615CN1_pca95xx2.out_data;
	return 0;
}
uint8_t FBJFFB1615CN1GetP1Bit(uint8_t whichIIC , uint8_t BitPosition)
{
	if(whichIIC==1) 	return (FBJFFB1615CN1_pca95xx1.out_data & (1 << (7-BitPosition)))? 1:0;
	if(whichIIC==2) 	return (FBJFFB1615CN1_pca95xx2.out_data & (1 << BitPosition))? 1:0;
	return 0;
}



//
	
	










