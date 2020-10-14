#ifndef __BSP_DEFINES_H_
#define __BSP_DEFINES_H_

#include "stm32f1xx_hal.h"


//	#include "stm32f1xx_hal_gpio.h"
//	#include "stm32f1xx_hal_uart.h"
//	#include "stm32f1xx_hal_spi.h"
//	#include "stm32f1xx_hal_tim.h"
	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"

typedef int32_t  				s32;
typedef int16_t 				s16;
typedef int8_t  				s8;

typedef const int32_t 	sc32;  
typedef const int16_t 	sc16;  
typedef const int8_t 		sc8;  

typedef __IO int32_t  	vs32;
typedef __IO int16_t  	vs16;
typedef __IO int8_t   	vs8;

typedef __I int32_t 		vsc32;  
typedef __I int16_t 		vsc16; 
typedef __I int8_t 			vsc8;   

typedef uint32_t  			u32;
typedef uint16_t 				u16;
typedef uint8_t  				u8;

typedef const uint32_t 	uc32;  
typedef const uint16_t 	uc16;  
typedef const uint8_t		uc8; 

typedef __IO uint32_t  	vu32;
typedef __IO uint16_t 	vu16;
typedef __IO uint8_t  	vu8;

typedef __I uint32_t 		vuc32;  
typedef __I uint16_t 		vuc16; 

typedef float						f32;
typedef double					f64;

typedef unsigned char 	UCHAR;                   //!< 8 bits unsigned
typedef unsigned short 	USHORT;                 //!< 16 bits unsigned
typedef unsigned int 		UINT;                     //!< 32 bits unsigned

/* Signed Types                                                              */
typedef signed char     		S8_T;
typedef short          	 		S16_T;
typedef long           			S32_T;
/* Unsigned Types                                                            */
typedef unsigned char   		U8_T;
typedef unsigned short  		U16_T;
typedef unsigned long   		U32_T;
typedef unsigned long long 	U64_T;
/* Float Types                                                               */
typedef float								F32_T;
typedef double							F64_T;
/* Boolean types declared as U8_T, as enums are generated as 16 bit          */
typedef U8_T								B_T;

/* Definitions for the two members of the Boolean type                       */
#ifndef FALSE
#define FALSE								((B_T) 0)
#endif

#ifndef TRUE
#define TRUE								((B_T) 1)
#endif

#ifndef OK_2
#define OK_2								((B_T) 2)
#endif

#ifndef IN_ON
#define IN_ON  0
#endif

#ifndef IN_OFF
#define IN_OFF  1
#endif

#ifndef UNUSED
#define UNUSED	((U8_T) 0xFF)
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef HIGH
#define HIGH  0x1
#endif

#ifndef LOW
#define LOW  0x0
#endif

#define LSB_FIRST 	0
#define MSB_FIRST 	1

#define PI 					3.1415926535898
#define NOP 				__NOP

#define MIN(a,b) (a<b) ? (a) : (b)
#define MAX(a,b) (a>b) ? (a) : (b)

#define min(X,Y)  ( (X) > (Y) ? 	(Y) : (X) ) 

#define   BIT0        0x00000001
#define   BIT1        0x00000002
#define   BIT2        0x00000004
#define   BIT3        0x00000008
#define   BIT4        0x00000010
#define   BIT5        0x00000020
#define   BIT6        0x00000040
#define   BIT7        0x00000080
#define   BIT8        0x00000100
#define   BIT9        0x00000200
#define   BIT10       0x00000400
#define   BIT11       0x00000800
#define   BIT12       0x00001000
#define   BIT13       0x00002000
#define   BIT14       0x00004000
#define   BIT15       0x00008000
#define   BIT16       0x00010000
#define   BIT17       0x00020000
#define   BIT18       0x00040000
#define   BIT19       0x00080000
#define   BIT20       0x00100000
#define   BIT21       0x00200000
#define   BIT22       0x00400000
#define   BIT23       0x00800000
#define   BIT24       0x01000000
#define   BIT25       0x02000000
#define   BIT26       0x04000000
#define   BIT27       0x08000000
#define   BIT28       0x10000000
#define   BIT29       0x20000000
#define   BIT30       0x40000000
#define   BIT31       0x80000000

#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */
#define interrupts() 			__enable_irq()//允许所有中断
#define no_interrupts() 	__disable_irq()//禁止所有中断

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <board.h>
#include <drivers/pin.h>

#include "user_finsh.h"

#define		PA_0				0
#define		PA_1				1
#define		PA_2				2
#define		PA_3				3
#define		PA_4				4
#define		PA_5				5
#define		PA_6				6
#define		PA_7				7
#define		PA_8				8
#define		PA_9				9
#define		PA_10				10
#define		PA_11				11
#define		PA_12				12
#define		PA_13				13
#define		PA_14				14
#define		PA_15				15

#define		PB_0				16
#define		PB_1				17
#define		PB_2				18
#define		PB_3				19
#define		PB_4				20
#define		PB_5				21
#define		PB_6				22
#define		PB_7				23
#define		PB_8				24
#define		PB_9				25
#define		PB_10				26
#define		PB_11				27
#define		PB_12				28
#define		PB_13				29
#define		PB_14				30
#define		PB_15				31

#define		PC_0				32
#define		PC_1				33
#define		PC_2				34
#define		PC_3				35
#define		PC_4				36
#define		PC_5				37
#define		PC_6				38
#define		PC_7				39
#define		PC_8				40
#define		PC_9				41
#define		PC_10				42
#define		PC_11				43
#define		PC_12				44
#define		PC_13				45
#define		PC_14				46
#define		PC_15				47

#define		PD_0				48   
#define		PD_1				49  
#define		PD_2				50  
#define		PD_3				51  
#define		PD_4				52  
#define		PD_5				53  
#define		PD_6				54  
#define		PD_7				55  
#define		PD_8				56  
#define		PD_9				57  
#define		PD_10				58  
#define		PD_11				59  
#define		PD_12				60  
#define		PD_13				61  
#define		PD_14				62  
#define		PD_15				63  

#define		PE_0				64
#define		PE_1				65
#define		PE_2				66
#define		PE_3				67
#define		PE_4				68
#define		PE_5				69
#define		PE_6				70
#define		PE_7				71
#define		PE_8				72
#define		PE_9				73
#define		PE_10				74
#define		PE_11				75
#define		PE_12				76
#define		PE_13				77
#define		PE_14				78
#define		PE_15				79

#define REPLY_OK 								0               //!< command successfully executed
#define REPLY_CHKERR 						1              	//!< checksum error
#define REPLY_INVALID_CMD		 		2        			  //!< command not supported
#define REPLY_WRONG_TYPE 				3          		  //!< wrong type code
#define REPLY_INVALID_VALUE 		4       				//!< wrong value
#define REPLY_EEPROM_LOCKED 		5       				//!< EEPROM is locked
#define REPLY_CMD_NOT_AVAILABLE 6   						//!< command not available due to current state
#define REPLY_CMD_LOAD_ERROR 		7      					//!< error when storing command to EEPROM
#define REPLY_WRITE_PROTECTED 	8     					//!< EEPROM is write protected
#define REPLY_MAX_EXCEEDED 			9        				//!< maximum number of commands in EEPROM exceeded

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*a))
#endif

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004

#define cpu_reset() ( SCB_AIRCR=SCB_RESET_VALUE)

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#endif


