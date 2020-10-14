
#include "mpm.h"	
#include "math.h"	


//0x40 准确

/*	pressure(inH2O)=1.25x( (Pout_dig-OSdig)/(2^24))xFSS(inH2O)

pressure(inH2O)=1.25x( (Pout_dig-0.1 * 2^24)/(2^24))xFSS(inH2O)

inch of water(inH2O) inH2O=249.082Pa=0.249082kP		±10 inH2O =2488.4Pa

1KPa=1000KPa=1000*0.004 inH2O  	

*/

/*	DLLR-L10D-E2BS-C-NAV6

±10 inH2O	;	Dual Port Opposite Side;	Barbed	;	SIP;
Commercial;	No Coating	;	Auto I2C, address 0x29/SPI;
1.68V to 3.6V;	16Bit
*/
/*
data:status=0x44,P_adval=8381696,T_adval=9032640,Temperature=27.298
data:status=0x45,P_adval=0,T_adval=9033472,Temperature=27.305
    
DLLR-L10D-E1BD-C-NAV8

PRESSURE RANGE:	±10 inH2O		
Dual Port Same Side
Barbed	
DIP
Commercial
No Coating
Auto I2C, address 0x29/SPI
1.68V to 3.6V
18 bit

*/
SPI_HandleTypeDef hspi3;
/*
// I2C Input, output buﬀers:
unsigned char inbuf[32] = {0}, outbuf[32] = {0};
// ----- DLLR Coefcients ------
ﬂoat DLLR_A = 0.0, DLLR_B = 0.0, DLLR_C = 0.0, DLLR_D = 0.0;
ﬂoat DLLR_E = 0.0, TC50H = 0.0, TCH50L = 0.0;
int32_t i32A = 0, i32B =0, i32C =0, i32D=0,
int8_t i8E = 0, i8TC50H = 0, i8TC50L = 0;

outbuf[0] = 47; // Address of A high word
success = DUT_I2C_Write(ui8Address, outbuf, 1); // 1-byte request
Wait_ms(20); // EEPROM access time : returns [Status][MSB][LSB]
success = DUT_I2C_Read(ui8Address, inbuf, 3); // EEPROM result
i32A = (inbuf[1] << 24) | (inbuf[2] <<16); // Assemble MSBs
outbuf[0] = 48; // Address of A low word
success = DUT_I2C_Write(ui8Address, outbuf, 1); // 1-byte request
Wait_ms(20); // EEPROM access time
success = DUT_I2C_Read(ui8Address, inbuf, 3); // EEPROM result
i32A |= ((inbuf[1] << 8) | (inbuf[2])); // assemble LSBs, for int32
DLLR_A = ((ﬂoat)(i32A))/((ﬂoat)(0x7FFFFFFF)); // convert to ﬂoat
outbuf[0] = 49;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32B = (inbuf[1] << 24) | (inbuf[2] <<16);
outbuf[0] = 50;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32B |= ((inbuf[1] << 8) | (inbuf[2]));
DLLR_B = (ﬂoat)(i32B)/(ﬂoat)(0x7FFFFFFF);


outbuf[0] = 51;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32C = (inbuf[1] << 24) | (inbuf[2] <<16);
outbuf[0] = 52;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32C |= ((inbuf[1] << 8) | (inbuf[2]));
DLLR_C = (ﬂoat)(i32C)/(ﬂoat)(0x7FFFFFFF);
outbuf[0] = 53;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32D = (inbuf[1] << 24) | (inbuf[2] <<16);
outbuf[0] = 54;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i32D |= ((inbuf[1] << 8) | (inbuf[2]));
DLLR_D = (ﬂoat)(i32D)/(ﬂoat)(0x7FFFFFFF);
outbuf[0] = 55;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i16E = ((inbuf[1] << 8) | (inbuf[2]));
i8TC50H = inbuf [1]; i8TC50L = inbuf [2];
TC50H = (ﬂoat)(i8TC50H)/(ﬂoat)(0x7F);
TC50L = (ﬂoat)(i8TC50L)/(ﬂoat)(0x7F);
outbuf[0] = 56;
success = DUT_I2C_Write(ui8Address, outbuf, 1);
Wait_ms(20);
success = DUT_I2C_Read(ui8Address, inbuf, 3);
i8E = inbuf [1];
DLLR_E = (ﬂoat)(i8E)/(ﬂoat)/(0x7F);

outbuf[0] = 0xAD; // Avg4 request = 0xAD
rc = DUT_I2C_Write(ui8Address, outbuf, 1) // send 1-byte request

rc = DUT_I2C_Read(ui8Address, inbuf, 7); // read 7 bytes: Status, P, T
ﬂoat AP3, BP2, CP, LCorr, PCorr, Padj, TCadj, TC50;
int32_t iPraw, Tdiﬀ, Tref, iTemp, iPCorrected;
uint32_t uiPCorrected;
//DLLR: Modify sensor P value:
iPraw = (inbuf[1]<<16) + (inbuf[2]<<8) + inbuf[3] - 0x800000;
iTemp = (inbuf[4]<<16) + (inbuf[5]<<8) + inbuf[6];
Pnorm = (ﬂoat)iPraw;
Pnorm /= (ﬂoat) 0x7FFFFF;
AP3 = DLLR_A * Pnorm * Pnorm * Pnorm; // A*POut2
BP2 = DLLR_B * Pnorm * Pnorm; // B*POut2
CP = DLLR_C * Pnorm; // C*POut
LCorr = AP3 + BP2 + CP + DLLR_D; // Linearity correction term
// Compute Temperature - Dependent Adjustment:
Tref = (int32_t)((2^24)*65/125); // Reference Temperature, in sensor counts
Tdiﬀ = iTemp - Tref;
//TC50: Select High/Low, based on sensor temperature reading:
if (iTemp > Tref)
TC50 = TC50H;
else
TC50 = TC50L;
if (Pnorm > 0.5)
Padj = Pnorm - 0.5;
else
Padj = 0.5 - Pnorm;
TCadj = (1.0 - (DLLR_E * 1.25 * Padj)) * Tdiﬀ * TC50;
PCorr = Pnorm + LCorr + TCadj; // corrected P: ﬂoat, ±1.0
iPCorrected = (int32_t)(PCorr*(ﬂoat)0x7FFFFF); // corrected P: signed int32
//corrected P: 24-bit unsigned value same unsigned format as sensor output
uiPCorrected = (uint32_t) (iPCorrected + 0x800000);

outbuf[0] = 0xAD; // Avg4 request = 0xAD
rc = DUT_I2C_Write(ui8Address, outbuf, 1) // send 1-byte request

PinH2O = 1.25 * (Pcorrected / 2^23) * 10 inH2O


*/

#define DLLR_ADDR		0x52
#define Start_Single	0xAA
#define Start_Average2	0xAC
#define Start_Average4	0xAD
#define Start_Average8	0xAE
#define Start_Average16	0xAF

#define SENSOR_SPI_MISO          GET_PIN(B, 4)    
#define SENSOR_SPI_MOSI          GET_PIN(B, 5)    
#define SENSOR_SPI_SCLK          GET_PIN(B, 3)    

#define SENSOR_PRESSURE_CS       GET_PIN(A, 15)    
#define SENSOR_PRESSURE_EOC      GET_PIN(D, 4)    

static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */
	

	//1.SPI_POLARITY_HIGH	SPI_PHASE_1EDGE PASS	0x44
	
	//0.SPI_POLARITY_LOW	SPI_PHASE_1EDGE PASS	0x44
	//1.SPI_POLARITY_LOW	SPI_PHASE_2EDGE PASS	0x44
	//2.SPI_POLARITY_HIGH	SPI_PHASE_1EDGE PASS	0x44
	//3.SPI_POLARITY_HIGH	SPI_PHASE_2EDGE FAIL	0x88
	
  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}
//5 MHz
void DLLR_L10D_init(void)
{
	pinMode(SENSOR_PRESSURE_CS,PIN_MODE_OUTPUT);
	
	pinMode(SENSOR_PRESSURE_EOC,PIN_MODE_INPUT);
	
	pinSet(PA_15);
	
	MX_SPI3_Init();
	
	//dllr_thread_init();
}

void DLLR_readdata(void)
{
	static uint8_t DLLR_inited=0;
	
	int Tdata=0;
	char Tsfdata[20]="";
	uint8_t inbuf[32] = {0}, outbuf[32] = {0};
	if(DLLR_inited==0)
	{
		DLLR_inited=1;
		DLLR_L10D_init();
	}
	outbuf[0]=Start_Single;outbuf[1]=0;outbuf[2]=0;
	
	pinReset(PA_15);
	
	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,3,1000);	//send Measurement Command
	
	pinSet(PA_15);
	
	for(uint8_t i=0;i<5;i++)	
	{
		//rt_kprintf("EOCpin=%d\n",pinRead(SENSOR_PRESSURE_EOC));
		rt_thread_mdelay(50);
	}
	outbuf[0]=0xF0;outbuf[1]=0;outbuf[2]=0;outbuf[3]=0;outbuf[4]=0;outbuf[5]=0;outbuf[6]=0;
	
	pinReset(PA_15);
	
	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,7,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
	
	pinSet(PA_15);
	
//	rt_kprintf("status=0x%x,P_adval=%d,T_adval=%d\n",inbuf[0],(int)((inbuf[1]<<16)|(inbuf[2]<<8)|inbuf[3]),(int)((inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6]));

	for(uint8_t i=0;i<5;i++)	
	{
		//rt_kprintf("EOCpin=%d\n",pinRead(SENSOR_PRESSURE_EOC));
		rt_thread_mdelay(50);
	}	
	
	Tdata=(inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6];
	sprintf(Tsfdata,"%.3f",(float)((double)(Tdata*1.0/(0xffffff+1))*125.0-40));

//	sprintf(Tsfdata,"%.3f",(float)(	(double)((Tdata/pow(2,24))*125)-40)	);	//1<<24
	//sprintf(Tsfdata,"%.2f",(float)Tdata*125‬-40));
	rt_kprintf("DLLR read data:status=0x%x,P_adval=%d,T_adval=%d,Temperature=%s\n",inbuf[0],(int)((inbuf[1]<<16)|(inbuf[2]<<8)|inbuf[3]),(int)((inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6]),Tsfdata);

	Tdata=0;
	Tdata=(inbuf[1]<<16)|(inbuf[2]<<8)|inbuf[3] & 0x0FFF;
	
	sprintf(	Tsfdata,"%.3f",(	(	(Tdata*1.0/(0xffffff+1))-0.1	)*2.4884*1.25	)	);

//	sprintf(	Tsfdata,"%.3f",(double)(	((Tdata/(0xffffff+1)-0.1	)*2488.4*1.25	)	));
	
	rt_kprintf("pressure=%s KPa\n",Tsfdata);
/*
////	rt_kprintf("pressure=%d Pa\n",(Tdata/(2^24)-0.1	)*2488.4*1.25	);
//	ﬂoat DLLR_A = 0.0, DLLR_B = 0.0, DLLR_C = 0.0, DLLR_D = 0.0;
//	ﬂoat DLLR_E = 0.0, TC50H = 0.0, TCH50L = 0.0;
//	int32_t i32A = 0, i32B =0, i32C =0, i32D=0;
//	int8_t i8E = 0, i8TC50H = 0, i8TC50L = 0;
//	
//	outbuf[0] = 47;
//	pinReset(PA_15);
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,1,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	
//	i32A = (inbuf[1] << 24) | (inbuf[2] <<16);
//	outbuf[0] = 48;
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,1,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	rt_thread_mdelay(20);
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,3,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	i32A |= ((inbuf[1] << 8) | (inbuf[2]));
//	DLLR_A = ((ﬂoat)(i32A))/((ﬂoat)(0x7FFFFFFF));
//	outbuf[0] = 49;
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,1,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	rt_thread_mdelay(20);
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,3,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	i32B = (inbuf[1] << 24) | (inbuf[2] <<16);
//	outbuf[0] = 50;
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,1,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	rt_thread_mdelay(20);
//	HAL_SPI_TransmitReceive(&hspi3,outbuf, inbuf,3,1000);	//Read Sensor Data	Host must send [0xF0], then 6 bytes of [0x00] on MOSI
//	i32B |= ((inbuf[1] << 8) | (inbuf[2]));
//	DLLR_B = (ﬂoat)(i32B)/(ﬂoat)(0x7FFFFFFF);
//	
//	float AP3, BP2, CP, LCorr, PCorr, Padj, TCadj, TC50;
//	int32_t iPraw, Tdiﬀ, Tref, iTemp, iPCorrected;
//	uint32_t uiPCorrected;
//	iPraw = (inbuf[1]<<16) + (inbuf[2]<<8) + inbuf[3] - 0x800000;
//	iTemp = (inbuf[4]<<16) + (inbuf[5]<<8) + inbuf[6];
//	Pnorm = (ﬂoat)iPraw;
//	Pnorm /= (ﬂoat) 0x7FFFFF;
//	AP3 = DLLR_A * Pnorm * Pnorm * Pnorm;
//	BP2 = DLLR_B * Pnorm * Pnorm;
//	CP = DLLR_C * Pnorm;
//	LCorr = AP3 + BP2 + CP + DLLR_D;
//	
//	Tref = (int32_t)((2^24)*65/125);
//	Tdiﬀ = iTemp - Tref;
//	if (iTemp > Tref)
//	TC50 = TC50H;
//	else
//	TC50 = TC50L;
//	if (Pnorm > 0.5)
//	Padj = Pnorm - 0.5;
//	else
//	Padj = 0.5 - Pnorm;
//	TCadj = (1.0 - (DLLR_E * 1.25 * Padj)) * Tdiﬀ * TC50;
//	PCorr = Pnorm + LCorr + TCadj;
//	iPCorrected = (int32_t)(PCorr*(ﬂoat)0x7FFFFF)
//	uiPCorrected = (uint32_t) (iPCorrected + 0x800000);
//	
//	sprintf(	Tsfdata,"%.3f",(float)((	(uiPCorrected/pow(2,23)))*2.4884*12.5	)	);

////	sprintf(	Tsfdata,"%.3f",(double)(	((Tdata/(0xffffff+1)-0.1	)*2488.4*1.25	)	));
//	
//	rt_kprintf("pressure=%s KPa\n",Tsfdata);
*/
}

static rt_thread_t tid2 = RT_NULL;

static void dllr_thread_entry(void *parameter)
{
	uint8_t EOCin=0;
	
	while (1)
	{	
		EOCin=pinRead(SENSOR_PRESSURE_EOC);
		rt_thread_mdelay(20);
		if(EOCin!=pinRead(SENSOR_PRESSURE_EOC))
		{
			rt_kprintf("EOCpin!=%d\n",EOCin);
		}
	}
}

int dllr_thread_init(void)
{
	tid2 = rt_thread_create("dllr",
													dllr_thread_entry,
													RT_NULL,
													1024,
													20,
													20);

	if (tid2 != RT_NULL)
	{
			rt_thread_startup(tid2);
	}
  return 0;
}

MSH_CMD_EXPORT_ALIAS(DLLR_readdata, DLLR,...);

