/*
 * ADC.c
 *
 * Created: 10/02/2026 08:02:40
 *  Author: jose_
 */ 
//Librerias
#include "ADC.h"

//NON-Interrupt subroutines

void init_ADC()
{
	ADMUX = 0;																		// LIMPIAR REGISTRO
	ADMUX |= (1 << REFS0);															// utilizar AVcc como referencia (5v)
	ADMUX |= (1 << ADLAR);															// justificado a la izquierda (y leo 8 bits mas significativos)
	ADMUX |= (1 << MUX0);															// utilizar PC1 para potenciometro
	
	ADCSRA = 0;																		// LIMPIAR REGISTRO
	ADCSRA |= (1 << ADEN);															// Activar ADC
	ADCSRA |= (1 << ADIE);															// Activar interrupcion del ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);							// prescaler 128
	//ADCSRA |= (1 << ADSC);															// iniciar primera conversión
}
