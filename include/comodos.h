#include <Arduino.h>
#include <stdint.h>
#include "RTClib.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#define ESCRITA '1'
#define LEITURA '0'

#define ABERTA  0
#define FECHADA 1

#define PORTAENTRADA 22
#define TRAVAENTRADA 23
#define BUZZER 42
#define LUZSALA A15 //69 //8
#define LUZSALALEITURA 8
#define LUZQUARTO 7


#define DS18B20 10 




extern RTC_DS1307 rtc; 

void portaentrada    (String msg);
void saladeestar     (String msg);
void quartoebanheiro (String msg);

void iniciaRtc();
void checkDoor(int call);
void checkBuzzer();
void ligaBuzzer();
void desligaBuzzer();

void temperaturaSala();