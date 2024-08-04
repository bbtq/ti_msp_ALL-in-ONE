#include <stdio.h>
#include "math.h"
#include "bsp_jy901.h"

#define JY901_PACKET_LENGTH 11

union short_div {
    short data;
    uint8_t div[2];
};

static union short_div angle_yaw;

float jy901_yaw;

void copeJY901_data(uint8_t data)
{
    static uint8_t rxBuffer[JY901_PACKET_LENGTH + 1] = {0}; // ���ݰ�
    static uint8_t rxCount = 0;        // ���ռ���

    rxBuffer[rxCount++] = data; // ���յ������ݴ��뻺������

    if (rxBuffer[0] != 0x55)
    {
        // ����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
        rxCount = 0; // ��ջ�����
        return;
    }
    if (rxCount < JY901_PACKET_LENGTH)
        return; // ���ݲ���11�����򷵻�

    /*********** ֻ�н�����11���ֽ����� �Ż�������³��� ************/

    if (0x53 == rxBuffer[1]) // �ж����ݰ�У���Ƿ���ȷ
    {
        angle_yaw.div[0] = rxBuffer[6];
        angle_yaw.div[1] = rxBuffer[7];
        jy901_yaw = angle_yaw.data  / 32768.0f * 180.0f;
        // printf("jy901_yaw : %d . %d\r\n",(int)jy901_yaw,(int)(jy901_yaw*100)%100);
    }
    rxCount = 0;
}
