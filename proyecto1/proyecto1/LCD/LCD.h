/*
 * LCD.h
 *
 * Created: 27/01/2026 13:40:03
 *  Author: jose_
 */ 


#ifndef LCD_H_
#define LCD_H_

//Librerias
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

//Function prototypes
void init_LCD8bits(void);											// inicializar LCD
void LCD_Port(uint8_t p);											// se le va a mandar un byte a la función y ese byte se va a colocar en el puerto de datos
void LCD_CMD(char r);												//funcion para enviar un comando (rs en 0)
void LCD_Write_Char(char c);										// función para enviar un caracter
void LCD_Write_String(char *s);										// enviar una cadena
void LCD_Shift_Right(void);											// desplazamiento a la derecha
//void LCD_Shift_Left(void);											// desplazamiento a la izquierda
void LCD_Set_Cursor(char col, char fil);								//establecer el cursos


#endif /* LCD_H_ */