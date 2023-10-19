
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define THROW_ERROR_IF_NOT_FAST 
#include <digitalWriteFast.h>
// pio lib install -g ~/digitalWriteFast-1.2.0.zip

#define BUTTON_PIN 10      // PB2

#define SHORT_PERIOD    50L // ms
#define MEDIUM_PERIOD 1500L // ms
#define LONG_PERIOD   3500L // ms

typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(unsigned long ms);

extern unsigned long ticks;

//extern void setupButton(uint8_t pin);
extern void setupButtonCallbackShort(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void setupButtonCallbackMedium(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void setupButtonCallbackLong(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed);
extern void buttonHandling(const unsigned long t);
  
#endif
