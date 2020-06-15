#include <Arduino.h>
#include <stdint.h>
#include "RTClib.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#define ESCRITA 1
#define LEITURA 0

#define ABERTA  0
#define FECHADA 1

#define PORTAENTRADA 20
#define TRAVAENTRADA 9
#define BUZZER 13
#define LUZSALA 8
#define LUZQUARTO 7


#define DS18B20 10 

OneWire oneWire(DS18B20);             //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&oneWire);  //BIBLIOTECA DallasTemperature UTILIZA A OneWire
extern DeviceAddress sensor;


extern RTC_DS1307 rtc; 

void portaentrada    (String msg);
void saladeestar     (String msg);
void quartoebanheiro (String msg);

void iniciaRtc();
void checkDoor();
void checkBuzzer();
void ligaBuzzer();
void desligaBuzzer();