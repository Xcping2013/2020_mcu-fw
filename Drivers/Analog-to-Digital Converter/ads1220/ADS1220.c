
#include "ADS1220.h"

#include "bsp_mcu_gpio.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif



#if 0	//CUBE SPI Init

#define ADS1220_CS0_PIN PA_4

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;//SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }

} 




void ADS1220_Init(void)
{
	MX_SPI1_Init();
	pinMode(ADS1220_CS0_PIN,PIN_MODE_OUTPUT);
	pinSet(ADS1220_CS0_PIN);
}


void ADS1220_AssertCS( int fAssert)
{
   if (fAssert)	pinReset(ADS1220_CS0_PIN);
   else					pinSet(ADS1220_CS0_PIN);

      
}
unsigned int ADS1220_ReadWriteByte(unsigned char Value)
{
	uint8_t Result;
//	pinReset(ADS1220_CS0_PIN);
  HAL_SPI_TransmitReceive(&hspi1, &Value, &Result, 1,1000);
//	pinSet(ADS1220_CS0_PIN);
	return Result;
}
#endif

/*

*/


// ADS1220 Initial Configuration
void ADS1220_Config0(void)
{
    unsigned Temp;
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
   
  // clear prev value;
    Temp &= 0x00;
    Temp |= (ADS1220_MUX_2_3 | ADS1220_GAIN_128);//ADS1220_GAIN_128);
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_0_REGISTER, 0x01, &Temp);
   
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);   
  // clear prev DataRate code;
    Temp &= 0x00;
    Temp |= (ADS1220_DR_20 | ADS1220_CC);      // Set default start mode to 600sps and continuous conversions
   
  // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    //    ADS1220ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, 0x00000000);
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
  // clear prev DataRate code;
    Temp &= 0x00;
    Temp |= (ADS1220_VREF_EX_DED | ADS1220_REJECT_BOTH);      // Set Internal Vref as 2.048 V
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &Temp);
}
void ADS1220_Config(void)
{
    unsigned Temp;
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
   
  // clear prev value;
    Temp &= 0x00;
    Temp |= (ADS1220_MUX_0_1 | ADS1220_GAIN_2);//ADS1220_GAIN_128);
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_0_REGISTER, 0x01, &Temp);
   
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);   
  // clear prev DataRate code;
    Temp &= 0x00;
    Temp |= (ADS1220_DR_20 | ADS1220_CC );      // Set default start mode to 600sps and continuous conversions
   
  // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    //    ADS1220ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, 0x00000000);
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
  // clear prev DataRate code;
    Temp &= 0x00;
    Temp |= (ADS1220_VREF_INT | ADS1220_IDAC_OFF);//|ADS1220_REJECT_BOTH);      // Set Internal Vref as 2.048 V
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &Temp);
		
		ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
  // clear prev DataRate code;
    Temp &= 0x00;
    Temp |= (ADS1220_IDAC1_OFF | ADS1220_IDAC2_OFF);      // Set Internal Vref as 2.048 V
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_3_REGISTER, 0x01, &Temp);
}

void ADS1220_SendByte(unsigned char Value)
{

	ADS1220_ReadWriteByte(Value);
//	uint8_t Result;
//	pinReset(ADS1220_CS0_PIN);
//  HAL_SPI_TransmitReceive(&hspi1, &Value, &Result, 1,1000);
//	pinSet(ADS1220_CS0_PIN);
}


uint8_t ADS1220_ReceiveByte(void)
{
	return ADS1220_ReadWriteByte(0x00);//(0x00);
}

/*
******************************************************************************
 higher level functions
*/
int ADS1220_ReadData0(void)
{
   uint32_t Data=0;
   
   // assert CS to start transfer
   ADS1220_AssertCS(1);

   // send the command byte
   ADS1220_SendByte(ADS1220_CMD_RDATA);
      
   // get the conversion result
#ifdef ADS1120
   Data = ADS1220_ReceiveByte();
   Data = (Data << 8) | ADS1220_ReceiveByte();
   //Data = (Data << 8) | ADS1220_ReceiveByte();

   // sign extend data
   if (Data & 0x8000)
      Data |= 0xffff0000;
#else
   Data = ADS1220_ReceiveByte();
   Data = (Data << 8) |ADS1220_ReceiveByte();
   Data = (Data << 8) |ADS1220_ReceiveByte();

   // sign extend data
   if (Data & 0x800000)	//Data &= 0x007fffff;
      Data |= 0xff000000;
   
#endif
   // de-assert CS
   ADS1220_AssertCS(0);
   return Data;
}

void ADS1220_ReadRegister(int StartAddress, int NumRegs, unsigned * pData)
{
   int i;

    // assert CS to start transfer
    ADS1220_AssertCS(1);
   
    // send the command byte
    ADS1220_SendByte(ADS1220_CMD_RREG | (((StartAddress<<2) & 0x0c) |((NumRegs-1)&0x03)));
   
    // get the register content
    for (i=0; i< NumRegs; i++)
    {
        *pData++ = ADS1220_ReceiveByte();
    }
   
    // de-assert CS
        ADS1220_AssertCS(0);
    
    return;
}

void ADS1220_WriteRegister(int StartAddress, int NumRegs, unsigned * pData)
{
    int i;
   
    // assert CS to start transfer
    ADS1220_AssertCS(1);
   
    // send the command byte
    ADS1220_SendByte(ADS1220_CMD_WREG | (((StartAddress<<2) & 0x0c) |((NumRegs-1)&0x03)));
   
    // send the data bytes
    for (i=0; i< NumRegs; i++)
    {
        ADS1220_SendByte(*pData++);
    }
   
    // de-assert CS
    ADS1220_AssertCS(0);
   
    return;
}

void ADS1220_SendResetCommand(void)
{
    // assert CS to start transfer
    ADS1220_AssertCS(1);
   
    // send the command byte
    ADS1220_SendByte(ADS1220_CMD_RESET);
   
    // de-assert CS
    ADS1220_AssertCS(0);
   
    return;
}

void ADS1220_SendStartCommand(void)
{
    // assert CS to start transfer
    ADS1220_AssertCS(1);
   
    // send the command byte
    ADS1220_SendByte(ADS1220_CMD_SYNC);
   
    // de-assert CS
    ADS1220_AssertCS(0);
     
    return;
}

void ADS1220_SendShutdownCommand(void)
{
    // assert CS to start transfer
    ADS1220_AssertCS(1);
   
    // send the command byte
    ADS1220_SendByte(ADS1220_CMD_SHUTDOWN);
   
    // de-assert CS
    ADS1220_AssertCS(0);
     
    return;
}

/*
******************************************************************************
register set value commands
*/

int ADS1220_SetChannel(int Mux)
{
    unsigned int cMux = Mux;       
   // write the register value containing the new value back to the ADS
   ADS1220_WriteRegister(ADS1220_0_REGISTER, 0x01, &cMux);
   
   return ADS1220_NO_ERROR;
}

int ADS1220_SetGain(int Gain)
{
    unsigned int cGain = Gain;   
    // write the register value containing the new code back to the ADS
    ADS1220_WriteRegister(ADS1220_0_REGISTER, 0x01, &cGain);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetPGABypass(int Bypass)
{
    unsigned int cBypass = Bypass;
    // write the register value containing the new code back to the ADS
    ADS1220_WriteRegister(ADS1220_0_REGISTER, 0x01, &cBypass);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetDataRate(int DataRate)
{
    unsigned int cDataRate = DataRate;  
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &cDataRate);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetClockMode(int ClockMode)
{
    unsigned int cClockMode = ClockMode;
   
    // write the register value containing the value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &cClockMode);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetPowerDown(int PowerDown)
{
    unsigned int cPowerDown = PowerDown;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &cPowerDown);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetTemperatureMode(int TempMode)
{
    unsigned int cTempMode = TempMode;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &cTempMode);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetBurnOutSource(int BurnOut)
{
    unsigned int cBurnOut = BurnOut;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_1_REGISTER, 0x01, &cBurnOut);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetVoltageReference(int VoltageRef)
{
    unsigned int cVoltageRef = VoltageRef;
   
    // write the register value containing the new value back to the ADS
     ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &cVoltageRef);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_Set50_60Rejection(int Rejection)
{
    unsigned int cRejection = Rejection;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &cRejection);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetLowSidePowerSwitch(int PowerSwitch)
{
    unsigned int cPowerSwitch = PowerSwitch;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &cPowerSwitch);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetCurrentDACOutput(int CurrentOutput)
{
    unsigned int cCurrentOutput = CurrentOutput;
   
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_2_REGISTER, 0x01, &cCurrentOutput);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetIDACRouting(int IDACRoute)
{
    unsigned int cIDACRoute = IDACRoute;
    
    // write the register value containing the new value back to the ADS
    ADS1220_WriteRegister(ADS1220_3_REGISTER, 0x01, &cIDACRoute);
    
    return ADS1220_NO_ERROR;
}

int ADS1220_SetDRDYMode(int DRDYMode)
{
    unsigned int cDRDYMode = DRDYMode;
   
    // write the register value containing the new gain code back to the ADS
    ADS1220_WriteRegister(ADS1220_3_REGISTER, 0x01, &cDRDYMode);
    
    return ADS1220_NO_ERROR;
}

/*
******************************************************************************
register get value commands
*/

int ADS1220_GetChannel(void)
{
    unsigned Temp;
    
    //Parse Mux data from register
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return (Temp >>4);
}

int ADS1220_GetGain(void)
{
    unsigned Temp;
    
    //Parse Gain data from register
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x0e) >>1);
}

int ADS1220_GetPGABypass(void)
{
    unsigned Temp;
    
    //Parse Bypass data from register
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return (Temp & 0x01);
}

int ADS1220_GetDataRate(void)
{
    unsigned Temp;
    
    //Parse DataRate data from register
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( Temp >>5 );
}

int ADS1220_GetClockMode(void)
{
    unsigned Temp;
    
    //Parse ClockMode data from register
      ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x18) >>3 );
}

int ADS1220_GetPowerDown(void)
{
    unsigned Temp;
    
    //Parse PowerDown data from register
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x04) >>2 );
}

int ADS1220_GetTemperatureMode(void)
{
    unsigned Temp;
    
    //Parse TempMode data from register
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x02) >>1 );
}

int ADS1220_GetBurnOutSource(void)
{
    unsigned Temp;
    
    //Parse BurnOut data from register
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( Temp & 0x01 );
}

int ADS1220_GetVoltageReference(void)
{
    unsigned Temp;
    
    //Parse VoltageRef data from register
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( Temp >>6 );
}

int ADS1220_Get50_60Rejection(void)
{
    unsigned Temp;
    
    //Parse Rejection data from register
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x30) >>4 );
}

int ADS1220_GetLowSidePowerSwitch(void)
{
    unsigned Temp;
    
    //Parse PowerSwitch data from register
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x08) >>3);
}

int ADS1220_GetCurrentDACOutput(void)
{
    unsigned Temp;
    
    //Parse IDACOutput data from register
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( Temp & 0x07 );
}

int ADS1220_GetIDACRouting(int WhichOne)
{
    // Check WhichOne sizing
    if (WhichOne >1) return ADS1220_ERROR;
    
    unsigned Temp;
    
    //Parse Mux data from register
    ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    if (WhichOne) return ( (Temp & 0x1c) >>2);
    
    else return ( Temp >>5 );
    
}

int ADS1220_GetDRDYMode(void)
{
    unsigned Temp;
    
    //Parse DRDYMode data from register
    ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
    
    // return the parsed data
    return ( (Temp & 0x02) >>1 );
}



/* Useful Functions within Main Program for Setting Register Contents
*
*   These functions show the programming flow based on the header definitions.
*   The calls are not made within the demo example, but could easily be used by calling the function
*       defined within the program to complete a fully useful program.
*   Similar function calls were made in the firwmare design for the ADS1220EVM.
*  
*  The following function calls use ASCII data sent from a COM port to control settings 
*   on the   The data is recontructed from ASCII and then combined with the
*   register contents to save as new configuration settings.
*
*   Function names correspond to datasheet register definitions
*/
void ADS1220_set_MUX(int c)
{       
          int mux = c - 48;
        int dERROR;
        unsigned Temp;      
    
        
    if (mux>=49 && mux<=54) mux -= 39;
    
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);

    Temp &= 0x0f;                                   // strip out old settings
    // Change Data rate
    switch(mux) {
        case 0:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_0_1);
            break;
        case 1:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_0_2);
            break;
        case 2:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_0_3);
            break;
        case 3:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_1_2);
            break;
        case 4:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_1_3);
            break;
        case 5:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_2_3);
            break;
        case 6:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_1_0);
            break;
        case 7:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_3_2);
            break;
        case 8:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_0_G);
            break;
        case 9:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_1_G);
            break;
        case 10:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_2_G);
            break;
        case 11:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_3_G);
            break;
        case 12:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_EX_VREF);
            break;
        case 13:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_AVDD);
            break;
        case 14:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_DIV2);
            break;
        case 15:
            dERROR = ADS1220_SetChannel(Temp + ADS1220_MUX_DIV2);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;                                              
    }
    
    if (dERROR==ADS1220_ERROR)
        ADS1220_set_ERROR();
    
}

void ADS1220_set_GAIN(char c)
{
    int pga = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
    
    Temp &= 0xf1;                                   // strip out old settings
    // Change gain rate
    switch(pga) {
        case 0:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_1);
            break;
        case 1:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_2);
            break;
        case 2:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_4);
            break;
        case 3:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_8);
            break;
        case 4:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_16);
            break;
        case 5:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_32);
            break;
        case 6:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_64);
            break;
        case 7:
            dERROR = ADS1220_SetGain(Temp + ADS1220_GAIN_128);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;  
        }
                                            
    
    
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
}

void ADS1220_set_PGA_BYPASS(char c)
{
    int buff = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
        
    Temp &= 0xfe;                                   // strip out old settings
    // Change PGA Bypass
    switch(buff) {
        case 0:
            dERROR = ADS1220_SetPGABypass(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetPGABypass(Temp + ADS1220_PGA_BYPASS);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_DR(char c)
{
    int spd = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    Temp &= 0x1f;                                   // strip out old settings
    // Change Data rate
    switch(spd) {
        case 0:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_20);
            break;
        case 1:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_45);
            break;
        case 2:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_90);
            break;
        case 3:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_175);
            break;
        case 4:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_330);
            break;
        case 5:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_600);
            break;
        case 6:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_1000);
            break;
        case 7:
            dERROR = ADS1220_SetDataRate(Temp + ADS1220_DR_1000);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
            
}

void ADS1220_set_MODE(char c)
{
    int spd = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    Temp &= 0xe7;                                   // strip out old settings
    // Change Data rate
    switch(spd) {
        case 0:
            dERROR = ADS1220_SetClockMode(Temp + ADS1220_MODE_NORMAL);
            break;
        case 1:
            dERROR = ADS1220_SetClockMode(Temp + ADS1220_MODE_DUTY);
            break;
        case 2:
            dERROR = ADS1220_SetClockMode(Temp + ADS1220_MODE_TURBO);
            break;
        case 3:
            dERROR = ADS1220_SetClockMode(Temp + ADS1220_MODE_DCT);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
        }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();

}

void ADS1220_set_CM(char c)
{
    int pwrdn = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    Temp &= 0xfb;                                   // strip out old settings
    // Change power down mode
    switch(pwrdn) {
        case 0:
            dERROR = ADS1220_SetPowerDown(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetPowerDown(Temp + ADS1220_CC);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_TS(char c)
{
    int tmp = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    Temp &= 0xfd;                                   // strip out old settings
    // Change Temp Diode Setting
    switch(tmp) {
        case 0:
            dERROR = ADS1220_SetTemperatureMode(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetTemperatureMode(Temp + ADS1220_TEMP_SENSOR);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_BCS(char c)
{
    int bo = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    Temp &= 0xfe;                                   // strip out old settings
    // Change PGA Bypass
    switch(bo) {
        case 0:
            dERROR = ADS1220_SetBurnOutSource(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetBurnOutSource(Temp + ADS1220_BCS);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
    
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
        
}

void ADS1220_set_VREF(char c)
{
    int ref = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    Temp &= 0x3f;                                   // strip out old settings
    // Change Reference
    switch(ref) {
        case 0:
            dERROR = ADS1220_SetVoltageReference(Temp + ADS1220_VREF_INT);
            break;
        case 1:
            dERROR = ADS1220_SetVoltageReference(Temp + ADS1220_VREF_EX_DED);
            break;
        case 2:
            dERROR = ADS1220_SetVoltageReference(Temp + ADS1220_VREF_EX_AIN);
            break;
        case 3:
            dERROR = ADS1220_SetVoltageReference(Temp + ADS1220_VREF_SUPPLY);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
        
}
void ADS1220_set_50_60(char c)
{
    int flt = (int) c - 48;
    int dERROR;
    unsigned Temp;
    
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    Temp &= 0xcf;                                   // strip out old settings
    // Change Filter
    switch(flt) {
        case 0:
            dERROR = ADS1220_Set50_60Rejection(Temp + ADS1220_REJECT_OFF);
            break;
        case 1:
            dERROR = ADS1220_Set50_60Rejection(Temp + ADS1220_REJECT_BOTH);
            break;
        case 2:
            dERROR = ADS1220_Set50_60Rejection(Temp + ADS1220_REJECT_50);
            break;
        case 3:
            dERROR = ADS1220_Set50_60Rejection(Temp + ADS1220_REJECT_60);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
        
}

void ADS1220_set_PSW(char c)
{
    int sw = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    Temp &= 0xf7;                                   // strip out old settings
        // Change power down mode
    switch(sw) {
        case 0:
            dERROR = ADS1220_SetLowSidePowerSwitch(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetLowSidePowerSwitch(Temp + ADS1220_PSW_SW);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_IDAC(char c)
{
    int current = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    Temp &= 0xf8;                                   // strip out old settings
    // Change IDAC Current Output
    switch(current) {
        case 0:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_OFF);
            break;
        case 1:
          #ifdef ADS1120
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_OFF);
          #else
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_10);
          #endif

            break;
        case 2:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_50);
            break;
        case 3:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_100);
            break;
        case 4:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_250);
            break;
        case 5:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_500);
            break;
        case 6:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_1000);
            break;
        case 7:
            dERROR = ADS1220_SetCurrentDACOutput(Temp + ADS1220_IDAC_1500);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
        
        }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_IMUX(char c, int i)
{
    int mux = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
    
    if (i==1) {
        Temp &= 0xe3;                                   // strip out old settings
        
        // Change IDAC2 MUX Output
    
        switch(mux) {
            case 0:
                Temp |= ADS1220_IDAC2_OFF;
                break;
            case 1:
                Temp |= ADS1220_IDAC2_AIN0;
                break;
            case 2:
                Temp |= ADS1220_IDAC2_AIN1;
                break;
            case 3:
                Temp |= ADS1220_IDAC2_AIN2;
                break;
            case 4:
                Temp |= ADS1220_IDAC2_AIN3;
                break;
            case 5:
                Temp |= ADS1220_IDAC2_REFP0;
                break;
            case 6:
                Temp |= ADS1220_IDAC2_REFN0;
                break;
            case 7:
                Temp |= ADS1220_IDAC2_REFN0;
                break;
            default:
                dERROR = ADS1220_ERROR;
                break;
        
        }
    }
    else {
        Temp &= 0x1f;
        // Change IDAC1 MUX Output
        switch(mux) {
            case 0:
                Temp |= ADS1220_IDAC1_OFF;
                break;
            case 1:
                Temp |= ADS1220_IDAC1_AIN0;
                break;
            case 2:
                Temp |= ADS1220_IDAC1_AIN1;
                break;
            case 3:
                Temp |= ADS1220_IDAC1_AIN2;
                break;
            case 4:
                Temp |= ADS1220_IDAC1_AIN3;
                break;
            case 5:
                Temp |= ADS1220_IDAC1_REFP0;
                break;
            case 6:
                Temp |= ADS1220_IDAC1_REFN0;
                break;
            case 7:
                Temp |= ADS1220_IDAC1_REFN0;
                break;
            default:
                dERROR = ADS1220_ERROR;
                break;
        }
    }
    
    if (dERROR==ADS1220_NO_ERROR) 
        dERROR = ADS1220_SetIDACRouting(Temp); 
    
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}

void ADS1220_set_DRDYM(char c)
{
    int drdy = (int) c - 48;
    int dERROR;
    unsigned Temp;
        
    // the DataRate value is only part of the register, so we have to read it back
    // and massage the new value into it
    ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
    
    Temp &= 0xfd;                                   // strip out old settings
    // Change DRDY Mode Setting
    switch(drdy) {
        case 0:
            dERROR = ADS1220_SetDRDYMode(Temp);
            break;
        case 1:
            dERROR = ADS1220_SetDRDYMode(Temp + ADS1220_DRDY_MODE);
            break;
        default:
            dERROR = ADS1220_ERROR;
            break;
    
    }
                                
    if (dERROR==ADS1220_ERROR) 
        ADS1220_set_ERROR();
    
}




void ADS1220_set_ERROR_Transmit(void)
{
    /* De-initialize the SPI comunication BUS */
}

void ADS1220_set_ERROR_Receive(void)
{
    /* De-initialize the SPI comunication BUS */
  DBG_TRACE("\rSPI Failed during Reception\n\r");
}

void ADS1220_set_ERROR(void)
{
    /* De-initialize the SPI comunication BUS */
	DBG_TRACE("\rADS1220 Error\n\r");
}

#if DBG_ENABLE
int ads1220(int argc, char **argv)
{
	uint8_t result = REPLY_OK;
	if (argc == 2 )
	{
		if (!strcmp(argv[1], "readRegs"))
		{
			unsigned Temp;
	
			ADS1220_ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
			DBG_TRACE("ads1220_reg[0]=0x%x\n",Temp);
			ADS1220_ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
			DBG_TRACE("ads1220_reg[1]=0x%x\n",Temp);
			ADS1220_ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
			DBG_TRACE("ads1220_reg[2]=0x%x\n",Temp);
			ADS1220_ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
			DBG_TRACE("ads1220_reg[3]=0x%x\n",Temp);
			
			return RT_EOK	; 
					
		}
		else	if (!strcmp(argv[1], "readData"))
		{
//			ADS1220_SendStartCommand();
//			while
			DBG_TRACE("ads1220_data=0x%x\n",ADS1220_ReadData());
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "readGain"))
		{
//			ADS1220_SendStartCommand();
//			while
			DBG_TRACE("ads1220_Gain=%d\n",ADS1220_GetGain());
			return RT_EOK	; 
		}
		
	}
	return result;
}


MSH_CMD_EXPORT(ads1220, ...);



#endif





