/*
 * I2C.c
 *
 * Created: 2/10/2026 10:15:16 AM
 *  Author: Diego Alejandro Ramos Rodas
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "I2C.h"
//Iniciar dispositivo MASTER
void I2C_MASTER_INIT(unsigned long sc_clock, uint8_t preescaler)
{
	DDRC &= ~((1 << DDC4) | (1 << DDC5)); //SDA ySCL
	//Seleccionar preescaler
	switch (preescaler)
	{
		case 0:
			TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
			break;
		case 4:
			TWSR &= ~(1 << TWPS1);
			TWSR |= (1 << TWPS0);
			break;
		case 16:
			TWSR &= ~(1 << TWPS0);
			TWSR |= (1 << TWPS1);
			break;
		case 64:
			TWSR |= (1 << TWPS0) | (1 << TWPS1);
			break;
		default:
			TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
			preescaler = 1;
			break;
	}
	TWBR = 18; //Bit rate (calculado aparte)
	TWCR |= (1 << TWEN);
}
//Protocolo de arranque
uint8_t I2C_MASTER_START(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); //Esperar bandera de interrupcion
	return ((TWSR & 0xF8) == 0x08); //Start condition
}
//Protocolo de arranque repetitivo
uint8_t I2C_MASTER_R_START(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); //Esperar bandera de interrupcion
	return ((TWSR & 0xF8) == 0x10); //Start condition
}
//Protocolo para detener lectura
void I2C_MASTER_STOP(void)
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO); //iniciar secuencia stop
	while (TWCR & (1 << TWSTO));						//esperar que el bit se limpie
}
//Funcion para escribir M --> S
uint8_t I2C_MASTER_WRITE(uint8_t data)
{
	uint8_t state;
	TWDR = data; //Cargar data
	TWCR = (1 << TWEN) | (1 << TWINT); //Secuencia de envio
	while(!(TWCR & (1 << TWINT))); //interrupt flag
	state = TWSR & 0xF8; //Extraer registros de estado
	// SLA + W + ACK | Data + ACK
	if ((state == 0x18) || (state == 0x28) || (state == 0x40)) return 1;
	else return state;
}
//Protocolo para leer S --> M
uint8_t I2C_MASTER_READ(uint8_t *buffer, uint8_t ack)
{
	uint8_t state;
	if (ack) //recibir mas datos
	{
		TWCR = (1 << TWINT) | ( 1 << TWEN) | (1 << TWEA);
	} 
	else // ~ack: ultimo byte
	{
		TWCR = (1 << TWINT) | (1<< TWEN);
	}
	
	while(!(TWCR & (1 << TWINT))); //int flag
	state = TWSR & 0xF8;
	if (ack && state != 0x50) return 0; //Estado ACK
	if (!ack && state != 0x58) return 0; //Estado NACK
	*buffer = TWDR;
	return 1;
}
//Inicializar dispositivo slave
void I2C_SLAVE_INIT(uint8_t address)
{
	DDRC &= ~((1 << DDC4) | (1 << DDC5)); //Entradas de inicio
	TWAR = address << 1; //Activar direccion
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

