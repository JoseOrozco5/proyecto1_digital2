/*
 * pwm1.h
 *
 * Created: 17/05/2025 02:06:38
 *  Author: jose_
 */ 

#include <avr/io.h>
#ifndef PWM1_H_
#define PWM1_H_

// Setear funciones
void init_PWM1(uint16_t TOP);
void pulso_PWM1(uint16_t pulso);
void pulso2_PWM1(uint16_t angle);



#endif /* PWM1_H_ */