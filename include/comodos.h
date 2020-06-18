#include <Arduino.h>
#include <stdint.h>
#include "RTClib.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#define ESCRITA '1'
#define LEITURA '0'

#define ABERTA  0
#define FECHADA 1

#define LUZQUARTO 7
#define LUZSALA 8
#define ACSALA 9
#define DS18B20 10 
#define BUZZER 13
#define PORTAENTRADA 22
#define TRAVAENTRADA 23


extern RTC_DS1307 rtc; 

void portaentrada    (String msg);
void saladeestar     (String msg);
void quartoebanheiro (String msg);

void iniciaRtc();
int checkDoor(int call);
int temptoBytes(int temp);
void checkBuzzer();
void ligaBuzzer();
void desligaBuzzer();

void temperaturaSala();