#include "tmc429.h"

#include "inc_projects.h"

#if 1   //DBG_ENABLE
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
	#endif
#endif

#if 1		//DEVXPBY_PIN_N		PN_n
	#define DEV3PB2_PIN 		PE_2
	#define DEV3IRQ_PIN 		PE_3
	#define DEV3PB1_PIN 		PE_4
	#define DEV3PB0_PIN 		PE_5
	#define DEV3PC1_PIN 		PE_6
	#define DEV3PC0_PIN 		PC_13
	#define EROM_SCL_PIN 		PC_14
	#define EROM_SDA_PIN 		PC_15
	#define DEV5PC0_PIN 		PC_0
	#define DEV5PC1_PIN 		PC_1
	#define DEV5PB1_PIN 		PC_2
	#define DEV5PB0_PIN 		GPC_3
	#define DEV5IRQ_PIN 		PA_0

	#define DEV5PB3_PIN 		PA_1
	#define DEV5TX_PIN 			PA_2
	#define DEV5RX_PIN 			PA_3
	#define FROMCS_PIN 			PA_4
	#define DEVXSCK_PIN 		PA_5
	#define DEVXMISO_PIN 		PA_6
	#define DEVXMOSI_PIN 		PA_7
	#define DEV5PB2_PIN 		PC_4
	#define DEV5PC3_PIN 		PC_5
	#define DEV5PC2_PIN 		PB_0
	#define INTPUT_IRQ_PIN 	PB_1
	#define LED_BOOT1_PIN 	PB_2
	#define DEV4PC0_PIN 		PE_7
	#define DEV4PC1_PIN 		PE_8
	#define DEV4PB0_PIN 		PE_9
	#define DEV4PB1_PIN 		PE_10
	#define DEV4IRQ_PIN 		PE_11
	#define DEV4PB2_PIN 		PE_12
	#define DEV4PB3_PIN 		PE_13
	#define DEV4PC2_PIN 		PE_14
	#define DEV4PC3_PIN 		PE_15

	#define DEV2PC0_PIN 		PB_10
	#define DEV2PC1_PIN 		PB_11
	#define DEV1CS_PIN 			PB_12
	#define DEV1SCK_PIN 		PB_13
	#define DEV1MISO_PIN 		PB_14
	#define DEV1MOSI_PIN 		PB_15
	#define DEV4TX_PIN 			PD_8
	#define DEV4RX_PIN 			PD_9
	#define DEV1PC3_PIN 		PD_10
	#define DEV1PC2_PIN 		PD_11
	#define DEV1PA2_PIN 		PD_12
	#define DEV1PA1_PIN 		PD_13
	#define DEV1PA0_PIN 		PD_14
	#define DEV1PC1_PIN 		PD_15
	#define DEV1PC0_PIN 		PC_6
	#define DEV2PC3_PIN 		PC_7
	#define DEV2PC2_PIN 		PC_8
	#define DEV2PB3_PIN 		PC_9
	#define DEV2PB2_PIN 		PA_8
	#define MCU_TX_PIN 			PA_9
	#define MCU_RX_PIN 			PA_10
	#define USB_DM_PIN 			PA_11
	#define USB_DP_PIN 			PA_12

	#define SWDIO_LANIRQ_PIN 	PA_13
	#define SWCLK_LANCS_PIN 	PA_14

	#define DEV2IRQ_PIN 		PA_15
	#define DEV2TX_PIN 			PC_10
	#define DEV2RX_PIN			PC_11
	#define DEV3TX_PIN 			PC_12
	#define DEV2PB1_PIN 		PD_0
	#define DEV2PB0_PIN 		PD_1
	#define DEV3RX_PIN 			PD_2
	#define DEV0PC1_PIN 		PD_3
	#define DEV0PC0_PIN 		PD_4
	#define DEV0IRQ_PIN 		PD_5
	#define DEV0PA1_PIN 		PD_6
	#define DEV0PA2_PIN 		PD_7

	#define DEV0SCK_PIN 		PB_3
	#define DEV0MISO_PIN 		PB_4
	#define DEV0MOSI_PIN 		PB_5
	#define DEV0PC3_PIN 		PB_6
	#define DEV0CS_PIN 			PB_7
	#define DEV0PC2_PIN 		PB_8
	#define DEV3PC3_PIN 		PB_9
	#define DEV3PC2_PIN 		PE_0
	#define DEV3PB3_PIN 		PE_1
#endif


const float Vmax_factor = 0.953674;
const float Amax_factor_slow = 0.953674;  //(600/(80*80)); 不能使用公式，必须常量
const float Amax_factor_fast = 0.0029478; //(130/(210*210));

uint8_t OriginSensorPin[6]=
{	
	DEV0PA2_PIN, DEV0PC3_PIN, DEV0PC2_PIN, 
	DEV1PA2_PIN,	DEV1PC3_PIN,	DEV1PC2_PIN ,

};

uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};

void tmc429_clk_init(void)
{
	//有源晶振 CLK=16MHZ 
}

void TMC429_DefaulSetting(void)	
{
  uint8_t i;
		
	for(i=0; i<N_O_MOTORS; i++) 					
	{		
		pinMode(OriginSensorPin[i],PIN_MODE_INPUT_PULLUP);
		
		motorSetting.saved=DATA_SAVED;
		
		motorSetting.limit_level_valid=1;          
		motorSetting.orgin_level_valid=0;        
		
		motorSetting.SpeedChangedFlag[i]= TRUE;		
		
		motorSetting.VMax[i]=840;
		motorSetting.AMax[i]=1000;
		
		motorSetting.PulseDiv[i]=8;
		motorSetting.RampDiv[i]=7;
		
		motorHoming.Homed[i]				=	FALSE;
		motorHoming.GoHome[i]				=	FALSE;
		motorHoming.GoLimit[i]			=	FALSE;
		motorHoming.HomeSpeed[i]		=	2000;
	}
	if(WhichProject==LID_OPEN)
	{
		motorSetting.limit_level_valid=1;          
		motorSetting.orgin_level_valid=0;  
		OriginSensorON[0]=LOW;	OriginSensorON[1]=LOW;	OriginSensorON[2]=LOW;
		OriginSensorON[3]=LOW;	OriginSensorON[4]=LOW;	OriginSensorON[5]=LOW;
	}
	else if(WhichProject==BUTTON_TEST)
	{
		motorSetting.limit_level_valid=1;          
		motorSetting.orgin_level_valid=0;  
		OriginSensorON[0]=LOW;	OriginSensorON[1]=LOW;	OriginSensorON[2]=LOW;
		OriginSensorON[3]=LOW;	OriginSensorON[4]=LOW;	OriginSensorON[5]=LOW;			
	}	
}
//








