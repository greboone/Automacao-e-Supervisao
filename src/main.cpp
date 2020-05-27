#include <Arduino.h>
#include <stdint.h>
#include "messagehandler.h"

void setup()
{
  msginit();

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

}

void loop()
{
  msgHandler();
}

