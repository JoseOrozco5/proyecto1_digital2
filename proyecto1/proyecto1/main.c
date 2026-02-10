/*
 * proyecto1.c
 *
 * Created: 10/02/2026 07:53:20
 * Author : jose_
 */ 
//Librerias
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

//Librerias hechas
#include "SPI/SPI.h"
#include "UART/UART.h"
#include "LCD/LCD.h"


//Variables globales
char signal;



//Function prototypes

//Main
int main(void)
{
    init_LCD8bits();
	//spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	init_UART();
	
	
	LCD_Set_Cursor(2,1);
	LCD_Write_String("S1:");
	LCD_Set_Cursor(7,1);
	LCD_Write_String("S2:");
	LCD_Set_Cursor(13,1);
	LCD_Write_String("S3:");
	LCD_Set_Cursor(1,2);
	LCD_Write_String("ADC");
	LCD_Set_Cursor(6,2);
	LCD_Write_String("CONT");
	LCD_Set_Cursor(12,2);
	LCD_Write_String("TEMP");
	sei();
    while (1) 
    {
    }
}

//NON-Interrupt subroutines

//Interrupt routines

ISR(USART_RX_vect)
{
	signal = UDR0;
}

