
#include "button.h"

uint8_t buttonPin = BUTTON_PIN;

unsigned long shortPeriod = 0;  // ms
unsigned long mediumPeriod = 0; // ms
unsigned long longPeriod = 0;   // ms

parameterizedCallbackFunction releasedShort = NULL; 
parameterizedCallbackFunction releasedMedium = NULL; 
parameterizedCallbackFunction releasedLong = NULL;
callbackFunction pressedShort = NULL;
callbackFunction pressedMedium = NULL;
callbackFunction pressedLong = NULL;

void setupButton(uint8_t pin) {
  buttonPin = pin;
}

void setupButtonCallbackShort(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  shortPeriod = period;
  // Function that is called when the button is released after time "shortPeriod".
  releasedShort = whenReleased;
  // Function that is called when "shortPeriod" has passed while the button is pressed.
  pressedShort = whilePressed;
}
void setupButtonCallbackMedium(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  mediumPeriod = period;
  // Function that is called when the button is released after time "mediumPeriod".
  releasedMedium = whenReleased;
  // Function that is called when "mediumPeriod" has passed while the button is pressed.
  pressedMedium = whilePressed;
}
void setupButtonCallbackLong(const unsigned long period,
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  longPeriod = period;
  // Function that is called when the button is released after time "longPeriod".
  releasedLong = whenReleased;
  // Function that is called when "longPeriod" has passed while the button is pressed.
  pressedLong = whilePressed;
}

void buttonHandling(const unsigned long t) {
  static unsigned long startMillis = 0, lastMillis = 0;
  unsigned long dt;
  static bool buttonActive = false;
  if (t == lastMillis) return;
  lastMillis = t;
  if (buttonActive) {
    if (digitalReadFast(buttonPin) == LOW) {
    ///if (digitalRead(buttonPin) == LOW) {
      if (pressedShort && (startMillis + shortPeriod) == t) {
        (*pressedShort)();
      } else if (pressedMedium && (startMillis + mediumPeriod) == t) {
        (*pressedMedium)();
      } else if (pressedLong && (startMillis + longPeriod) == t) {
        (*pressedLong)();
      }
    } else {
      dt = t - startMillis;
      if (releasedShort && (startMillis + shortPeriod) <= t &&
        (startMillis + mediumPeriod) > t) {
        (*releasedShort)(dt);
      } else if (releasedMedium && (startMillis + mediumPeriod) <= t &&
        (startMillis + longPeriod) > t) {
        (*releasedMedium)(dt);
      } else if (releasedLong && (startMillis + longPeriod) <= t) {
        (*releasedLong)(dt);
      }
      buttonActive = false;
    }
  } else {
    buttonActive = (bool)!digitalReadFast(buttonPin);
    ///buttonActive = (bool)!digitalRead(buttonPin);
    if (buttonActive) startMillis = t;
  }

}
