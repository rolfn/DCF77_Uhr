
#include "beepers.h"

uint8_t buzzer_pin;
unsigned long single_beep_on;
unsigned long alternating_beep_on;
unsigned long alternating_beep_off;

uint8_t buzzerState = 255;
unsigned long singleBeepCounter = 0;
unsigned long alternatingBeepCounter = 0;

void setupBeepers(const uint8_t pin, const unsigned long s_on,
  const unsigned long a_on, const unsigned long a_off) {
  buzzer_pin = pin;
  pinMode(pin, OUTPUT);
  single_beep_on = s_on;
  alternating_beep_on = a_on;
  alternating_beep_off = a_off;
}

void setBuzzer(const uint8_t x) {
  if (buzzerState == x) return;
  buzzerState = x;
  digitalWrite(buzzer_pin, x); 
}

void alternatingBeep(const uint8_t sw) {
  if (sw == ON) {
    alternatingBeepCounter = alternating_beep_on;
    setBuzzer(ON);
  } else {
    alternatingBeepCounter = 0;
    setBuzzer(OFF);
  }
}

void alternatingBeepsHandling(const unsigned long ticks) {
  static unsigned long lastMillis = 0;
  if (alternatingBeepCounter == 0 || ticks == lastMillis) return;
  lastMillis = ticks;
  alternatingBeepCounter--;
  if (alternatingBeepCounter == 0) {
    if (buzzerState == ON) {
      alternatingBeepCounter = alternating_beep_off;
      setBuzzer(OFF);
    } else {
      alternatingBeepCounter = alternating_beep_on;
      setBuzzer(ON);
    }
  } 
}

void singleBeep(void) {
  singleBeepCounter = single_beep_on;
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

void beepersHandling(const unsigned long t) {
  singleBeepHandling(t);
  alternatingBeepsHandling(t);
}

