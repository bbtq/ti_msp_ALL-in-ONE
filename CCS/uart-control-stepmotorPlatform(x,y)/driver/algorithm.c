/*
 * algorithm.c
 *
 *  Created on: 2024��6��7��
 *      Author: Admin
 */

#include "ti_msp_dl_config.h"

/**
 * @brief �޷�����
*         if(amt<low)   amt = low;
*               else         amt = amt;
*         if(amt>high)   amt = high;
*               else         amt = amt;
*/

uint8_t constrain_uint8_t(uint8_t amt, uint8_t low, uint8_t high)//�޷�
{
  return ((amt)<(low)?(low):((amt) >(high)?(high):(amt)));
}

uint8_t rotate_left_with_bit7_to_bit0(uint8_t value) {
    // ȡ�� bit7 ��ֵ
    uint8_t bit7 = (value & 0x80) >> 7;
    // ����һλ���� bit7 �ŵ� bit0
    return (value << 1) | bit7;
}

uint8_t rotate_right_with_bit0_to_bit7(uint8_t value) {
    // ȡ�� bit0 ��ֵ
    uint8_t bit0 = (value & 0x01);
    // ����һλ���� bit0 �ŵ� bit7
    return (value >> 1) | (bit0 << 7);
}

