#include "bsp_eeprom_24xx.h"
#include "bsp_mcu_delay.h"

const device_t C24LC01   = {AT24C01, 0x007F, 0x08, 0x01,	1};
const device_t C24LC02   = {AT24C02, 0x00FF, 0x08, 0x01,	1};
const device_t C24LC04   = {AT24C04, 0x01FF, 0x10, 0x02,	2};
const device_t C24LC08   = {AT24C08, 0x03FF, 0x10, 0x04,	2};
const device_t C24LC16   = {AT24C16, 0x07FF, 0x10, 0x08,	2};
const device_t C24LC32   = {AT24C32, 0x0FFF, 0x20, 0x01,	2};
const device_t C24LC64   = {AT24C64, 0x1FFF, 0x20, 0x01,	2};
const device_t C24LC128  = {AT24C128,0x3FFF, 0x40, 0x01,	2};
const device_t C24LC256  = {AT24C256,0x7FFF, 0x40, 0x01,	2};
const device_t C24LC512  = {AT24C512,0xFFFF, 0x40, 0x01,	2};

const device_t C24LCM01  = {AT24CM01,0x1FFFF, 0x100, 0x01,	2};

//初始化IIC接口
void AT24CXX_Init(at24cxx_t dev)
{
	SoftI2c.pin_init(dev.pins);
}
void AT24CXX_Reads(at24cxx_t dev, u32 address,u8 *data,	u16 length)
{
	if(address+length < (dev.eeprom.maxAddress + 2)	)
	{
		if (dev.eeprom.type < AT24C04)
		{
			SoftI2c.reads(dev.pins, 1, dev.devAddress, address ,data,length);
		}
		else if (dev.eeprom.type > AT24C02 && dev.eeprom.type < AT24C32)
		{
			SoftI2c.reads(dev.pins, 3, dev.devAddress, address ,data,length);
		}
		else if (dev.eeprom.type > AT24C32 && dev.eeprom.type < AT24C512)
		{
			SoftI2c.reads(dev.pins, 2, dev.devAddress, address ,data,length);
		}	
		else 
		{
			SoftI2c.reads(dev.pins, 4, dev.devAddress, address ,data,length);
		}	
	}
}
/**
	ex:AT24C256_Writes(10,data,100):
	---1.	SizeLeft=100;				OffsetPage=10%64=10;	SizeNext=54 			write(10,&data[0],54);
	---2.	SizeLeft=100-54=46	OffsetPage=64%64=0;	 	SizeNext=46 			write(64,&data[54],46);
  ---3. SizeLeft==46-46=0		

 */
void AT24CXX_Writes(at24cxx_t dev, u32 address,u8 *data,	u16 length)
{	
//web site link:
//最后的地址还可以写1字节
	if((address+length ) < ( dev.eeprom.maxAddress + 2) )
	//if((address+length ) <= (dev.eeprom.maxAddress+1 ))
	{
		s16 SizeLeft	 = length;						// bytes left to write
		s16 OffsetData = 0;									// current offset in data pointer
		s16 OffsetPage = 0;									// current offset in page
		s16 SizeNext 	 = 0;									// next n bytes to write	
		
		while(SizeLeft>0)									  //还有数据要写
		{	
			//页偏移地址 	
			OffsetPage	 = address % dev.eeprom.pageSize;		
			SizeNext 		 = min(min(SizeLeft, dev.eeprom.pageSize), dev.eeprom.pageSize - OffsetPage);		
			//从页偏移地址开始写入下次可写的长度
			if (dev.eeprom.type < AT24C04)
			{
				SoftI2c.writes(dev.pins, 1, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			else if (dev.eeprom.type > AT24C02 && dev.eeprom.type < AT24C32)
			{
				SoftI2c.writes(dev.pins, 3, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			else if (dev.eeprom.type > AT24C32 && dev.eeprom.type < AT24C512)
			{
				SoftI2c.writes(dev.pins, 2, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			else SoftI2c.writes(dev.pins, 4, dev.devAddress, address,(data+OffsetData),SizeNext);
			drv_mdelay(5);
			SizeLeft-=SizeNext;							//剩下的字节数
			OffsetData+=SizeNext;						//下次需要写入的DATA位置
			address+=SizeNext;							//下次需要写入的ADDR位置	
		}
	}
}
//在AT24CXX指定地址读出一个数据
u8 AT24CXX_ReadU8(at24cxx_t dev, u16 address)
{				  
	AT24CXX_Reads(dev,address,dev.data,1);
	return dev.data[0];
}
/**
 * Write byte
 */
void AT24CXX_WriteU8(at24cxx_t dev, u16 address,u8 data)
{			
	AT24CXX_Writes(dev, address, &data,1);
	drv_mdelay(5);
}
//
u16 AT24CXX_ReadU16(at24cxx_t dev, u16 address)
{			
	AT24CXX_Reads(dev, address, dev.data, 2);
	return *(u16*)&dev.data[0];
}
//
void AT24CXX_WriteU16(at24cxx_t dev, u16 address,u16 data)
{			
	AT24CXX_Writes(dev, address, (u8*)&data,2);
	drv_mdelay(5);
}
/**
 * Read long
 */
u32 AT24CXX_ReadU32(at24cxx_t dev, u16 address)
{			
	AT24CXX_Reads(dev, address, dev.data, 4);
	return *(u32*)&dev.data[0];
}
/**
 * Write long
 */
void AT24CXX_WriteU32(at24cxx_t dev, u16 address,u32 data)
{		
	AT24CXX_Writes(dev, address, (u8*)&data,4);	
	drv_mdelay(5);
}

/**
 * Read float
 */
f32 AT24CXX_ReadF32(at24cxx_t dev, u16 address)
{		
	AT24CXX_Reads(dev, address, dev.data, 4);
	return *(float*)&dev.data[0];
}
/**
 * Write float
 */
void AT24CXX_WriteF32(at24cxx_t dev, u16 address,f32 data)
{			
	AT24CXX_Writes(dev, address, (u8*)&data,4);	
	drv_mdelay(5);
}
//armfly
uint8_t AT24CXX_CheckOk(at24cxx_t dev)
{
	if (SoftI2c.check(dev.pins ,dev.devAddress ) == 0)
	{
		return 1;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		SoftI2c.stop(dev.pins);
		return 0;
	}
}

uint8_t ee_ReadBytes(at24cxx_t dev,uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;

	/* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

	/* 第1步：发起I2C总线启动信号 */
	SoftI2c.start(dev.pins);

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_WR);	/* 此处是写指令 */

	/* 第3步：发送ACK */
	if (SoftI2c.waitack(dev.pins) != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	if (dev.eeprom.addrBytes == 1)
	{
		SoftI2c.sendbyte(dev.pins,(uint8_t)_usAddress);
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}
	}
	else
	{
		SoftI2c.sendbyte(dev.pins,(_usAddress >> 8));
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}

		SoftI2c.sendbyte(dev.pins,(uint8_t)_usAddress);
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}
	}

	/* 第6步：重新启动I2C总线。下面开始读取数据 */
	SoftI2c.start(dev.pins);

	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_RD);	/* 此处是读指令 */

	/* 第8步：发送ACK */
	if (SoftI2c.waitack(dev.pins) != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第9步：循环读取数据 */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = SoftI2c.readbyte(dev.pins,0);	/* 读1个字节 */

		/* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
		if (i != _usSize - 1)
		{
			SoftI2c.ack(dev.pins);	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
		}
		else
		{
			SoftI2c.nack(dev.pins);	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
	}
	/* 发送I2C总线停止信号 */
	SoftI2c.stop(dev.pins);
	return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	SoftI2c.stop(dev.pins);
	return 0;
}
//
uint8_t ee_WriteBytes(at24cxx_t dev, uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;

	/*
		写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
		对于24xx02，page size = 8
		简单的处理方法为：按字节写操作模式，每写1个字节，都发送地址
		为了提高连续写的效率: 本函数采用page wirte操作。
	*/

	usAddr = _usAddress;
	for (i = 0; i < _usSize; i++)
	{
		/* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
		if ((i == 0) || (usAddr & (dev.eeprom.pageSize - 1)) == 0)
		{
			/*　第０步：发停止信号，启动内部写操作　*/
			SoftI2c.stop(dev.pins);

			/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
				CLK频率为200KHz时，查询次数为30次左右
			*/
			for (m = 0; m < 1000; m++)
			{
				/* 第1步：发起I2C总线启动信号 */
				SoftI2c.start(dev.pins);

				/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
				SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_WR);	/* 此处是写指令 */

				/* 第3步：发送一个时钟，判断器件是否正确应答 */
				if (SoftI2c.waitack(dev.pins) == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM器件写超时 */
			}

			/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
			if (dev.eeprom.addrBytes == 1)
			{
				SoftI2c.sendbyte(dev.pins,(uint8_t)usAddr);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM器件无应答 */
				}
			}
			else
			{
				SoftI2c.sendbyte(dev.pins,usAddr >> 8);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM器件无应答 */
				}

				SoftI2c.sendbyte(dev.pins,usAddr);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM器件无应答 */
				}
			}
		}

		/* 第6步：开始写入数据 */
		SoftI2c.sendbyte(dev.pins,_pWriteBuf[i]);

		/* 第7步：发送ACK */
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}

		usAddr++;	/* 地址增1 */
	}

	/* 命令执行成功，发送I2C总线停止信号 */
	SoftI2c.stop(dev.pins);
	return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	SoftI2c.stop(dev.pins);
	return 0;
}


//armfly

AT24CXX_DrvTypeDef at24cxx=
{
	AT24CXX_Init,
	AT24CXX_CheckOk,
	AT24CXX_Reads,
	AT24CXX_Writes,
	
	AT24CXX_ReadU8,
	AT24CXX_WriteU8,
	
	AT24CXX_ReadU16,
	AT24CXX_WriteU16,
	
	AT24CXX_ReadU32,
	AT24CXX_WriteU32,
	
	AT24CXX_ReadF32,
	AT24CXX_WriteF32,
	
	ee_ReadBytes,
	ee_WriteBytes,
};

/*********************************   END  *********************************
*/
