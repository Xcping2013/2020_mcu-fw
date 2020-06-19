
#include "inc_controller.h"	

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

PinH2O = 1.25 * (Pcorrected / 223) * 10 inH2O


*/

#define DLLR_ADDR		0x52
#define Start_Single	0xAA
#define Start_Average2	0xAC
#define Start_Average4	0xAD
#define Start_Average8	0xAE
#define Start_Average16	0xAF


void DLLR_L10D_init(void)
{
	pinMode(PB_12,PIN_MODE_OUTPUT);
	pinSet(PB_12);
}

void DLLR_readdata(void)
{
	uint8_t DLLR_inited=0;
	
	int Tdata=0;
	char Tsfdata[10]="";
	uint8_t inbuf[32] = {0}, outbuf[32] = {0};
	if(DLLR_inited==0)
	{
		DLLR_inited=1;
		DLLR_L10D_init();
	}
	outbuf[0]=Start_Single;outbuf[1]=0;outbuf[2]=0;
	
	pinReset(PB_12);
	HAL_SPI_TransmitReceive(&hspi2,outbuf, inbuf,3,1000);
	pinSet(PB_12);
	rt_kprintf("status=0x%x\n",inbuf[0]);

//		delay_us(100);
//	pinReset(PB_12);
//	outbuf[0]=0xF0;outbuf[1]=0;outbuf[2]=0;outbuf[3]=0;outbuf[4]=0;outbuf[5]=0;outbuf[6]=0;
//	HAL_SPI_TransmitReceive(&hspi2,outbuf, inbuf,1,1000);
//	pinSet(PB_12);
//	rt_kprintf("status=0x%x\n",inbuf[0]);
	
	delay_ms(1000);
	pinReset(PB_12);
	outbuf[0]=0xF0;outbuf[1]=0;outbuf[2]=0;outbuf[3]=0;outbuf[4]=0;outbuf[5]=0;outbuf[6]=0;
	HAL_SPI_TransmitReceive(&hspi2,outbuf, inbuf,7,1000);
	pinSet(PB_12);
//	rt_kprintf("status=0x%x,P_adval=%d,T_adval=%d\n",inbuf[0],(int)((inbuf[1]<<16)|(inbuf[2]<<8)|inbuf[3]),(int)((inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6]));
	
	
	
	Tdata=(inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6];
	sprintf(Tsfdata,"%.3f",(float)((double)(Tdata*1.0/(0xffffff+1))*125.0-40));

	//sprintf(Tsfdata,"%.2f",(float)Tdata*125‬-40));
	rt_kprintf("DLLR read data:status=0x%x,P_adval=%d,T_adval=%d,Temperature=%s\n",inbuf[0],(int)((inbuf[1]<<16)|(inbuf[2]<<8)|inbuf[3]),(int)((inbuf[4]<<16)|(inbuf[5]<<8)|inbuf[6]),Tsfdata);

}

MSH_CMD_EXPORT_ALIAS(DLLR_readdata, DLLR,...);

