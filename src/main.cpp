#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "RTClib.h"
#include "comodos.h"
#include "messagehandler.h"


void setup()
{
  // Starta a serial
  msgInit();

  // configura saídas
  pinMode(OUT1,OUTPUT);
  pinMode(OUT2,OUTPUT);
  pinMode(OUT3,OUTPUT);
  pinMode(OUT4,OUTPUT);

  // configura entradas
  pinMode(IN1,INPUT);
  pinMode(IN2,INPUT);
  pinMode(IN3,INPUT);
  pinMode(IN4,INPUT);

  // inicia com todas desligadas
  digitalWrite(OUT1, OFF);
  digitalWrite(OUT2, OFF);
  digitalWrite(OUT3, OFF);
  digitalWrite(OUT4, OFF);

  //teste
  pinMode(6,OUTPUT);
  analogWrite(6,255);

  if (! rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    Serial.println("DS1307 rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
    rtc.adjust(DateTime(2020, 6, 10, 14, 00, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  }
  delay(100);
}

void loop()
{
  checkBuzzer();
  msgHandler();
}

