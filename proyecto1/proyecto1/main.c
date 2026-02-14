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
#define slave1R (0x30 << 1) | 0x01
#define slave1W (0x30 << 1) & 0xFE
#define slave2R (0x50 << 1) | 0x01
#define slave2W (0x50 << 1) & 0xFE

uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C_1 = 0;
uint8_t bufferI2C_2 = 0;
char signal;
char buffer1[7];
char buffer2[7];

//////////////////////////// Main Loop //////////////////////////////
int main(void)
{
	init_LCD8bits();
	init_UART();
	I2C_MASTER_INIT(100000,4);
	LCD_Set_Cursor(2,1);
	LCD_Write_String("S1:");
	LCD_Set_Cursor(7,1);
	LCD_Write_String("S2:");
	LCD_Set_Cursor(13,1);
	LCD_Write_String("S3:");
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
		PORTC |= (1 << PORTC3);
		I2C_MASTER_READ(&bufferI2C_1,0);									//NACK
		//Imprimir datos en la LCD
		LCD_Set_Cursor(1,2);
		snprintf(buffer1, sizeof(buffer1), "%3u ", bufferI2C_1);
		LCD_Write_String(buffer1);
		I2C_MASTER_STOP();
		_delay_ms(1000);
		/*
		//----------------------------SLAVE 2----------------------------//
		if (!I2C_MASTER_START()) return;									//No avanzar hasta realizar correctamente el start
		if (!I2C_MASTER_WRITE(slave2W))										//Esperar a que slave responda si esta escuchando
		{
			I2C_MASTER_STOP();
			return;
		}
		//Comando para leer datos de slave
		I2C_MASTER_WRITE('D');
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
		_delay_ms(300);
		*/
	}
}
////////////////////// Non-interrupt function ///////////////////////////

/////////////////////////// Interrupt function /////////////////////////
ISR(USART_RX_vect)
{
	signal = UDR0;
}
