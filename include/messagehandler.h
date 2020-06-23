#include <Arduino.h>
#include <stdint.h>
#include "lrc.h"
#include "comodos.h"


void msgHandler();
void msgInit();
void setBaud(String msg);
void schedule();
uint8_t calculalrc(uint8_t *msg, uint8_t t);
