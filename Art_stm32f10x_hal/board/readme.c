
//�����������ļ� #include "tmc429_interface.h"		tmc429_config.c

//

#if 0			//Main research direction

1. ȫ���忨FW���̻㼯��һ��Keil�����£�ÿ���ļ�������һ������ϵ���ļ�����ͬ��Ӧ�ñ��벻ͬ���ļ���
   ---��Ҫע����Ƕ��.c�ļ�����ͬ����ʹ�����α�����ѡ��
	 ---.h�ļ��޷����Σ�Ҫô������һ��������.h�ļ��ڹ��õ�ͷ�ļ���;Ҫôֱ����ͨ��ͷ�ļ��н���ʹ������
			//#if board #else board #endif
	 
2. EEPROMͨ�����ã� "app_eeprom_24xx.h"�� 

		A:����2��������Բ������е�EEPROM�ռ䣬SN����洢�ڵ�һҳ
		EEPROM Read|WritePage
		EEPROM Read|Write
		B������read|savedata����Ҫע���ַ�Ƿ���ͻ���������ݱ�����



#endif


/********************************Version ChangeList  ****************************/
#if 0			//INC-MB1616DEV

FW:INC-MB1616DEV Ver1.0:

	 BSP_GPIO_PCA95XX								FB_RGB_ISL29125		
	 BSP_ADC_MCP3421								FB_CC_TLC5973
	 BSP_EEPROM_AT24C128						FB_MOTOR_TMC429


#endif

#if 0			//FB_RGB_ISL29125	


#endif

#if 0			//FB_MOTOR_TMC429


�źŴ�����Ч��ƽ��

��λ������δ����ʱΪ�͵�ƽ��������Ϊ�ߵ�ƽ ---Ĭ�ϲ��Ӵ�����=������λ

ԭ�㴥����Ч��ƽ��
uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};

#endif



/********************************Commands msh   ******************************************
�����ն����ã�

#define RT_DEVICE_FLAG_STREAM 0x040    
	��ģʽ���������򴮿��ն�����ַ�������������ַ��ǡ�\n�� ʱ���Զ���ǰ�油һ�� ��\r�� ������
//#define FINSH_ECHO_DISABLE_DEFAULT  
//	���������������

����Ӧ��
1.config
Baudrate:	115200	Data Bits:8		Parity: none	Stop Bits:1
2.PC�����ַ���������\r\n���������ư巵�������� \r\n>>���� 
3.�����б���Է���help�鿴�������ʽ��cmd ����1 ����2 ...
4.�������Ͷ�������ʱ���������20ms
5.ͨѶ�쳣����command not found�Ļ������ط����

6.�����е�<>�ڵ�����Ϊ���͵Ĳ��� ʵ�ʷ���������Ҫȥ���˷���
	�����е�[]�ڵ�����Ϊ���صĲ��� 
	
help				�����б��ӡ
version			��ӡ�忨��Ϣ
reboot			���ƿ�����
*****************************************************************************************/

#if 0			//Commands msh GPIO

readinput <ch> ��ȡ����ͨ��ch��״̬

send1:		readinput 1					send2:		readinput 1 2 2 3	
receive1��in[1]=0						  receive2:	in[1]=0,in[2]=0,in[2]=0,in[3]=0

output on	<ch>	�������ͨ��ch״̬Ϊ1

send1��		output on 1					send2:		output on 1 2 2 3	
receive1��out[1]=1						receive2:	out[1]=1,out[2]=1,out[2]=1,out[3]=1

output off	<ch>	�������ͨ��ch״̬Ϊ0

send1��		output off 1				send2:		output off 1 2 2 3	
receive1��out[1]=0						receive2:	out[1]=0,out[2]=0,out[2]=0,out[3]=0

#endif

#if 0			//Commands msh EEPROM

savedata	<page>	<data>	��1~200�洢λ��д������,ÿҳ���洢�ĳ���Ϊ64�ֽڣ���֧�����ݴ��ո�,��Ҫ�ѿո��������ַ��滻

send��		savedata 1 this_data_is_for_test:position1=100;position2=200;
receive: 	savedata[1] ok

readdata	<page>	��ȡ1~200�洢λ�õ�����

send��		readdata 1
receive:	readdata[1]=this_data_is_for_test:position1=100;position2=200;

#endif

#if 0			//Commands msh MOTOR��axis_number=0~5 )

motor	stop	<axis_number>			���ֹͣ���в����ص�ǰλ��

send��		motor stop	1
receive:	motor[1] is stop and P[1]=0

motor  		set  speed	<axis_number>	<value>	�趨���ÿ�����е�����������������΢����أ������õ�����ٶ�Ϊ3814 /2=1907
send�� 		motor set speed	1 105
receive:	set motor[1] speed=105 ok

motor	move	<axis_number>	<position>	�������˶����趨ֵ

send��		motor move 1 1000
receive:	motor[1] is start to make relative motion

motor	get	position	<axis_number>			��ȡ��Ӧ�ᵱǰλ��

send��		motor get position	1
receive:	motor[1] actual position=0	

motor	get	is_stop	<axis_number>				��ȡ��Ӧ���Ƿ���ֹͣ״̬

send��		motor get is_stop	1
receive:	motor[1] stop=1	


motor	 golimit <motor number> <velocity>   ---motor search limit sensor

send��		motor	 golimit 0 1600   ����β������˳ʱ���ҵ�����λʱֹͣ
receive:	motor[0] is start go home by searching limit sensor
					---
					motor[0] golimit ok

send��		motor	 golimit 0 -1600  ����β��������ʱ���ҵ�����λʱֹͣ
receive:	motor[0] is start go home by searching limit sensor
					---
					motor[0] golimit ok
					
motor	 gohome  <motor number> <velocity>   ---motor search limit then search the origin sensor

send��		motor	 gohome  0 1600   ����β������˳ʱ���ȸ����ҵ�����λ����ʱ�������ԭ��
receive:	motor[0] is start go home by searching home sensor
					---
					motor[0] gohome ok

send��		motor	 gohome  0 -1600   ����β��������ʱ�����ҵ�����λ��˳ʱ����ԭ��
receive:	motor[0] is start go home by searching home sensor
					---
					motor[0] gohome ok

#endif


