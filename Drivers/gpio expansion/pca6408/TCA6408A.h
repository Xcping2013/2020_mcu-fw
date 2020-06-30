/** Based on InvenSense TCA6408A datasheet
* 03/05/2014 by Conor Forde <me@conorforde.com>
* Updates should be available at https://github.com/Snowda/TCA6408A
*/

#ifndef _TCA6408A_H_
#define _TCA6408A_H_


#include "bsp_mcu_softi2c.h"

// TCA6408A addr
//#define TCA6408A_DEFAULT_ADDRESS        0x20
//#define TCA6408A_ALT_DEFAULT_ADDRESS    0x21
//Register map
#define TCA6408A_INPUT                  0x00
#define TCA6408A_OUTPUT                 0x01
#define TCA6408A_POLARITY               0x02
#define TCA6408A_CONFIG                 0x03

// TCA6408A masks
#define TCA6408A_PIN0                   0x01
#define TCA6408A_PIN1                   0x02
#define TCA6408A_PIN2                   0x04
#define TCA6408A_PIN3                   0x08
#define TCA6408A_PIN4                   0x10
#define TCA6408A_PIN5                   0x20
#define TCA6408A_PIN6                   0x40
#define TCA6408A_PIN7                   0x80


typedef struct 
{
	IIC_GPIO	pins;
	uint8_t devAddress;
	uint8_t in_data;	
	uint8_t out_data;	
	
}pca6408a_t;

uint8_t pca6408a_init(pca6408a_t dev);

#if 0
class TCA6408A {
    public:
        TCA6408A();
        void initialize();

        uint8_t readInputs();
        bool readPin0(void);
        bool readPin1(void);
        bool readPin2(void);
        bool readPin3(void);
        bool readPin4(void);
        bool readPin5(void);
        bool readPin6(void);
        bool readPin7(void);

        void pinLow0(void);
        void pinLow1(void);
        void pinLow2(void);
        void pinLow3(void);
        void pinLow4(void);
        void pinLow5(void);
        void pinLow6(void);
        void pinLow7(void);

        void pinHigh0(void);
        void pinHigh1(void);
        void pinHigh2(void);
        void pinHigh3(void);
        void pinHigh4(void);
        void pinHigh5(void);
        void pinHigh6(void);
        void pinHigh7(void);

        void writeOutputs(const uint8_t outputs);

        void setInput(const uint8_t port_number);
        void setOutput(const uint8_t port_number);

        void setHigh();
        void setLow();

    private:
        bool writeRegister(const uint8_t register_addr, const uint8_t value);
        bool writeRegisters(const uint8_t msb_register, const uint8_t msb_value, const uint8_t lsb_register, const uint8_t lsb_value);
        bool writeMaskedRegister(const uint8_t register_addr, const uint8_t mask, const uint8_t value);

        uint8_t readRegister(const uint8_t register_addr);
        uint16_t readRegisters(const uint8_t msb_register, const uint8_t lsb_register);
        uint8_t readMaskedRegister(const uint8_t register_addr, const uint8_t mask);


        uint8_t _address;
        uint8_t _whoami;
};
#endif

#endif /* _TCA6408A_H_ */

