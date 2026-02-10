/*
 * UART.h
 *
 * Created: 26/01/2026 22:02:42
 *  Author: jose_
 */ 


#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void init_UART();
void WriteChar(char j);
void writeString(char* txt);




#endif /* UART_H_ */