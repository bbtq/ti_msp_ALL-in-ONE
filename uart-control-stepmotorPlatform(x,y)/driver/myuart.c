/*
 * myuart.c
 *
 *  Created on: 2024��7��11��
 *      Author: Admin
 */

#include "ti_msp_dl_config.h"

char rxbuf[100] = {};
uint8_t rxflag = 0;

void uart0_pack_transmit(uint8_t rxdata){
    static uint8_t len = 2 - 1;     //�������ݰ�����
    static uint8_t now_len = 0; //�������浱ǰ���浽�ڼ�λ
    rxbuf[now_len] = rxdata;
    now_len = now_len==len ? 0 : now_len+1;


    if(now_len==0){
        rxflag = 1;
//        DL_UART_Main_transmitData(UART_0_INST, rxflag); //�����յ������ݷ�����ȥ
    }
}

uint8_t get_rxpack(uint8_t *rxbuf){

}

