/*
 * proyecto1.c
 *
 * Created: 10/02/2026 07:53:20
 * Author : jose_
 */ 
////////////////////// Librerias /////////////////////////////
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>																//snprintf
#include "I2C/I2C.h"
#include "UART/UART.h"
#include "LCD/LCD.h"

///////////////////// Variables ///////////////////////////
//Definir direcciones
//#define slave1 = 0x30;
//#define slave2 = 0x50;
//Definir 2 variables por slave para diferentes modos (Read y Write)
#define slave1R (0x30 << 1) | 0x01															//Slave 1 read
#define slave1W (0x30 << 1) & 0xFE															//Slave 1 write
#define slave2R (0x50 << 1) | 0x01															//Slave 2 read
#define slave2W (0x50 << 1) & 0xFE															//Slave 2 write
#define LM75_R (0x48 << 1) | 0x01
#define LM75_W (0x48 << 1) & 0xFE


uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C_1 = 0;
uint8_t bufferI2C_2 = 0;
uint16_t temp_h, temp_l;
uint8_t temperatura;
char signal;
char buffer1[7];
char buffer2[7];
char buffer3[8];

//Function prototypes
void setup(void);
uint8_t LM75_to_uintC(uint8_t dato_high, uint8_t dato_low);
//////////////////////////// Main Loop //////////////////////////////
int main(void)
{
	setup();
	init_LCD8bits();
	init_UART();
	I2C_MASTER_INIT(100000,4);
	LCD_Set_Cursor(1,1);
	LCD_Write_String("HUM:");
	LCD_Set_Cursor(7,1);
	LCD_Write_String("S2:");
	LCD_Set_Cursor(12,1);
	LCD_Write_String("TEMP:");
	DDRC |= (1 << DDC3);
	PORTC &= ~(1 << PORTC3);
	sei();
	while (1)
	{
		//-----------------------------SLAVE 1------------------------------------------------//
		if (!I2C_MASTER_START()) return;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave1W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			return;
		}
		//Comando para leer datos de slave
		I2C_MASTER_WRITE('C');
		if (!I2C_MASTER_R_START())											//Empezar a leer
		{
			I2C_MASTER_STOP();
			return;
		}
		if (!I2C_MASTER_WRITE(slave1R))										//Si no esta escribiendo, cortar comunicacion
		{
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_READ(&bufferI2C_1,0);									//NACK
		//Imprimir datos en la LCD
		LCD_Set_Cursor(1,2);
		snprintf(buffer1, sizeof(buffer1), "%3u%% ", bufferI2C_1);
		LCD_Write_String(buffer1);
		//if de motor dc aqui
		if (bufferI2C_1 < 10)
		{
			PORTB |= (1 << PORTB4);
		}
		else
		{
			PORTB &= ~(1 << PORTB4);
		}
		I2C_MASTER_STOP();
		//----------------------------SLAVE 2----------------------------//
		if (!I2C_MASTER_START()) return;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave2W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			return;
		}
		//Comando para leer datos de slave
		I2C_MASTER_WRITE('T');
		if (!I2C_MASTER_R_START())											//Empezar a leer
		{
			I2C_MASTER_STOP();
			return;
		}
		if (!I2C_MASTER_WRITE(slave2R))										//Si no esta escribiendo, cortar comunicacion
		{
			I2C_MASTER_STOP();
			return;
		}
		//LED (si quiero)
		I2C_MASTER_READ(&bufferI2C_2,0);									//NACK
		//Imprimir datos en la LCD
		LCD_Set_Cursor(7,2);
		snprintf(buffer2, sizeof(buffer2), "%3u ", bufferI2C_2);
		LCD_Write_String(buffer2);
		I2C_MASTER_STOP();
		//------------------------Sensor I2C----------------------------//
  		if (!I2C_MASTER_START()) return;									//No avanzar hasta realizar correctamente el start
  		if (!I2C_MASTER_WRITE(LM75_W))										//Esperar a que slave responda si esta escuchando
  		{
  			I2C_MASTER_STOP();
  			return;
  		}
  		//Comando para leer datos de slave
  		I2C_MASTER_WRITE(0x00);
  		I2C_MASTER_STOP();
  		if (!I2C_MASTER_START()) return;
  		if (!I2C_MASTER_WRITE(LM75_R))										//Si no esta escribiendo, cortar comunicacion
  		{
  			I2C_MASTER_STOP();
  			return;
  		}
 		  
   		//Leer dos bytes del LM75
   		I2C_MASTER_READ(&temp_h,1);
		I2C_MASTER_READ(&temp_l,0);
		
		PORTC |= (1 << PORTC3);  
		I2C_MASTER_STOP();
  		temperatura = LM75_to_uintC(temp_h,temp_l);
  		//Mostrar temperatura en lcd
  		LCD_Set_Cursor(13,2);
  		snprintf(buffer3, sizeof(buffer3), "%3u ", temperatura);
  		LCD_Write_String(buffer3);
 
	}
}
////////////////////// Non-interrupt function ///////////////////////////
void setup(void)
{
	DDRB |= (1 << DDB4);
	PORTC &= ~(1 << PORTC4);													//MOTOR DC EN PB4
	
}
uint8_t LM75_to_uintC(uint8_t dato_high, uint8_t dato_low)
{
	int16_t raw = ((int16_t)dato_high << 8) | dato_low;
	int16_t half = raw >> 7;													// unidades de 0.5°C 
	int16_t tempC = half / 2;													// °C enteros (signed)
	if (tempC < 0) tempC = 0;													// usar solo datos positivos
	if (tempC > 255) tempC = 255;    
	return (uint8_t)tempC;
}
/////////////////////////// Interrupt function /////////////////////////
ISR(USART_RX_vect)
{
	signal = UDR0;
}
