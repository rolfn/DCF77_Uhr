
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define ON HIGH
#define OFF LOW

extern unsigned long ticks;

extern void setupButton(uint8_t pin);
extern void setupButtonCallbackShort(unsigned long period,
  void whenReleased(unsigned long ms), 
  void whilePressed());
extern void setupButtonCallbackMedium(unsigned long period,
  void whenReleased(unsigned long ms), 
  void whilePressed());
extern void setupButtonCallbackLong(unsigned long period,
  void whenReleased(unsigned long ms), 
  void whilePressed());
extern void buttonHandling(const unsigned long t);
  
#endif
