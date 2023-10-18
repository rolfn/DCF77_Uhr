
#include <Arduino.h>
#include "beeper.h"
#include "button.h"

/*
TODO: https://github.com/ArminJo/digitalWriteFast
*/

#define PERIOD 4000L

#define BUZZER_PIN 11             // PB3  (piezo active buzzer)
#define SINGLE_BEEP_ON       300L // ms
#define ALTERNATING_BEEP_ON  300L // ms
#define ALTERNATING_BEEP_OFF 150L // ms

#define BUTTON_PIN 10      // PB2
#define SHORT_PRESS    50L // ms
#define MEDIUM_PRESS 1500L // ms
#define LONG_PRESS   3500L // ms
 
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
  Serial.print("buttonPressedShort ("); Serial.print(SHORT_PRESS); Serial.println(")");
}
void buttonPressedMedium() {
  Serial.print("buttonPressedMedium ("); Serial.print(MEDIUM_PRESS); Serial.println(")");
}
void buttonPressedLong() {
  Serial.print("buttonPressedLong ("); Serial.print(LONG_PRESS); Serial.println(")");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  setupBeeper(BUZZER_PIN, SINGLE_BEEP_ON, ALTERNATING_BEEP_ON,
    ALTERNATING_BEEP_OFF);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupButton(BUTTON_PIN);
  setupButtonCallbackShort(SHORT_PRESS, &buttonReleasedShort, &buttonPressedShort);
  setupButtonCallbackMedium(MEDIUM_PRESS, &buttonReleasedMedium, &buttonPressedMedium);
  setupButtonCallbackLong(LONG_PRESS, &buttonReleasedLong, &buttonPressedLong);
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
