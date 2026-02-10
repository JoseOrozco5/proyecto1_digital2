/*
 * slave1.c
 *
 * Created: 10/02/2026 08:21:40
 * Author : jose_
 */ 
//Librerias
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

//Librerias hechas
#include "ADC/ADC.h"
#include "SPI/SPI.h"
#include "PWM1/pwm1.h"

//Variables globales
uint8_t data;
uint8_t adc_value;

//Function prototypes

//Main

int main(void)
{
    cli();
	spiInit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_HIGH, SPI_CLOCK_FIRST_EDGE);
	init_ADC();
	init_PWM1(19999);
	SPCR |= (1 << SPIE);
	sei();
    while (1) 
    {
    }
}

//NON-Interrupt subroutines

//Interrupt routines

ISR(SPI_STC_vect)
{
	data = SPDR;
}

ISR(ADC_vect)
{
	adc_value =  ADCH;
}