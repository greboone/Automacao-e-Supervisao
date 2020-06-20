#include <Arduino.h>
#include <stdint.h>
#include "lrc.h"
#include "comodos.h"


void msgHandler();
void msgInit();

void schedule();
uint8_t calculalrc(uint8_t *msg, uint8_t t);
/*
void static read_coil       (String msg);
void static read_input      (String msg);
void static write_coil      (String msg);
void static read_analog     (String msg);
void static write_analog    (String msg);

*/