/*
 * LCD.c
 *
 * Created: 27/01/2026 13:40:15
 *  Author: jose_
 */ 
//Librerias
#include "LCD.h"

//Function prototypes
void init_LCD8bits(void){
	
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7);			//PD0-PD5 LCD
	DDRB |=	(1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3);											// PB0 y PB1 son PD6 y PD7, PB2 es E y PB3 es RS
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
	PORTB &= ~((1 << PORTB2) | (1 << PORTB3));
	
	LCD_Port(0x00);
	_delay_ms(20);
	LCD_CMD(0x30);
	_delay_ms(5);
	LCD_CMD(0x30);
	_delay_us(120);
	LCD_CMD(0x30);
	_delay_us(120);
	LCD_CMD(0x38);
	_delay_us(120);
	
	
	//Function set
	LCD_CMD(0x38);
	//Display ON/OFF
	LCD_CMD(0x0C);
	//Entry mode
	LCD_CMD(0x06);
	//Clear display
	LCD_CMD(0x01);
	
}

void LCD_CMD(char r){
	PORTB &= ~(1 << PORTB3);									// PONER RS EN 0
	LCD_Port(r);												//poner comando en puerto
	PORTB |= (1 << PORTB2);										//poner enable en 1
	_delay_ms(5);												// tiempo de espera para borrar el visualizador
	PORTB &= ~(1 << PORTB2);									// poner en 0 enable
}

//mandar byte al puerto D
void LCD_Port(uint8_t p){
	if (p & 1)
	{
		PORTD |= (1 << PORTD2);									//PD0 LCD = 1
	} 
	else
	{
		PORTD &= ~(1 << PORTD2);								//PD0 LCD = 0
	}
	
	if (p & 2)
	{
		PORTD |= (1 << PORTD3);									//PD1 LCD = 1
	} 
	else
	{
		PORTD &= ~(1 << PORTD3);								//PD1 LCD = 0
	}
	
	if (p & 4)
	{
		PORTD |= (1 << PORTD4);									//PD2 LCD = 1
	}
	else
	{
		PORTD &= ~(1 << PORTD4);								//PD2 LCD = 0
	}
	
	if (p & 8)
	{
		PORTD |= (1 << PORTD5);									//PD3 LCD = 1
	}
	else
	{
		PORTD &= ~(1 << PORTD5);								//PD3 LCD = 0
	}
	
	if (p & 16)
	{
		PORTD |= (1 << PORTD6);									//PD4 LCD = 1
	}
	else
	{
		PORTD &= ~(1 << PORTD6);								//PD4 LCD = 0
	}
	
	if (p & 32)
	{
		PORTD |= (1 << PORTD7);									//PD5 LCD = 1
	}
	else
	{
		PORTD &= ~(1 << PORTD7);								//PD5 LCD = 0
	}
	
	if (p & 64)
	{
		PORTB |= (1 << PORTB0);									//PD6 LCD = 1
	}
	else
	{
		PORTB &= ~(1 << PORTB0);								//PD6 LCD = 0
	}
	
	if (p & 128)
	{
		PORTB |= (1 << PORTB1);									//PD7 LCD = 1
	}
	else
	{
		PORTB &= ~(1 << PORTB1);								//PD7 LCD = 0
	}
}

//escribir un caracter
void LCD_Write_Char(char c){
	PORTB |= (1 << PORTB3);										// PONER RS EN 1
	LCD_Port(c);												//colocar byte en puerto de datos
	PORTB |= (1 << PORTB2);										//PONER E EN 1
	_delay_ms(5);
	PORTB &= ~(1 << PORTB2);									//PONER E EN 0
}

void LCD_Write_String(char *s){
	for (uint8_t i = 0; s[i] != '\0'; i++)
	{
		LCD_Write_Char(s[i]);
	}
}

void LCD_Shift_Right(void){
	LCD_CMD(0x01);
}

void LCD_Set_Cursor(char col, char fil){
	char temporal;
	if (fil == 1)
	{
		temporal = 0x80 + col - 1;
		LCD_CMD(temporal);
	} else if (fil == 2)
	{
		temporal = 0xC0 + col -1;
		LCD_CMD(temporal);
	}
}