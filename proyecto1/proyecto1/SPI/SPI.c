/*
 * SPI.c
 *
 * Created: 2/02/2026 22:31:26
 *  Author: jose_
 */ 
//Librerias
#include "SPI.h"

//NON-Interrupt subroutines

void spiInit(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase)
{
	//Cableado en fisico es PB2 es SS, PB3 es MOSI, PB4 es MISO, PB5 es SCK
	
	if (sType & (1 << MSTR))			// Si estamos en modo maestro
	{
		DDRB |= (1 << PORTB3) | (1 << PORTB5) | (1 << PORTB2);			//MOSI, SCK, NEGADO_SS
		DDRB &= ~(1 << PORTB4);											//MISO entrada
		SPCR |= (1 << MSTR);
		
		uint8_t temp = sType & 0b00000111;
		switch(temp)
		{
			case 0:														//DIV2
				SPCR &= ~((1 << SPR1) | (1 << SPR0));
				SPSR |= (1 << SPI2X);
				break;
			case 1:														//DIV4
				SPCR &= ~((1 << SPR1) | (1 << SPR0));
				SPSR &= ~(1 << SPI2X);
				break;
			case 2:														//DIV8
				SPCR |= (1 << SPR0);
				SPCR &= ~(1 << SPR1);
				SPSR |= (1 << SPI2X);
				break;
			case 3:														//DIV16
				SPCR |= (1 << SPR0);
				SPCR &= ~(1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
			case 4:														//DIV32
				SPCR &= ~(1 << SPR0);
				SPCR |= (1 << SPR1);
				SPSR |= (1 << SPI2X);
				break;
			case 5:														//DIV64
				SPCR &= ~(1 << SPR0);
				SPCR |= (1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
			case 6:														//DIV128
				SPCR |= (1 << SPR0) | (1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
				
		}
	}
	else
	{
		DDRB |=(1 << PORTB4);													//MISO
		DDRB &= ~((1 << PORTB3) | (1 << PORTB5) | (1 << PORTB2));				//MOSI, SCK, SS
		SPCR &= ~(1 << MSTR);													//modo esclavo
	}
	
	SPCR |= (1 << SPE) | sDataOrder | sClockPolarity | sClockPhase;				//para ambas opciones se habilita el SPE y los otros parametros
}


void spiWrite(uint8_t dato){
	SPDR = dato;																// cargar dato deseado en el registro de datos
}


unsigned spiDataReady()															//Lee la bandera de la interrupcion y verifica si el dato esta listo para ser leido
{															
	if (SPSR & (1 << SPIF))
	{
		return 1;
	}else
	{
		return 0;
	}
}


uint8_t spiRead(void)
{
	while (!(SPSR & (1 << SPIF)));												//Esperar que la bandera de la interrupcion se encienda
	return(SPDR);																//Leer el dato recibido (registro)
}