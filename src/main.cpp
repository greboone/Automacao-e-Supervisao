#include <Arduino.h>
#include <stdint.h>
#include "Wire.h"
#include "messagehandler.h"


void setup()
{
  // Starta a serial
  msgInit();
  
  delay(100);
}

void loop()
{
  schedule();
  msgHandler();
}

