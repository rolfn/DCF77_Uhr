
#include "beepers.h"

uint8_t buzzer_pin;
unsigned long single_beep_on;
unsigned long alternate_beep_on;
unsigned long alternate_beep_off;

uint8_t buzzerState = 255;
unsigned long singleBeepCounter = 0;
unsigned long alternateBeepCounter = 0;

void setupBeepers(uint8_t pin, unsigned long s_on, unsigned long a_on,
  unsigned long a_off) {
  buzzer_pin = pin;
  pinMode(pin, OUTPUT);
  single_beep_on = s_on;
  alternate_beep_on = a_on;
  alternate_beep_off = a_off;
}

void setBuzzer(uint8_t x) {
  if (buzzerState == x) return;
  buzzerState = x;
  digitalWrite(buzzer_pin, x); 
}

void alternateBeep(uint8_t sw) {
  if (sw == ON) {
    alternateBeepCounter = alternate_beep_on;
    setBuzzer(ON);
  } else {
    alternateBeepCounter = 0;
    setBuzzer(OFF);
  }
}

void updateAlternateBeeps(void) {
  static unsigned long lastMillis = 0;
  if (alternateBeepCounter == 0 || ticks == lastMillis) return;
  lastMillis = ticks;
  alternateBeepCounter--;
  if (alternateBeepCounter == 0) {
    if (buzzerState == ON) {
      alternateBeepCounter = alternate_beep_off;
      setBuzzer(OFF);
    } else {
      alternateBeepCounter = alternate_beep_on;
      setBuzzer(ON);
    }
  } 
}

void singleBeep(void) {
  singleBeepCounter = single_beep_on;
  setBuzzer(ON);
}

void updateSingleBeep(void) {
  static unsigned long lastMillis = 0;
  if (singleBeepCounter == 0 || ticks == lastMillis) return;
  lastMillis = ticks;
  singleBeepCounter--;
  if (singleBeepCounter == 0) {
    setBuzzer(OFF);
  } 
}

void updateBeepers(void) {
  updateSingleBeep();
  updateAlternateBeeps();
}

