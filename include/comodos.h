#include "RTClib.h"

#define ESCRITA 1
#define LEITURA 0

#define ABERTA  0
#define FECHADA 1

#define PORTAENTRADA 20
#define BUZZER 21

extern RTC_DS1307 rtc; 


int initBuzzer[3];
int controlBuzzer[4] = {2,0,0,0};
int doorStatus;
int doorTimer[4] = {0,0,0,0};
int closeTimeout[3] = {0,1,30};

void portaentrada    (String msg);
void saladeestar     (String msg);
void quartoebanheiro (String msg);

void iniciaRtc();
void checkDoor();
void checkBuzzer();
void ligaBuzzer();
void desligaBuzzer();