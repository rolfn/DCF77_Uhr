
#include "button.h"

uint8_t buttonPin = 255;

unsigned long shortPeriod = 0;
unsigned long mediumPeriod = 0;
unsigned long longPeriod = 0;

void (*releasedShort)(unsigned long ms) = NULL; 
void (*releasedMedium)(unsigned long ms) = NULL; 
void (*releasedLong)(unsigned long ms) = NULL; 
void (*pressedShort)() = NULL; 
void (*pressedMedium)() = NULL; 
void (*pressedLong)() = NULL; 

void setupButton(uint8_t pin) {
  buttonPin = pin;
}

void setupButtonCallbackShort(unsigned long period,
  void (*whenReleased)(unsigned long ms), 
  void (*whilePressed)()) {
  shortPeriod = period;
  releasedShort = whenReleased;
  pressedShort = whilePressed;
}
void setupButtonCallbackMedium(unsigned long period,
  void (*whenReleased)(unsigned long ms), 
  void (*whilePressed)()) {
  mediumPeriod =   period;
  releasedMedium = whenReleased;
  pressedMedium = whilePressed;
}
void setupButtonCallbackLong(unsigned long period,
  void (*whenReleased)(unsigned long ms), 
  void (*whilePressed)()) {
  longPeriod =  period;
  releasedLong = whenReleased;
  pressedLong = whilePressed;
}

void buttonHandling(const unsigned long t) {
  static unsigned long startMillis = 0, lastMillis = 0;
  static bool buttonActive = false;
  if (t == lastMillis) return;
  lastMillis = t;
  if (buttonActive) {
    if (digitalRead(buttonPin) == LOW) {
      if ((startMillis + shortPeriod) == t) {
        if (pressedShort) (*pressedShort)();
      } else if ((startMillis + mediumPeriod) == t) {
        if (pressedMedium) (*pressedMedium)();
      } else if ((startMillis + longPeriod) == t) {
        if (pressedLong) (*pressedLong)();
      }
    } else {
      if ((startMillis + shortPeriod) <= t && (startMillis + mediumPeriod) > t) {
        if (releasedShort) (*releasedShort)(t - startMillis);
      } else if ((startMillis + mediumPeriod) <= t && (startMillis + longPeriod) > t) {
        if (releasedMedium) (*releasedMedium)(t - startMillis);
      } else if ((startMillis + longPeriod) <= t) {
        if (releasedLong) (*releasedLong)(t - startMillis);
      }
      buttonActive = false;
    }
  } else {
    buttonActive = (bool)!digitalRead(buttonPin);
    if (buttonActive) startMillis = t;
  }

}
