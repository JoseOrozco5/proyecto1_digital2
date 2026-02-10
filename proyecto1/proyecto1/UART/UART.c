/*
 * UART.c
 *
 * Created: 26/01/2026 22:02:28
 *  Author: jose_
 */ 
//Librerias
#include "UART.h"

//NON-Interrupt subroutines
void init_UART()
{
	DDRD |= (1 << DDD1);															// Tx como salida
	DDRD &= ~(1 << DDD0);															// Rx como entrada
	
	UCSR0A = 0;																		// Configurar UCSR0A
	
	UCSR0B |=  (1 << TXEN0) | (1 << RXEN0 ) | (1 << RXCIE0);						// habilitar bit para recibir, transmitir y habilitar interrupción
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);										// CHARACTER SIZE 8 BITS Y 1 STOP BIT
	UBRR0 = 103;																		// BAUD RATE 9600 0.2% DE ERROR
}

void WriteChar(char j)
{
	//while((UCSR0A & (1 << UDRE0)) == 0);
	while(!(UCSR0A &(1 << UDRE0)));							// esperar a que se vacie el buffer (!= es no sea)
	UDR0 = j;												// meter valor
}


void writeString(char* txt)												// el char* sirve para representar cadenas de texto
{
	for (uint8_t i = 0; txt[i] != '\0'; i++)
	{
		WriteChar(txt[i]);									// es como leer cada letra
	}
}