/** Based on InvenSense TCA6408A datasheet
* 03/05/2014 by Conor Forde <me@conorforde.com>
* Updates should be available at https://github.com/Snowda/TCA6408A
*
* Changelog:
*     ... - ongoing development release

* NOTE: THIS IS ONLY A PARIAL RELEASE. 
* THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE DEVELOPMENT AND IS MISSING MOST FEATURES. 
* PLEASE KEEP THIS IN MIND IF YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.
*/


/*
Hardware Address Pin Allows Two TCA6408A
Internal Power-On Reset
Power Up With All Channels Configured as Inputs
*/

#include "TCA6408A.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

#define TCA6408A_DEFAULT_ADDRESS        0x40
#define TCA6408A_ALT_DEFAULT_ADDRESS    0x42

pca6408a_t E75_pca6408a=
{
	{PD_4,PD_3},
	TCA6408A_DEFAULT_ADDRESS,
	
	//TCA6408A_ALT_DEFAULT_ADDRESS,	
	0,
	0,
};
uint8_t pca6408a_init(pca6408a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		DBG_TRACE("iic device pca6408a addr=%d......no found\n",dev.devAddress);
		return REPLY_CHKERR;
	}
	else
	{
		DBG_TRACE("iic device pca6408a addr=%d......found\n",dev.devAddress);
		DBG_TRACE("out_data=%d\n",dev.out_data);
		DBG_TRACE("&out_data=%d\n",&dev.out_data);
		DBG_TRACE("(uint8_t *)dev.out_data=%d\n",(uint8_t *)dev.out_data);
		DBG_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
		
		u8 reg_data=0xff; SoftI2c.writes(dev.pins,1,dev.devAddress,6,&reg_data,1); //Set p0 as input
		
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,4,&reg_data,1);		 //set input Polarity
									 SoftI2c.writes(dev.pins,1,dev.devAddress,3,&dev.out_data,1);//set all outputs off
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,7,&reg_data,1);		 //set p1 as output
	}
	return REPLY_OK;
}

static u8 pca6408_inited=0;
void E75_Reg(uint8_t RegAddr,uint8_t RegValue)
{

	u8 reg_data=RegValue;
	if(pca6408_inited==0)
	{
		SoftI2c.pin_init(E75_pca6408a.pins);
		pca6408_inited=1;
	}
	SoftI2c.writes(E75_pca6408a.pins,1,E75_pca6408a.devAddress,RegAddr,&reg_data,1);
}

MSH_CMD_EXPORT(E75_Reg, set the output channel status);
FINSH_FUNCTION_EXPORT(E75_Reg,...);


#if 0
/** Default constructor, uses default I2C address.
 * @see TCA6408A_DEFAULT_ADDRESS
 */
TCA6408A::TCA6408A() {
    _address = TCA6408A_DEFAULT_ADDRESS;
}

bool TCA6408A::writeRegister(const uint8_t register_addr, const uint8_t value) {
    //send write call to sensor address
    //send register address to sensor
    //send value to register
    bool write_status = 0;
    Wire.beginTransmission(_address); //open communication with 
    Wire.write(register_addr);  
    Wire.write(value); 
    Wire.endTransmission(); 

    return write_status; //returns whether the write succeeded or failed
}

bool TCA6408A::writeRegisters(const uint8_t msb_register, const uint8_t msb_value, const uint8_t lsb_register, const uint8_t lsb_value) { 
    //send write call to sensor address
    //send register address to sensor
    //send value to register
    bool msb_bool, lsb_bool;
    msb_bool = writeRegister(msb_register, msb_value);
    lsb_bool = writeRegister(lsb_register, lsb_value);
    return 0; //returns whether the write succeeded or failed
}

bool TCA6408A::writeMaskedRegister(const uint8_t register_addr, const uint8_t mask, const uint8_t value) {
    uint8_t masked_value = (mask & value); //there has to be an easier way to do this.... I know, I know, shut up, I know it's that, I'll get around to it when I can ok?
    return writeRegister(register_addr, masked_value);
    //every reference to this is wrong (also)!! fix them!
}

uint8_t TCA6408A::readRegister(const uint8_t register_addr) {
    //call sensor by address
    //call registers
    uint8_t data = 0;

    Wire.beginTransmission(_address); 
    Wire.write(register_addr); 
    Wire.endTransmission(); 

    Wire.requestFrom((int)_address, 1);

    while(Wire.available()) {
        data = Wire.read();    // receive a byte as character
    }

    return data; //return the data returned from the register
}

uint16_t TCA6408A::readRegisters(const uint8_t msb_register, const uint8_t lsb_register) {
    uint8_t msb = readRegister(msb_register);
    uint8_t lsb = readRegister(lsb_register);
    return (((int16_t)msb) << 8) | lsb;

}

uint8_t TCA6408A::readMaskedRegister(const uint8_t register_addr, const uint8_t mask) {
    uint8_t data = readRegister(register_addr);
    return (data & mask);

    //every reference to this is wrong!!! fix them
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void TCA6408A::initialize(void) {
    Wire.begin(); 

    setClockSource(0);
    setFullScaleGyroRange(TCA6408A_GYRO_FULL_SCALE_250DPS);
    setFullScaleAccelRange(TCA6408A_FULL_SCALE_8G);
    setSleepEnabled(false);
}

// AUX_VDDIO register (InvenSense demo code calls this RA_*G_OFFS_TC)

uint8_t TCA6408A::readPins() {
    return readRegister(TCA6408A_OUTPUT);
}

bool TCA6408A::readOutput(uint8_t port_number) {
    return readMaskedRegister(TCA6408A_OUTPUT, (0xFE >> port_number)) != 0;
}

bool TCA6408A::readInput(uint8_t port_number) {
    return readMaskedRegister(TCA6408A_INPUT, (0x01 << port_number)) != 0;
}

void TCA6408A::setInput(uint8_t port_number) {
    uint8_t config = readRegister(TCA6408A_CONFIG);
    if (port_number < 8) {
        writeRegister(config || (0x01 << port_number))
    }
}

void TCA6408A::setOutput(uint8_t port_number) {
    uint8_t config = readRegister(TCA6408A_CONFIG);
    if (port_number < 8) {
        writeRegister(config && (0xFE << port_number))
    }
}
#endif

//

