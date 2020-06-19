/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "bsp_defines.h"

#define ENABLE_TEST_CODE		0
#define ENABLE_DEBUG_CODE		1

/*
extern int i; 申明
int i;				定义

*/
int code_sizeof_test(void)
{
	uint8_t *p = NULL;

	rt_kprintf("size of p = %d\n",sizeof(p));							//4
	rt_kprintf("size of *p = %d\n",sizeof(*p));						//4
	
	int a[100];
	rt_kprintf("size of a = %d\n",sizeof(a));							//400
	rt_kprintf("size of &a = %d\n",sizeof(&a));						//4
	rt_kprintf("size of a[100] = %d\n",sizeof(a[100]));		//4
	rt_kprintf("size of &a[100] = %d\n",sizeof(&a[0]));		//4
	
	
	rt_kprintf("size of int = %d\n",sizeof(int));					//4
	rt_kprintf("size of long = %d\n",sizeof(long));				//4
	
	char b[1000];	//[-128,127]

	for(int i=0; i<1000; i++)
	{
		b[i] = -1-i;
	}
	rt_kprintf("%d\n",strlen(b));													//255

	return RT_EOK;
}
int code_strlen_test(void)
{
	char a[1000];	//[-128,127]

	for(int i=0; i<1000; i++)
	{
		a[i] = -1-i;
	}
	rt_kprintf("%d",strlen(a));													//255

	return RT_EOK;
}
void code_defined_test(void)
{
	#if (ENABLE_TEST_CODE)
		rt_kprintf("test code is enable\n");									//disable
	#endif
	#if defined(ENABLE_TEST_CODE)
		rt_kprintf("test code is enable\n");									//enable
	#endif
}

//MSH_CMD_EXPORT(code_sizeof_test, ......);
//MSH_CMD_EXPORT(code_defined_test, ......);
//MSH_CMD_EXPORT(code_strlen_test, ......);

void eeprom_fun_analysis(void)
{
	
/*	
		
		页偏移地址	下次需要写入长度	剩余长度	要写入的数据偏移 下次eeprom写地址				操作EEPROM
		address%64																														address,(data+OffsetData),SizeNext
						 min(min(SizeLeft, 64), 64 - OffsetPage)	
																SizeLeft-=SizeNext;
																						OffsetData+=SizeNext;
																															address+=SizeNext;	
		OffsetPage			SizeNext			SizeLeft			OffsetData			address
0			0								0							100							0							0				
1			0								64						36							64						64				0,string,64																												
2.	  1								36					  0								100						100				64,string+64,36

0			0								0							1000						0							0				
1			0								64						936							64						64				0,string,64																												
2.	  0								64					  872							128					 128				64,string+64,64
...
	
*/
		
		
		
		
	}
}

