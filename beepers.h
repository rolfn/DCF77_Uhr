
#ifndef BEEPERS_H
#define BEEPERS_H

#include <Arduino.h>

#define ON HIGH
#define OFF LOW

extern unsigned long ticks;

extern void setupBeepers(uint8_t pin, unsigned long s_on, unsigned long a_on,
  unsigned long a_off);
extern void setBuzzer(uint8_t x);
extern void alternateBeep(uint8_t sw);
extern void singleBeep(void);
extern void updateBeepers(void);

#endif
