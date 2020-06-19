
#include "leds.h"	

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
//MBI5024
//
void MBI5024_WriteData(void);

SPI_HandleTypeDef hspi2;

static void MX_SPI2_Init(void);

static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;//SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

//MBI5024

uint8_t MBI5024_CS_Pin=PB_12;

typedef struct
{     
  union
  {
    uint32_t u32_Data;  	
    UCHAR Byte[4];   		//Byte[3]<<24 || Byte[2]<<16 || Byte[1]<<8 || Byte[0]
  } Value;           
} led_rgb_T;        	

led_rgb_T led_rgb_val={0};
	
void MBI5024_Init(void)
{
	MX_SPI2_Init();
	
	MBI5024_WriteData();
}
void MBI5024_WriteData(void)
{
//	pinReset(MBI5024_CS_Pin);
	
	pinSet(MBI5024_CS_Pin);
	HAL_SPI_Transmit(&hspi2,led_rgb_val.Value.Byte,4,1000);
	
//	pinSet(MBI5024_CS_Pin);
//	
//	delay_us(1);
	
	pinReset(MBI5024_CS_Pin);
}

int led(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("led <ch> <r> <g> <b>\n");
			  rt_kprintf("    ch:1~8\n");	
			  rt_kprintf("    r|g|b:1|0\n");
			
        return RT_ERROR;
    }
    if (argc == 5)
    {	

			uint8_t ch=atoi(argv[1]);
			
			uint8_t rgb_r=atoi(argv[2]);
			uint8_t rgb_g=atoi(argv[3]);
			uint8_t rgb_b=atoi(argv[4]);
			
			if(ch>0 && ch<9)
			{
				switch(ch)
				{
					case 1:	
									bitWrite(led_rgb_val.Value.Byte[3],0,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[3],1,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[3],2,rgb_b);
						break;

					case 2:	
									bitWrite(led_rgb_val.Value.Byte[3],3,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[3],4,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[3],5,rgb_b);
						break;	
					
					case 3:	
									bitWrite(led_rgb_val.Value.Byte[2],0,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[2],1,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[2],2,rgb_b);
						break;

					case 4:	
									bitWrite(led_rgb_val.Value.Byte[2],3,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[2],4,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[2],5,rgb_b);
						break;
					case 5:	
									bitWrite(led_rgb_val.Value.Byte[1],0,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[1],1,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[1],2,rgb_b);
						break;

					case 6:	
									bitWrite(led_rgb_val.Value.Byte[1],3,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[1],4,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[1],5,rgb_b);
						break;	
					
					case 7:	
									bitWrite(led_rgb_val.Value.Byte[0],0,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[0],1,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[0],2,rgb_b);
						break;

					case 8:	
									bitWrite(led_rgb_val.Value.Byte[0],3,rgb_r);
									bitWrite(led_rgb_val.Value.Byte[0],4,rgb_g);
									bitWrite(led_rgb_val.Value.Byte[0],5,rgb_b);
						break;					
					default:
						break;
				}
				MBI5024_WriteData();
				
				//rt_kprintf("led_rgb_val=%d\n",led_rgb_val.Value.u32_Data);
				
				rt_kprintf("led[%d]:red=%d,green=%d,blue=%d\n",ch,rgb_r,rgb_g,rgb_b);
			}			
			return RT_EOK;	
    }
		return RT_ERROR;
}
//
MSH_CMD_EXPORT(led, ......);


