#include <Arduino.h>
#include <stdint.h>
#include <string.h>
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

#define AQUECEDOR 37
#define ENABLESALA 42
#define MOTORSALAHORARIO 43
#define MOTORSALAANTIHORARIO 44
#define ENABLEQUARTO 45
#define MOTORQUARTOHORARIO 46
#define MOTORQUARTOANTIHORARIO 47

#define VENTO A12
#define LM35 A13
#define ESTADOJANELAQUARTO A14
#define ESTADOJANELASALA A15


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

void desligaMotores();
void controlaMotores();

int controlaAquecedor();
int controlaVento();
int travaPorta();
void retorna();