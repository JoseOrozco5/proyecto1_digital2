/*
 * XH711.h
 *
 * Created: 17/02/2026 11:46:03
 *  Author: jose_
 */ 


#ifndef XH711_H_
#define XH711_H_
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdint.h>

typedef struct{
	int32_t offset;
	float scale;
}HX711;

void init_HX711(void);
uint8_t sensor_listo(void);
int32_t leer_24bits(void);
int32_t lectura_promedio(uint8_t promedio);
void tara_bascula(HX711 *cal, int32_t value);
void factor_conversion(HX711 *cal, float equivalencia);
float peso_real(const HX711 *cal, int32_t lectura);

int32_t mandar_24bit(uint8_t b2, uint8_t b1, uint8_t b0);



#endif /* XH711_H_ */