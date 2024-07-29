/*
 * ������������Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ
 * �����������www.lckfb.com
 * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ
 * ������̳��https://oshwhub.com/forum
 * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
 * ��������׬Ǯ���������й�����ʦΪ����
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-23     LCKFB     first version
 */
#ifndef	__BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"

#define limit(value, min, max)      ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

extern char Serial_RxPacket[100];				//"@MSG\r\n"
extern uint8_t Serial_RxFlag;

void board_init(void);

void delay_us(unsigned long __us);
void delay_ms(unsigned long ms);
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);

void uart0_send_char(char ch);
void uart0_send_string(char* str);



#endif
