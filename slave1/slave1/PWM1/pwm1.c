/*
 * pwm1.c
 *
 * Created: 17/05/2025 02:06:53
 *  Author: jose_
 */ 
// Librerias
#include "pwm1.h"

// NON-Interrupt subroutines
// El PWM sirve para el servomotor, este genera una señal que hace que se mueva
void init_PWM1(uint16_t TOP){
	DDRB |= (1 << PORTB1) | (1 << PORTB2);										// Salida para servo1 (PB1) Y servo 2 (PB2)
	TCCR1A = 0;																	// LIMPIAR TCCRA
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);						// Polaridad no invertido (OC1A para servo1 y OC1B para servo2) y fast pwm (parte baja)
	TCCR1B = 0;																	// LIMPIAR TCCRB
	TCCR1B = (1 << WGM13) | (1 << WGM12);										// MODO FAST PWM (parte alta)
	TCCR1B |= (1 << CS11);														// PRESCALER PWM 8
	//TCCR1B |= (1 << ICNC1);														// noise canceler
	ICR1 = TOP;																	// valor maximo para ICR1 "19999"
}

// El registro OCR1A revisa cuanto dura el pulso en PB1 (servo) en cada ciclo PWM, el servo lee el pulso y se mueve segun la posicion que le diga
void pulso_PWM1(uint16_t pulso){
	//OCR1A = 1000 + ((uint32_t)pulso * 4000 / 180);
	OCR1A = 20.05 * (float)pulso + 1200.0;												// base
	
}

void pulso2_PWM1(uint16_t angle)
{
	//OCR1B = 1000 + ((uint32_t)angle * 4000 / 180);
	OCR1B = 20.05 * (float)angle + 1200.0;
}
// Interrupt routines
