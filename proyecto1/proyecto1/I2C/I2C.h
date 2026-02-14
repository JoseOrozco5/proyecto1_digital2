/*
 * I2C.h
 *
 * Created: 2/10/2026 10:14:58 AM
 *  Author: Diego Alejandro Ramos Rodas
 */ 


#ifndef I2C_H_
#define I2C_H_

#define F_CPU 16000000

void I2C_MASTER_INIT(unsigned long sc_clock, uint8_t preescaler);

////Inicio de comunicacion I2C////
uint8_t I2C_MASTER_START(void);
uint8_t I2C_MASTER_R_START(void);
////Funcion de paro de comunicacion//// 
void I2C_MASTER_STOP(void);
//// Funcion de transmision de datos M-->S ////
//Devolver 0 si S recibio dato
uint8_t I2C_MASTER_WRITE(uint8_t data);
//// Recepcion de datos S-->M ////
uint8_t I2C_MASTER_READ(uint8_t *buffer, uint8_t ack);
//// Inicializar S ////
void I2C_SLAVE_INIT(uint8_t address);

#endif /* I2C_H_ */