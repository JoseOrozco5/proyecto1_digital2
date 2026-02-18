/*
 * HX711.c
 *
 * Created: 17/02/2026 11:45:47
 *  Author: jose_
 */ 
//Librerias
#include "XH711.h"
#include <util/delay.h>

//NON-Interrupt subroutinnes

void init_HX711(void){
	
	DDRC |=  (1 << DDC1);			//SALIDA SCK en PC1
	DDRC &= ~(1 << DDC2);			//ENTRADA DT EN PC2
	PORTC &= ~(1 << PORTC1);		//SCK APAGADO inicialmente
	
}

uint8_t sensor_listo(void){
	return ((PINC & (1 << PC2)) == 0);	//revisar si DT esta en 0 (osea listo para entregar datos) y devuelve 1 cuando ya tiene un dato
}


int32_t leer_24bits(void){
	uint32_t data = 0;
	while(!sensor_listo());					//leer los 24 bits del sensor con pulsos en sck y hace un pulso extra
	
	for (uint8_t i = 0; i < 24; i++)
	{
		PORTC |= (1 << PORTC1);				//SCK en 1
		_delay_us(1);
		
		data <<= 1;							//DESPLAZAR BITS UNA POSICION A LA IZQUIERDA
		
		if (PINC & (1 << PORTC2))
		{
			data |= 1;						//leer bit actual
		}
		PORTC &= ~(1 << PORTC1);			//BAJAR CLOCK
		_delay_us(1);
	}
	//pulso extra para selecionar A128
	PORTC |= (1 << PORTC1);					//sck
	_delay_us(1);
	PORTC &= ~(1 << PORTC1);
	_delay_us(1);
	
	if (data & 0x800000UL)
	{
		data |= 0xFF000000UL;
	}
	return (int32_t) data;
}

//Lee varias veces el sensor y hace un promedio de la lectura para evitar ruido
int32_t lectura_promedio(uint8_t promedio){	
	if (promedio == 0)
	{
		promedio = 1;
	}
	int64_t sum = 0;
	
	for (uint8_t i = 0; i < promedio; i++)
	{
		sum += leer_24bits();
	}
	return(int32_t)(sum/promedio);
}

int32_t mandar_24bit(uint8_t b2, uint8_t b1, uint8_t b0){							//reconstruye los 24 bits en 3 bytes para poder enviarlos al master
	uint32_t datos = ((uint32_t)b2 << 16 | ((uint32_t)b1 << 8) | ((uint32_t)b0));
	if (datos & 0x800000UL)
	{
		datos |= 0xFF000000UL;
	}
	return(int32_t)datos;
}

void tara_bascula(HX711 *cal, int32_t value){										// guarda value como offset y pone scale en 1 si esta en 0 oara evitar divisiones de 0
	if (!cal)
	{
		return;
	}
	cal->offset = value;
	if (cal->scale == 0.0f)
	{
		cal->scale = 1.0f;
	}
}

void factor_conversion(HX711 *cal, float equivalencia){								//guarda el factor de conversion en scale
	if (!cal)
	{
		return;
	}
	if (equivalencia == 0.0f)
	{
		equivalencia = 1.0f;
	}
	cal->scale = equivalencia;
}

float peso_real(const HX711 *cal, int32_t lectura){									//convierte lectura a unidades con decimales
	if (!cal)
	{
		return 0.0f;
	}
	return (float)(lectura - cal ->offset)/(cal->scale);
}