// Adafruit IO Publish & Subscribe Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

// ------------ UART ------------------ //
#include <HardwareSerial.h>
#define RXD2 16
#define TXD2 17

// -------------- Variables ---------------//
int count = 0;
uint8_t peso = 0;
uint8_t temperatura = 0;
uint8_t humedad = 0;
char buffer[20];
int id_buffer = 0;
int v_temperatura = 0;
int v_humedad = 0;
int32_t v_peso = 0;
// Track time of last published messages and limit feed->save events to once
// every IO_LOOP_DELAY milliseconds.
//
// Because this sketch is publishing AND subscribing, we can't use a long
// delay() function call in the main loop since that would prevent io.run()
// from being called often enough to receive all incoming messages.
//
// Instead, we can use the millis() function to get the current time in
// milliseconds and avoid publishing until IO_LOOP_DELAY milliseconds have
// passed.
#define IO_LOOP_DELAY 8000
unsigned long lastUpdate = 0;
int modo = 0; //automatico <-- 1, manual <-- 0
// set up the 'counter' feed
// AdafruitIO_Feed *counter = io.feed("counter");
//Feed de datos para enviar a Adafruit
AdafruitIO_Feed *humedad_feed = io.feed("S_humedad");
AdafruitIO_Feed *peso_feed = io.feed("S_peso");
AdafruitIO_Feed *temperatura_feed = io.feed("S_temperatura");
//Feed de datos para enviar al nano
AdafruitIO_Feed *modo_feed = io.feed("modo");
AdafruitIO_Feed *DC_feed = io.feed("M_DC");
AdafruitIO_Feed *servo_feed = io.feed("M_servo");
AdafruitIO_Feed *stepper_feed = io.feed("M_stepper");
void setup() {

  // start the serial connection
  Serial.begin(115200);
  delay(1000);
  Serial.println("SETUP OK");
  delay(500);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("UART2 OK");

  Serial.print("Connecting to Adafruit IO");
  //conectar a adafruit
  io.connect();
  Serial.println();
  Serial.println(io.statusText());
  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  //counter->onMessage(handleMessage);
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  //Enviar feed de motores
  modo_feed->onMessage(handleModo);
  DC_feed->onMessage(handleDC);
  servo_feed->onMessage(handleSERVO);
  stepper_feed->onMessage(handleSTEPPER);
  //counter->get();

}

void loop() {

  io.run(); //Si se desconecta de adafruit se vuelve a conectar
  //UART
  while (Serial2.available())
  {
    
    char dato = Serial2.read();
    if (dato == '\n') {
      buffer[id_buffer] = '\0';

      if (id_buffer > 1) {
        char tipo = buffer[0];
        long valor = strtol(&buffer[1], NULL, 10);
        Serial.print("Recibido tipo: ");
        Serial.print(tipo);
        Serial.print(" valor: ");
        Serial.println(valor);
        //Recibe el primer bit para ver que dato se ha enviado
        switch (tipo) {
          case 'T': v_temperatura = valor; break;
          case 'P': v_peso = valor; break;
          case 'H': v_humedad = valor; break;
        }
      }

      id_buffer = 0;  // reiniciar posicion del "puntero" del buffer
    }
    else 
    {
      if (id_buffer < sizeof(buffer) - 1) 
      {
        buffer[id_buffer++] = dato;
      }
    } 
  }
  if (millis() > (lastUpdate + IO_LOOP_DELAY)) 
  {

    
    // save count to the 'counter' feed on Adafruit IO
    //enviamos valores de sensores a adafruit
    Serial.print("T:");
    temperatura_feed->save(v_temperatura);
    Serial.print("H:");
    humedad_feed->save(v_humedad);
    Serial.print("P:");
    peso_feed->save(v_peso);
    // after publishing, store the current time
    lastUpdate = millis();
  }
}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  Serial.println(data->value());
}
void handleDC(AdafruitIO_Data *data) {

  if (modo == 1) {
    int estado = data->toInt();
    Serial2.print("H");
    Serial2.print(estado);
    Serial2.print("\n");
  }
}

void handleSERVO(AdafruitIO_Data *data) {

  if (modo == 1) {
    int estado = data->toInt();
    Serial2.print("P");
    Serial2.print(estado);
    Serial2.print("\n");
  }
}
void handleSTEPPER(AdafruitIO_Data *data) {

  if (modo == 1) {
    int estado = data->toInt();
    Serial2.print("T");
    Serial2.print(estado);
    Serial2.print("\n");
  }
}
void handleModo(AdafruitIO_Data *data) {

  modo = data->toInt();

  Serial.print("Modo recibido: ");
  Serial.println(modo);

  // Siempre informamos al Nano del modo
  Serial2.print("M");
  Serial2.print(modo);
  Serial2.print("\n");
}
