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
// Definir direcciones
// #define slave1 = 0x30;
// #define slave2 = 0x50;
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
int32_t pesaje;
uint8_t peso_1, peso_2, peso_3;
char signal;
char buffer1[7];
char buffer2[10];
char buffer3[8];

//Function prototypes
void setup(void);
uint8_t LM75_to_uintC(uint8_t dato_high, uint8_t dato_low);
int32_t mandar_24bit(uint8_t b2, uint8_t b1, uint8_t b0);

//////////////////////////// Main Loop //////////////////////////////
int main(void)
{
	init_LCD8bits();
	init_UART();
	I2C_MASTER_INIT(100000,4);
	LCD_Set_Cursor(1,1);
	LCD_Write_String("HUM:");
	LCD_Set_Cursor(6,1);
	LCD_Write_String("PESO:");
	LCD_Set_Cursor(12,1);
	LCD_Write_String("TEMP:");
	DDRC |= (1 << DDC3);
	PORTC &= ~(1 << PORTC3);
	sei();
	while (1)
	{
		//-----------------------------SLAVE 1------------------------------------------------//
		if (!I2C_MASTER_START()) continue;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave1W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			continue;
		}
		//Comando para leer datos de slave
		I2C_MASTER_WRITE('C');
		if (!I2C_MASTER_R_START())											//Empezar a leer
		{
			I2C_MASTER_STOP();
			continue;
		}
		if (!I2C_MASTER_WRITE(slave1R))										//Si no esta escribiendo, cortar comunicacion
		{
			I2C_MASTER_STOP();
			continue;
		}
		I2C_MASTER_READ(&bufferI2C_1,0);									//NACK
		//Imprimir datos en la LCD
		LCD_Set_Cursor(1,2);
		snprintf(buffer1, sizeof(buffer1), "%3u%% ", bufferI2C_1);
		LCD_Write_String(buffer1);
		I2C_MASTER_STOP();
		//----------------------------SLAVE 2----------------------------//
		if (!I2C_MASTER_START()) continue;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave2W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			continue;
		}
		//Comando para leer datos de slave
		I2C_MASTER_WRITE('T');
		if (!I2C_MASTER_R_START())											//Empezar a leer
		{
			I2C_MASTER_STOP();
			continue;
		}
		if (!I2C_MASTER_WRITE(slave2R))										//Si no esta escribiendo, cortar comunicacion
		{
			I2C_MASTER_STOP();
			continue;
		}
  		I2C_MASTER_READ(&peso_1, 1);
  		I2C_MASTER_READ(&peso_2, 1);
  		I2C_MASTER_READ(&peso_3, 0);
 		LCD_Set_Cursor(7,2);
		pesaje = mandar_24bit(peso_1, peso_2, peso_3);
 		snprintf(buffer2, sizeof(buffer2), "%ldg   ", (long)pesaje);
 		LCD_Write_String(buffer2);											//Imprimir datos en la LCD
 		I2C_MASTER_STOP();
// 		LCD_Set_Cursor(7,2);
// 		snprintf(buffer2, sizeof(buffer2), "%3u ", bufferI2C_2);
// 		LCD_Write_String(buffer2);											//Imprimir datos en la LCD
// 		I2C_MASTER_STOP();
		//------------------------Sensor I2C----------------------------//
  		if (!I2C_MASTER_START()) continue;									//No avanzar hasta realizar correctamente el start
  		if (!I2C_MASTER_WRITE(LM75_W))										//Esperar a que slave responda si esta escuchando
  		{
  			I2C_MASTER_STOP();
  			continue;
  		}
  		//Comando para leer datos de slave
  		I2C_MASTER_WRITE(0x00);
  		I2C_MASTER_STOP();
  		if (!I2C_MASTER_START()) continue;
  		if (!I2C_MASTER_WRITE(LM75_R))										//Si no esta escribiendo, cortar comunicacion
  		{
  			I2C_MASTER_STOP();
  			continue;
  		}
 		  
   		//Leer dos bytes del LM75
   		I2C_MASTER_READ(&temp_h,1);
		I2C_MASTER_READ(&temp_l,0);
		
		PORTC |= (1 << PORTC3);  
		I2C_MASTER_STOP();
  		temperatura = LM75_to_uintC(temp_h,temp_l);
  		//Mostrar temperatura en lcd
  		LCD_Set_Cursor(12,2);
  		snprintf(buffer3, sizeof(buffer3), "%3u%cC ", temperatura, 223);
  		LCD_Write_String(buffer3);
		//----------------Mandar temperatura a Slave 2-----------------//
		if (!I2C_MASTER_START()) continue;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave2W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			continue;
		}
		//Comando para mandar datos de temperatura a slave2
		I2C_MASTER_WRITE('K');
		I2C_MASTER_WRITE(temperatura);
		I2C_MASTER_STOP();
	}
}
////////////////////// Non-interrupt function ///////////////////////////

uint8_t LM75_to_uintC(uint8_t dato_high, uint8_t dato_low)
{
	int16_t raw = ((int16_t)dato_high << 8) | dato_low;
	int16_t half = raw >> 7;													// unidades de 0.5°C 
	int16_t tempC = half / 2;													// °C enteros (signed)
	if (tempC < 0) tempC = 0;													// usar solo datos positivos
	if (tempC > 255) tempC = 255;    
	return (uint8_t)tempC;
}

int32_t mandar_24bit(uint8_t b2, uint8_t b1, uint8_t b0){							//reconstruye los 24 bits en 3 bytes para poder enviarlos al master
	uint32_t datos = ((uint32_t)b2 << 16 | ((uint32_t)b1 << 8) | ((uint32_t)b0));
	if (datos & 0x800000UL)
	{
		datos |= 0xFF000000UL;
	}
	return(int32_t)datos;
}
/////////////////////////// Interrupt function /////////////////////////
ISR(USART_RX_vect)
{
	signal = UDR0;
}
