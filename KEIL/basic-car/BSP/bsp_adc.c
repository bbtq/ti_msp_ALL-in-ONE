
#include "bsp_adc.h"
#include "bsp_Filter.h"

bool gCheckADC1 = false ;
bool gCheckADC2 = false ;


void adc_start(void)
{
    //�������ADC��ʼת��
    DL_ADC12_startConversion(adc_INST);

}

//��ȡADC������
void adc_getValue(uint16_t * adcbuf)
{
    //��ȡ����
    adcbuf[0] = DL_ADC12_getMemResult(adc_INST, adc_ADCMEM_1);
    adcbuf[1] = DL_ADC12_getMemResult(adc_INST, adc_ADCMEM_2);

    //�������˲�
    adcbuf[0] = kalman_update(&kf_adc1,(float)adcbuf[0]);
    adcbuf[1] = kalman_update(&kf_adc2,(float)adcbuf[1]);
    
    //�����־λ
    gCheckADC1 = false;
    gCheckADC2 = false;

}

float adc_getvoltage(uint16_t adc_value)
{
    return (adc_value/4095.0f*3.3f);
}

//ADC�жϷ�����
void adc_INST_IRQHandler(void)
{
        //��ѯ�����ADC�ж�
        switch (DL_ADC12_getPendingInterrupt(adc_INST)) 
        {
                        //����Ƿ�������ݲɼ�
                        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
                                        gCheckADC1 = true;//����־λ��1
                                        break;
                        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
                                        gCheckADC2 = true;//����־λ��1
                                        break;
                        default:
                                        break;
        }
}


