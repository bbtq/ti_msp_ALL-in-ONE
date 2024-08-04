/*
 * myuart.h
 *
 *  Created on: 2024��7��11��
 *      Author: Admin
 */

#ifndef DRIVER_MYUART_H_
#define DRIVER_MYUART_H_

extern char rxbuf[100];
extern uint8_t rxflag;

void uart0_pack_transmit(uint8_t rxdata);
uint8_t get_rxpack(uint8_t *rxbuf);

#endif /* DRIVER_MYUART_H_ */
