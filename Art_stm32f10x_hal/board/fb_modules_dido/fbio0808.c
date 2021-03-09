
#include "fbio0808.h"	
	
#include "inc_dido.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif
pca95xx_t fbio0808_pca95xx=
{
	{PD_7,PD_6},
	0xEC,//0x4C,	
	0,
	0xff,
	0,
};

uint8_t FBIO0808_is_inited=0;
/****************************************************************************************/
void FBIO0808_Init(void)
{ 
	if(FBIO0808_is_inited)	{;}
	else if(	pca95xx_init(&fbio0808_pca95xx)==REPLY_OK)
	{
		FBIO0808_is_inited=1;
		rt_kprintf("iic device pca95xx found on FBIO0808 addr=0x%x\n",fbio0808_pca95xx.devAddress);
//
		
	}
//	else 
//	{
//		if(	pca95xx_init(fbio0808_pca95xx)!=REPLY_OK)
//		{
//			fbio0808_pca95xx.devAddress=0xEC;
//			if(	pca95xx_init(fbio0808_pca95xx)==REPLY_OK)
//			{
//				FBIO0808_is_inited=1;
//				rt_kprintf("iic device pca95xx found on FBIO0808 addr=0x%x\n",fbio0808_pca95xx.devAddress);
//			}	
//		}
//		else 
//		{
//				FBIO0808_is_inited=1;
//				rt_kprintf("iic device pca95xx found on FBIO0808 addr=0x%x\n",fbio0808_pca95xx.devAddress);		
//		}
//	}
}
//
uint8_t FBIO0808GetP0(void)
{
	SoftI2c.reads(fbio0808_pca95xx.pins,1,fbio0808_pca95xx.devAddress,PCA95xx_REG_INPUT_P0, &fbio0808_pca95xx.in_data ,1);	
	return fbio0808_pca95xx.in_data;
}

uint8_t FBIO0808GetP0Bit(uint8_t BitPosition)
{
	return ((FBIO0808GetP0()) & (1 << BitPosition))? 1:0;
}
void FBIO0808SetP1(uint8_t P1data)
{
	fbio0808_pca95xx.out_data=P1data;	
	SoftI2c.writes(fbio0808_pca95xx.pins,1,fbio0808_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&fbio0808_pca95xx.out_data,1);	
}
void FBIO0808SetP1Bit(uint8_t BitPosition,  uint8_t Bitdata)
{
	fbio0808_pca95xx.out_data= Bitdata? fbio0808_pca95xx.out_data | (1 << BitPosition) : fbio0808_pca95xx.out_data & ((uint8_t) ~(1 << BitPosition));
	SoftI2c.writes(fbio0808_pca95xx.pins,1,fbio0808_pca95xx.devAddress,PCA95xx_REG_OUTPUT_P1,&fbio0808_pca95xx.out_data,1);	
}
uint8_t FBIO0808GetP1(void)
{
	return fbio0808_pca95xx.out_data;	
}
uint8_t FBIO0808GetP1Bit(uint8_t BitPosition)
{
	return (fbio0808_pca95xx.out_data & (1 << BitPosition))? 1:0;
}





