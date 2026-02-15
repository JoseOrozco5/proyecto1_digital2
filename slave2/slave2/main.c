/*
 * slave2.c
 *
 * Created: 10/02/2026 08:59:03
 * Author : jose_
 */ 
//Librerias
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Librerias hechas
#include "ADC/ADC.h"
#include "PWM1/pwm1.h"
#include "I2C/I2C.h"
//Variables globales
#define SlaveAdress 0x50
uint8_t buffer;
volatile uint8_t adc_value;

//Function prototypes

//Main

int main(void)
{
	cli();
	DDRC |=(1 << DDC3);
	DDRC &= ~(1 << DDC1);								//pot
	DDRB |= (1 << DDB5);
	PORTC &= ~(1 << PORTC3);								//LED toggle
	init_ADC();
	init_PWM1(19999);
	I2C_SLAVE_INIT(SlaveAdress);
	sei();
    while (1) 
    {
		if (buffer == 'T')
		{
			PORTC |= (1 << PORTC3);
			buffer = 0;
		}
		//Secuencia ADC
		ADCSRA |= (1 << ADSC);
    }
}

//NON-Interrupt subroutines

//Interrupt routines


ISR(ADC_vect)
{
	adc_value =  ADCH;
	ADCSRA |= (1 << ADIF);
}

ISR(TWI_vect)
{
	PORTB ^= (1 << PORTB5);
	uint8_t state = TWSR & 0xF8;						//Extraer 5 bits del estado
	switch (state)
	{
		// ---> Slave
		case 0x60:
		case 0x70:
			TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		case 0x80:
		case 0x90:
			buffer = TWDR;
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		// Enviar datos desde slave
		case 0xA8: //SLA+R
		case 0xB8: //Dato enviado, ACK --> Slave
			//PORTC |= (1 << PORTC3);
			TWDR = adc_value;														//Enviar valor del sensor
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		case 0xC0: //Dato transmitido, ACK --> slave
		case 0xC8: //Ultimo dato transmitido
			TWCR = 0;
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA); //(1 << TWINT)
			break;
		case 0xA0: //STOP | R_START recibido
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		//Errores
		default:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
	}
}
