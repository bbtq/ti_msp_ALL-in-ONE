/*
 * ������������Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ
 * �����������www.lckfb.com
 * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ
 * ������̳��https://oshwhub.com/forum
 * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
 * ��������׬Ǯ���������й�����ʦΪ����
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-27     LCKFB-LP    first version
 */

#include "bsp_ultrasonic.h"


bool SR04_LEN_GET_FLAG = 0;

volatile unsigned int msHcCount = 0;//ms����
volatile float distance = 0;

/******************************************************************
 * �� �� �� �ƣ�bsp_ultrasonic
 * �� �� ˵ ������������ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��TRIG���Ÿ����ͳ��������崮
******************************************************************/
void Ultrasonic_Init(void)
{
        
    SYSCFG_DL_init();
    //�����ʱ���жϱ�־
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //ʹ�ܶ�ʱ���ж�
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    //������������GPIOA�˿��ж�
    NVIC_EnableIRQ(SR04_INT_IRQN);
        
}
/******************************************************************
 * �� �� �� �ƣ�Open_Timer
 * �� �� ˵ �����򿪶�ʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��
******************************************************************/
void Open_Timer(void)
{
        
    DL_TimerG_setTimerCount(TIMER_0_INST, 0);   // �����ʱ������  
        
    msHcCount = 0;  
        
    DL_TimerG_startCounter(TIMER_0_INST);   // ʹ�ܶ�ʱ��
}

/******************************************************************
 * �� �� �� �ƣ�Get_TIMER_Count
 * �� �� ˵ ������ȡ��ʱ����ʱʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ�����
 * ��       �ߣ�LC
 * ��       ע��
******************************************************************/
uint32_t Get_TIMER_Count(void)
{
    uint32_t time  = 0;  
    time   = msHcCount*100;                         // �õ�us 
    time  += DL_TimerG_getTimerCount(TIMER_0_INST);  // �õ�ms 
        
    DL_TimerG_setTimerCount(TIMER_0_INST, 0);   // �����ʱ������  
    delay_ms(10);
    return time ;          
}

/******************************************************************
 * �� �� �� �ƣ�Close_Timer
 * �� �� ˵ �����رն�ʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��
******************************************************************/
void Close_Timer(void)
{
    DL_TimerG_stopCounter(TIMER_0_INST);     // �رն�ʱ�� 
}

/******************************************************************
 * �� �� �� �ƣ�TIMER_0_INST_IRQHandler
 * �� �� ˵ ������ʱ���жϷ�����
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��1ms����һ��
******************************************************************/
void TIMER_0_INST_IRQHandler(void)
{
    //��������˶�ʱ���ж�
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://�����0����ж�
                                msHcCount++;
            break;
        
        default://�����Ķ�ʱ���ж�
            break;
    }
}


/******************************************************************
 * �� �� �� Hcsr04StartGet
 * �� �� ˵ ������ʼ��������
 * �� �� �� �Σ���
 * �� �� �� �أ���������
 * ��       �ߣ�bbtq
 * ��       ע����
******************************************************************/
void Hcsr04Start(void)
{
    Close_Timer();

    SR04_LEN_GET_FLAG = 0;

    SR04_TRIG(0);//trig�����źţ������͵�ƽs   
    delay_1us(10);//����ʱ�䳬��5us                        
            
    SR04_TRIG(1);//trig�����źţ������ߵ�ƽ
            
    delay_1us(20);//����ʱ�䳬��10us
            
    SR04_TRIG(0);//trig�����źţ������͵�ƽ
    /*Echo�����ź� �ȴ������ź�*/
                /*���뷽����ģ����Զ�����8��40KHz�����������ͬʱ�ز����ţ�echo���˵ĵ�ƽ����0��Ϊ1��
                ����ʱӦ��������ʱ����ʱ���������������ر�ģ����յ�ʱ���ز��� �Ŷ˵ĵ�ƽ����1��Ϊ0��
                ����ʱӦ��ֹͣ��ʱ������������ʱ�����µ����ʱ�伴Ϊ
                                                �������ɷ��䵽���ص���ʱ����*/
}

/******************************************************************
 * �� �� �� �ƣ�Hcsr04GetLength
 * �� �� ˵ ������ȡ��������
 * �� �� �� �Σ���
 * �� �� �� �أ���������
 * ��       �ߣ�bbtq
 * ��       ע����
******************************************************************/
volatile float length = 0;
float Hcsr04GetLength(void)
{
    SR04_LEN_GET_FLAG = 0;
    return length;
}

void GROUP1_IRQHandler(void)//Group1���жϷ�����
{
    //��ȡGroup1���жϼĴ���������жϱ�־λ
    switch( DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) )
    {
        //����Ƿ���KEY��GPIOA�˿��жϣ�ע����INT_IIDX������PIN_18_IIDX 
        case SR04_INT_IIDX:
            //����������±�Ϊ�ߵ�ƽ
            if( DL_GPIO_readPins(SR04_PORT, SR04_ECHO_PIN) > 0 )
            {
                //����LED����״̬��ת
                Open_Timer();   //�򿪶�ʱ�� 
                // printf("sr04 interrpt\r\n");
            }
            if( DL_GPIO_readPins(SR04_PORT, SR04_ECHO_PIN) == 0 )
            {
                Close_Timer();   // �رն�ʱ�� 
                length = (float)Get_TIMER_Count() / 58.0f;   // cm  
                SR04_LEN_GET_FLAG = 1;
            }
        break;
    }
}

