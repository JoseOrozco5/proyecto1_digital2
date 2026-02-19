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
#include <util/delay.h>
#include "ADC/ADC.h"
#include "I2C/I2C.h"
#include "PWM1/pwm1.h"
//////////////////////////// Variables /////////////////////////////
#define SlaveAdress 0x30
volatile uint8_t buffer;
volatile uint8_t adc_value;
uint8_t porcentaje_humedad;
uint8_t dato_recibido;
uint8_t bandera_DC = 0;
uint8_t cmd_actual;
uint8_t esperando_dato;

//Function prototypes
void setup(void);
//Main

int main(void)
{
	//CONFIGURAR sensor como entrada
	cli();
	DDRC &= ~(1 << DDC1);						// pot
	DDRB |= (1 << DDB5);						// toggle
	DDRD |= (1 << DDD3);
	PORTD &= ~(1 << PORTD3);					//led para mostrar que se recibio un dato
	DDRC |= (1 << DDC3);
	PORTC &= ~(1 << PORTC3);					//debuggeo manual del case
	setup();
	init_ADC();
	init_PWM1(19999);
	I2C_SLAVE_INIT(SlaveAdress);
	sei();
    while (1) 
    {
		if (bandera_DC == 2)
		{
			PORTB &= ~(1 << PORTB4);
		}
		else if (bandera_DC == 1)
		{
			PORTB |= (1 << PORTB4);
		}
		else
		{
			if (porcentaje_humedad < 10)
			{
				PORTB |= (1 << PORTB4);
			}
			else
			{
				PORTB &= ~(1 << PORTB4);
			}
		}
		
		
		//Secuencia ADC
		ADCSRA |= (1 << ADSC);
    }
}

//NON-Interrupt subroutines
void setup(void)
{
	DDRB |= (1 << DDB4);
	PORTC &= ~(1 << PORTC4);													//MOTOR DC EN PB4
	
}
//Interrupt routines


ISR(ADC_vect)
{
	adc_value =  ADCH;
	porcentaje_humedad = 100 - ((adc_value * 100) / 255);
	ADCSRA |= (1 << ADIF);
}

ISR(TWI_vect)
{
	PORTB ^= (1 << PORTB5);
	uint8_t state = (TWSR & 0xF8); //Extraer 5 bits del estado
	switch (state)
	{
		// --> Slave
		case 0x60:
		case 0x70:
			esperando_dato = 0;
			cmd_actual = 0;
			TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		case 0x80:
		case 0x90:
			buffer = TWDR;
			if (!esperando_dato)
			{
				cmd_actual = buffer;
				esperando_dato = 1;
			}
			else
			{
				if (cmd_actual == 'C')
				{
					bandera_DC = buffer;
				}
				esperando_dato = 0;
				cmd_actual = 0;
			}
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		// Enviar datos desde slave
		case 0xA8: //SLA+R
			TWDR = porcentaje_humedad;														//Enviar valor del sensor
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		case 0xB8: //Dato enviado, ACK --> Slave
			PORTC |= (1 << PORTC3);
			TWDR = porcentaje_humedad;														//Enviar valor del sensor
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		case 0xC0: //Dato transmitido, ACK --> slave
		case 0xC8: //Ultimo dato transmitido
			TWCR = 0;
			TWCR =  (1 << TWEN) | (1 << TWIE) | (1 << TWEA);  //(1 << TWINT);
			break;
		case 0xA0: //STOP | R_START recibido
			esperando_dato = 0;
			cmd_actual = 0;
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		//Errores
		default:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
	}
}