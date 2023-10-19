
#include "beeper.h"

//uint8_t buzzer_pin;
unsigned long single_beep_on;
unsigned long alternating_beep_on;
unsigned long alternating_beep_off;

uint8_t buzzerState = 255;
unsigned long singleBeepCounter = 0;
unsigned long alternatingBeepCounter = 0;

void setBuzzer(const uint8_t x) {
  if (buzzerState == x) return;
  buzzerState = x;
  digitalWrite(BUZZER_PIN, x); 
}

void alternatingBeep(const uint8_t sw) {
  if (sw == ON) {
    alternatingBeepCounter = ALTERNATING_BEEP_ON;
    setBuzzer(ON);
  } else {
    alternatingBeepCounter = 0;
    setBuzzer(OFF);
  }
}

void alternatingBeepHandling(const unsigned long ticks) {
  static unsigned long lastMillis = 0;
  if (alternatingBeepCounter == 0 || ticks == lastMillis) return;
  lastMillis = ticks;
  alternatingBeepCounter--;
  if (alternatingBeepCounter == 0) {
    if (buzzerState == ON) {
      alternatingBeepCounter = ALTERNATING_BEEP_OFF;
      setBuzzer(OFF);
    } else {
      alternatingBeepCounter = ALTERNATING_BEEP_ON;
      setBuzzer(ON);
    }
  } 
}

void singleBeep(void) {
  singleBeepCounter = SINGLE_BEEP_ON;
  setBuzzer(ON);
}

void singleBeepHandling(const unsigned long ticks) {
  static unsigned long lastMillis = 0;
  if (singleBeepCounter == 0 || ticks == lastMillis) return;
  lastMillis = ticks;
  singleBeepCounter--;
  if (singleBeepCounter == 0) {
    setBuzzer(OFF);
  } 
}

void beeperHandling(const unsigned long t) {
  singleBeepHandling(t);
  alternatingBeepHandling(t);
}

