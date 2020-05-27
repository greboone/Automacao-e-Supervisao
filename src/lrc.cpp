#include <stdint.h>
#include "lrc.h"

uint8_t lrc(uint8_t *dado, uint8_t t)
{
    uint8_t val_lrc = 0;
    for(int i = 0; i < t; i++)
    {
        if((val_lrc + dado[i]) & 0xFF)
            val_lrc = val_lrc + dado[i];
    }
    val_lrc = ((val_lrc^0xFF) + 1) & 0xFF;

    return val_lrc;
}