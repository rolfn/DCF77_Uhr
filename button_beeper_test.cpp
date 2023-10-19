
#include <Arduino.h>
#include "beeper.h"
#include "button.h"

/*
TODO: https://github.com/ArminJo/digitalWriteFast
*/

#define PERIOD 4000L
 
unsigned long ticks = 0;

unsigned long startMillis = 0;

void buttonReleasedShort(unsigned long ms) {
  Serial.print("buttonReleasedShort: "); Serial.println(ms);
}
void buttonReleasedMedium(unsigned long ms) {
  Serial.print("buttonReleasedMedium: "); Serial.println(ms);
}
void buttonReleasedLong(unsigned long ms) {
  Serial.print("buttonReleasedLong: "); Serial.println(ms);
}
void buttonPressedShort() {
  Serial.print("buttonPressedShort ("); Serial.print(SHORT_PERIOD); Serial.println(")");
}
void buttonPressedMedium() {
  Serial.print("buttonPressedMedium ("); Serial.print(MEDIUM_PERIOD); Serial.println(")");
}
void buttonPressedLong() {
  Serial.print("buttonPressedLong ("); Serial.print(LONG_PERIOD); Serial.println(")");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupButtonCallbackShort(&buttonReleasedShort, &buttonPressedShort);
  setupButtonCallbackMedium(&buttonReleasedMedium, &buttonPressedMedium);
  setupButtonCallbackLong(&buttonReleasedLong, &buttonPressedLong);
  startMillis = millis();
  singleBeep();
  //alternatingBeep(ON);
}

/*
uint8_t sample_input_pin() {
  ticks++;
  ...
}
*/

void loop() {
  ticks = millis();
  beeperHandling(ticks);
  buttonHandling(ticks);
  if (ticks == startMillis + 500L * 60) {// 0.5 min
    alternatingBeep(OFF);
    singleBeep();
  }
  if (ticks % PERIOD == 0) {
    //singleBeep(); 
  }
}
