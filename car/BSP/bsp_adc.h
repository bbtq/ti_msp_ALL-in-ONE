#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_


#include "board.h"

//ѭ��
extern bool gCheckADC1;        //ADC1�ɼ��ɹ���־λ
extern bool gCheckADC2;        //ADC2�ɼ��ɹ���־λ

void adc_start(void);
void adc_getValue(uint16_t * adcbuf);
    

#endif


