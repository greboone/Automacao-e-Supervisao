#include <Wire.h>
#include "RTClib.h"

#define ESCRITA 1
#define LEITURA 0


#define PORTAENTRADA 20
#define BUZZER 21

RTC_DS1307 rtc; 


int initBuzzer[3];


void portaentrada    (String msg);
void saladeestar     (String msg);
void quartoebanheiro (String msg);

void checkBuzzer();
void ligaBuzzer();
void desligaBuzzer();