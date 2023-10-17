
#ifndef BEEPERS_H
#define BEEPERS_H

#include <Arduino.h>

#define ON HIGH
#define OFF LOW

extern void setupBeepers(const uint8_t pin, const unsigned long s_on,
  const unsigned long a_on, const unsigned long a_off);
extern void setBuzzer(const uint8_t x);
extern void alternatingBeep(const uint8_t sw);
extern void singleBeep(void);
extern void beepersHandling(const unsigned long t);

#endif
