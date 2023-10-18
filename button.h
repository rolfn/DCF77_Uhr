
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(unsigned long ms);

extern unsigned long ticks;

extern void setupButton(uint8_t pin);
extern void setupButtonCallbackShort(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void setupButtonCallbackMedium(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void setupButtonCallbackLong(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void buttonHandling(const unsigned long t);
  
#endif
