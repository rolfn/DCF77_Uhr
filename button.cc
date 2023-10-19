
#include "button.h"

uint8_t buttonPin = BUTTON_PIN;

//unsigned long shortPeriod = 0;  // ms
//unsigned long mediumPeriod = 0; // ms
//unsigned long longPeriod = 0;   // ms

parameterizedCallbackFunction releasedShort = NULL; 
parameterizedCallbackFunction releasedMedium = NULL; 
parameterizedCallbackFunction releasedLong = NULL;
callbackFunction pressedShort = NULL;
callbackFunction pressedMedium = NULL;
callbackFunction pressedLong = NULL;

void setupButtonCallbackShort(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  // Function that is called when the button is released after time "MEDIUM_PERIOD".
  releasedShort = whenReleased;
  // Function that is called when "MEDIUM_PERIOD" has passed while the button is pressed.
  pressedShort = whilePressed;
}
void setupButtonCallbackMedium(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  // Function that is called when the button is released after time "MEDIUM_PERIOD".
  releasedMedium = whenReleased;
  // Function that is called when "MEDIUM_PERIOD" has passed while the button is pressed.
  pressedMedium = whilePressed;
}
void setupButtonCallbackLong(
  const parameterizedCallbackFunction whenReleased, 
  const callbackFunction whilePressed) {
  // Function that is called when the button is released after time "LONG_PERIOD".
  releasedLong = whenReleased;
  // Function that is called when "LONG_PERIOD" has passed while the button is pressed.
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
      if (pressedShort && (startMillis + SHORT_PERIOD) == t) {
        (*pressedShort)();
      } else if (pressedMedium && (startMillis + MEDIUM_PERIOD) == t) {
        (*pressedMedium)();
      } else if (pressedLong && (startMillis + LONG_PERIOD) == t) {
        (*pressedLong)();
      }
    } else {
      dt = t - startMillis;
      if (releasedShort && (startMillis + SHORT_PERIOD) <= t &&
        (startMillis + MEDIUM_PERIOD) > t) {
        (*releasedShort)(dt);
      } else if (releasedMedium && (startMillis + MEDIUM_PERIOD) <= t &&
        (startMillis + LONG_PERIOD) > t) {
        (*releasedMedium)(dt);
      } else if (releasedLong && (startMillis + LONG_PERIOD) <= t) {
        (*releasedLong)(dt);
      }
      buttonActive = false;
    }
  } else {
    buttonActive = (bool)!digitalReadFast(buttonPin);
    if (buttonActive) startMillis = t;
  }

}
