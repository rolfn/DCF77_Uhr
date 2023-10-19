
#ifndef BEEPER_H
#define BEEPER_H

#include <Arduino.h>

#define BUZZER_PIN 11             // PB3  (piezo active buzzer) 
#define SINGLE_BEEP_ON       300L // ms
#define ALTERNATING_BEEP_ON  300L // ms
#define ALTERNATING_BEEP_OFF 150L // ms

#define ON HIGH
#define OFF LOW

extern void setBuzzer(const uint8_t x);
extern void alternatingBeep(const uint8_t sw);
extern void singleBeep(void);
extern void beeperHandling(const unsigned long t);

#endif
