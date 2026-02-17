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
#include "HX711/XH711.h"
//Variables globales
#define TEMP_HIGH 24
#define TEMP_LOW 21
#define SlaveAdress 0x50
uint8_t buffer;
volatile uint8_t adc_value;
//--------------------Variables sensor de temperatura------------------------//
uint8_t temperatura;
uint8_t bandera_nuestra;
uint8_t dato_temperatura;
//-------------------Variables del stepper-----------------------------------//
uint8_t state = 0;
uint8_t last_state;
const uint8_t secuencia[8] =
{
	(1 << PORTD2),
	(1 << PORTD2) | (1 << PORTD3),
	(1 << PORTD3),
	(1 << PORTD3) | (1 << PORTD4),
	(1 << PORTD4),
	(1 << PORTD4) | (1 << PORTD5),
	(1 << PORTD5),
	(1 << PORTD5) | (1 << PORTD2)
};
const uint8_t r_secuencia[8] =
{
	(1 << PORTD5),
	(1 << PORTD5) | (1 << PORTD4),
	(1 << PORTD4),
	(1 << PORTD4) | (1 << PORTD3),
	(1 << PORTD3),
	(1 << PORTD3) | (1 << PORTD2),
	(1 << PORTD2),
	(1 << PORTD2) | (1 << PORTD5)
};

//----------------Variables sensor de peso-----------------------------------//
uint8_t byte_h, byte_m, byte_l =0;
int32_t gram;
uint8_t indice = 0;

HX711 calibracion = {0, 500.0f};


//Function prototypes
void init_stepper(void);
void init_timer1(void);
//---------SENSOR DE PESO----------//



//Main

int main(void)
{
	cli();
	DDRC |=(1 << DDC3);
	DDRC &= ~(1 << DDC1);								//pot
	DDRB |= (1 << DDB5);
	PORTC &= ~(1 << PORTC3);								//LED toggle
	init_HX711();
	init_ADC();
	init_PWM1(19999);
	I2C_SLAVE_INIT(SlaveAdress);
	init_stepper();
	init_timer1();
	sei(); 
	int32_t boton_tare = lectura_promedio(10);
	tara_bascula(&calibracion, boton_tare);
    while (1) 
    {		
		if (buffer == 'T')
		{
			PORTC |= (1 << PORTC3);
			buffer = 0;
			if (gram >= 150)
			{
				pulso_PWM1(140);
			}
			else if (gram <= 300)
			{
				pulso_PWM1(0);
			}
		}
		
		//------------------TEMPERATURA-----------------------//
		last_state = state;
		if (temperatura>=TEMP_HIGH)
		{
			state = 1;
		}
		else if (temperatura<=TEMP_LOW)
		{
			state = 2;
		}
		else
		{
			state = 0;
		}
		if (state == 1)			//cerrar techo
		{
			if (!(last_state == state))
			{
				for (uint8_t j = 0; j <= 8; j++)
				{
					for (uint8_t i = 0; i < 8;i++)
					{
						PORTD |= secuencia[i];
						_delay_us(500);
					}
				}
			}
				PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5));
			}
		else if (state == 2)
		{
			if (!(last_state == state))
			{
				for (uint8_t j = 0; j <= 8; j++)
				{
					for (uint8_t i = 0; i < 8;i++)
					{
						PORTD |= r_secuencia[i];
						_delay_us(500);
					}
				}
				PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5));
			}
		}
		else
		{
			PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5));
		}
		//-----------------PESO----------------------------------------------//
		int32_t entero = lectura_promedio(8);
		float peso = peso_real(&calibracion, entero);
		
		if (peso < 0)
		{
			peso = 0;
		}
		gram = (int32_t) peso;
		cli();
		byte_h = (gram >> 16) & 0xFF;
		byte_m = (gram >> 8) & 0xFF;
		byte_l = gram & 0xFF;
		sei();
    }
}

//NON-Interrupt subroutines
void init_stepper(void)
{
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5);
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5));
}

void init_timer1(void)
{
	TCCR1A = 0x00;
	//PRESCALER 64
	TCNT1 = 65286;
	TIMSK1 |= (1 << TOIE1);
}

//Interrupt routines

ISR(TIMER1_OVF_vect)
{
	TCNT1 = 65286;
}

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
			dato_temperatura = TWDR;
			if (bandera_nuestra)
			{
				temperatura = dato_temperatura;
				bandera_nuestra = 0;
			}else{
				buffer = TWDR;
				if (buffer == 'K')
				{
					bandera_nuestra = 1;
					buffer = 0;
				}
			}
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		// Enviar datos desde slave
		case 0xA8: //SLA+R
		case 0xB8: //Dato enviado, ACK --> Slave
			//PORTC |= (1 << PORTC3);
			if (indice == 0)
			{
				indice = 1;
				byte_h = TWDR;
			}
			if (indice == 1)
			{
				indice = 2;
				byte_m = TWDR;
			}
			if (indice == 2)
			{
				indice = 0;
				byte_l = TWDR;
			}
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
