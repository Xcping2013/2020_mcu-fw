#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softI2c.h"

void 		SoftI2c_Init(IIC_GPIO pin);
void 		SoftI2c_start(IIC_GPIO pin);
void 		SoftI2c_stop(IIC_GPIO pin);
uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address);
uint8_t SoftI2c_wait_ack(IIC_GPIO pin);
void 		SoftI2c_send_ack(IIC_GPIO pin);
void 		SoftI2c_send_no_ack(IIC_GPIO pin);
void 		SoftI2c_send_byte(IIC_GPIO pin,uint8_t byte);
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack);
uint8_t SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint32_t reg_address, uint8_t *data, uint16_t num_to_read);
uint8_t SoftI2c_writes(IIC_GPIO pin, uint8_t DeviceType, uint8_t slave_address, uint32_t reg_address, uint8_t *data, uint16_t num_to_write);

SoftI2c_DrvTypeDef	SoftI2c=
{
	SoftI2c_Init,
	SoftI2c_CheckDevice,
	SoftI2c_start,
	SoftI2c_stop,
	SoftI2c_wait_ack,
	SoftI2c_send_ack,
	SoftI2c_send_no_ack,
	SoftI2c_send_byte,
	SoftI2c_read_byte,
	SoftI2c_writes,
	SoftI2c_reads,	
};

//IIC引脚电路未加上拉电阻值，需要设置为PIN_MODE_OUTPUT  有上拉时可设为PIN_MODE_OUTPUT_OD
void SoftI2c_Init(IIC_GPIO pin)
{
    rt_pin_mode(pin.sda_pin,PIN_MODE_OUTPUT_OD);	
    rt_pin_mode(pin.scl_pin,PIN_MODE_OUTPUT_OD);	
		SoftI2c_stop(pin);
}
/*						基本时序						 */
//产生IIC起始信号
void SoftI2c_start(IIC_GPIO pin)
{
	pinMode(pin.sda_pin,PIN_MODE_OUTPUT_OD);	//sda线输出

  pinSet(pin.sda_pin)  ;
	pinSet(pin.scl_pin)  ;
  drv_udelay(4);
  pinReset(pin.sda_pin)  ;								//START:when CLK is high,DATA change form high to low 
  drv_udelay(4);
  pinReset(pin.scl_pin)  ;								//钳住I2C总线，准备发送或接收数据 
	drv_udelay(4);
}
void SoftI2c_stop(IIC_GPIO pin)
{
    pinMode(pin.sda_pin,PIN_MODE_OUTPUT_OD);//sda线输出
    pinReset(pin.sda_pin)  ;
		drv_udelay(1);
    pinSet(pin.scl_pin)  ;								//STOP:when CLK is high DATA change form low to high
    drv_udelay(4);
    pinSet(pin.sda_pin)  ;								//发送I2C总线结束信号
    drv_udelay(4);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t SoftI2c_wait_ack(IIC_GPIO pin)
{
	uint8_t ucErrTime=0;
	pinMode(pin.sda_pin,PIN_MODE_INPUT_PULLUP);
	drv_udelay(1);	
	pinSet(pin.scl_pin)  ;
	drv_udelay(1);
	while(pinRead(pin.sda_pin))
	{
			ucErrTime++;
			if(ucErrTime>250)
			{
					//pinMode(SoftI2c.sda_pin,PIN_MODE_OUTPUT);
					SoftI2c_stop(pin);
					return 1;

			}

	}
	pinReset(pin.scl_pin)  ;
	drv_udelay(1);
	return 0;
}

//产生ACK应答
void SoftI2c_send_ack(IIC_GPIO pin)
{
	pinReset(pin.scl_pin)  ;							//L
	pinMode(pin.sda_pin,PIN_MODE_OUTPUT_OD);
	pinReset(pin.sda_pin)  ;							//D
	drv_udelay(2);
	pinSet(pin.scl_pin)  ;								//H
	drv_udelay(2);
	pinReset(pin.scl_pin)  ;
	drv_udelay(2);
	pinSet(pin.sda_pin)  ;		/* CPU释放SDA总线 */				
}
//产生NACK		
void SoftI2c_send_no_ack(IIC_GPIO pin)
{
		pinReset(pin.scl_pin)  ;
    pinMode(pin.sda_pin,PIN_MODE_OUTPUT_OD);
    pinSet(pin.sda_pin)  ;
    drv_udelay(2);
    pinSet(pin.scl_pin)  ;
    drv_udelay(2);
    pinReset(pin.scl_pin)  ;
    drv_udelay(1);
}

uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address)
{
	uint8_t ucAck;

	if ( pinRead(pin.sda_pin)  &&  pinRead(pin.scl_pin))
	{
		SoftI2c_start(pin);		/* 发送启动信号 */

		/* 发送设备地址+读写控制bit（0 = w|I2C_WR， 1 = r) bit7 先传 */
		SoftI2c_send_byte(pin,_Address | 0 );
		ucAck = SoftI2c_wait_ack(pin);	/* 检测设备的ACK应答 */

		SoftI2c_stop(pin);			/* 发送停止信号 */

		return ucAck;
	}
	return 1;	/* I2C总线异常 */	
}
/**
 * @brief 发送数据.
 *
 * @param[in] byte 将被发送的数据.
 *
 * @return 发送结果.返回0表示发送成功，返回1表示发送失败.
 */
void SoftI2c_send_byte(IIC_GPIO pin,uint8_t byte)
{
    //uint8_t ret = 0;
    uint8_t t;
    pinMode(pin.sda_pin,PIN_MODE_OUTPUT_OD);
		pinReset(pin.scl_pin)  ;									//拉低时钟开始数据传输
    drv_udelay(1);
		for(t=0;t<8;t++)	
    {
        pinWrite(pin.sda_pin , (byte & 0x80)>>7);
				byte<<=1;
        drv_udelay(1);

        pinSet(pin.scl_pin)  ;
        drv_udelay(2);
        pinReset(pin.scl_pin)  ;
        drv_udelay(1);
    }
		pinSet(pin.sda_pin)  ;		/* CPU释放SDA总线 */
//    ret = SoftI2c_wait_ack();
//    return ret;
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack)
{
    uint8_t i = 8;
    uint8_t byte = 0;
    pinMode(pin.sda_pin,PIN_MODE_INPUT_PULLUP);
    for(i=0;i<8;i++ )
    {
				drv_udelay(2);
        pinSet(pin.scl_pin)  ;
				byte<<=1;	
				if(pinRead(pin.sda_pin))	byte++;
        drv_udelay(2);
				pinReset(pin.scl_pin)  ;
    }
		if (!ack)
				SoftI2c_send_no_ack(pin);//发送nACK
		else
				SoftI2c_send_ack(pin); 	//发送ACK  

		return byte;
}
/*						基本时序						 */

//地址1~2字节设备,多为大容量EEPROM,设备REG比较少,一字节
uint8_t SoftI2c_writes(IIC_GPIO pin, uint8_t DeviceType, uint8_t slave_address, uint32_t reg_address, uint8_t *data, uint16_t num_to_write)
{
  SoftI2c_start(pin);  
	switch(DeviceType)
	{
		case 0:	//寄存器只有一个，无需地址指明
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //写器件地址 	  
						if(SoftI2c_wait_ack(pin)) return 1;
				break;
		case 1:	//一字节寄存器地址			
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //写器件地址 	  
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin , reg_address%256);  		 	//发送低地址
						if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 2:	//两字节寄存器地址
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //写器件地址 	  
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin, reg_address>>8);				//发送高地址
						if(SoftI2c_wait_ack(pin)) return 2;
						SoftI2c_send_byte(pin, reg_address%256);  		 //发送低地址
						if(SoftI2c_wait_ack(pin)) return 2;	 
			break;
		case 3:	//寄存器地址与器件地址复用
						SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address/256)<<1));   //发送器件地址0XA0,写数据 
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin, reg_address%256);   			 //发送低地址
						if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 4:	//寄存器地址与器件地址复用,
					SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address>>16)<<1));   //发送器件地址0XA0,写数据 
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, ((uint16_t)reg_address)>>8);   			 //发送FIRST WORLD地址
					if(SoftI2c_wait_ack(pin)) return 2;
					SoftI2c_send_byte(pin, (uint8_t)reg_address);   			 //发送SECORD WORLD地址
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		default:
			break;		
	}
		
  while(num_to_write--)
	{
		SoftI2c_send_byte(pin, *(data++));     		//发送字节							   
		if(SoftI2c_wait_ack(pin)) return 3;
	}
  SoftI2c_stop(pin);													//产生一个停止条件 
	delay_ms(10);
  return 0;
}


//uint8_t 	SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_read)
uint8_t 	SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint32_t reg_address, uint8_t *data, uint16_t num_to_read)
{
	switch(DeviceType)
	{
		case 0:	//寄存器只有一个，无需地址指明
			break;
		case 1:	//一字节寄存器地址			
				SoftI2c_start(pin);
				SoftI2c_send_byte(pin, slave_address & 0xFE); 		//写器件地址 	  
				if(SoftI2c_wait_ack(pin)) return 1;
				SoftI2c_send_byte(pin, reg_address %256);				//发送低地址
				if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 2:	//两字节寄存器地址
					SoftI2c_start(pin);
					SoftI2c_send_byte(pin, slave_address & 0xFE);   //写器件地址 	  
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, reg_address>>8);				 //发送高地址
					if(SoftI2c_wait_ack(pin)) return 2;
					SoftI2c_send_byte(pin, reg_address%256);  		 	//发送低地址
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 3:	//寄存器地址与器件地址复用
					SoftI2c_start(pin);
					SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address/256)<<1));   //发送器件地址0XA0,写数据 
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, reg_address%256);   			 //发送低地址
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 4:	//寄存器地址与器件地址复用,
					SoftI2c_start(pin);
					SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address>>16)<<1));   //发送器件地址0XA0,写数据 
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, ((uint16_t)reg_address)>>8);   			 //发送FIRST WORLD地址
					if(SoftI2c_wait_ack(pin)) return 2;
					SoftI2c_send_byte(pin, (uint8_t)reg_address);   			 //发送SECORD WORLD地址
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		default:
			break;	
		
	}
	SoftI2c_start(pin);
	SoftI2c_send_byte(pin, slave_address | 0x01);					//进入接收模式
	if(SoftI2c_wait_ack(pin)) return 3;

	while(num_to_read!=1)
	{
		num_to_read--;
		*data++ = SoftI2c_read_byte(pin, 1);
	}
	*data++ = SoftI2c_read_byte(pin, 0);
	SoftI2c_stop(pin);
	return 0;
}
//

//#define EEPROM_RTT_DEBUG

//#ifdef 	EEPROM_RTT_DEBUG
//#define EEPROM_TRACE         rt_kprintf
//#else
//#define EEPROM_TRACE(...)     
//#endif
///**********************************AT24CXX_USER_APP**********************************
//*/
//at24cxx_t at24c256=
//{
//	{0},
//	{PB_4,PB_3},	//SDA SCL
//	0xA0,
//};

//void at24cxx_hw_init(void)
//{
//	at24c256.eeprom=C24LC256;//C24LC256;
//	at24cxx.init(at24c256);
//}

//rt_err_t save_data(u8 pageNum , char *Commands)
//{
//	u8 len=strlen(Commands);
//	if(len<65	&& pageNum>0)	
//	{
//		if(len!=64)	{Commands[len]='\0';len=len+1;}
//		at24cxx.write(at24c256,pageNum*64,Commands,len);
//		EEPROM_TRACE("<ok>");
//	}
//	else EEPROM_TRACE("<pageNum must be 1~255,commands len must be 1~64>");
//}

//void read_data(u8 pageNum)
//{
//	u8 read_info[64];
//	u8 i;
//	if(pageNum>0)	
//	{
//		at24cxx.read(at24c256 , pageNum*64, read_info, 64);
//		for(i=0;i<64;i++)	
//		{
//			if(read_info[i]>127) { read_info[i]='\0';break;}
//		}
//		EEPROM_TRACE("<%.64s>",read_info);
//	}
//	else EEPROM_TRACE("<pageNum must be 1~255>");
//}

///*********************************   END  *********************************
//*/

