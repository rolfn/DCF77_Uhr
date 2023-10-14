/*
 * DCF77_Uhr_utils.cpp
 *
 * See: https://github.com/rolfn/DCF77_Uhr
 *
 * Copyright:  Rolf Niepraschk
 * License: MIT
 *
 */

#include "DCF77_Uhr.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void restart_timer_0(void) {
// enable timer 0 overflow interrupt
// --> wiring.c
#if defined(TIMSK) && defined(TOIE0)
  sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
  sbi(TIMSK0, TOIE0);
#else
  #error  Timer 0 overflow interrupt not set correctly
#endif
}


Adafruit_7Seg disp1;

#if defined(__AVR__)
const uint8_t dcf77_analog_sample_pin = 5;
const uint8_t dcf77_sample_pin = 12;       // A5 == d19
const uint8_t dcf77_inverted_samples = 1;
const uint8_t dcf77_analog_samples = 0;
// const uint8_t dcf77_pin_mode = INPUT;  // disable internal pull up
const uint8_t dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up

const uint8_t dcf77_monitor_led = 13;  // A4 == d18

uint8_t ledpin(const uint8_t led) {
    return led;
}
#elif defined(__STM32F1__)
const uint8_t dcf77_sample_pin = PB6;
const uint8_t dcf77_inverted_samples = 1; //output from HKW EM6 DCF 3V
const WiringPinMode dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up
const uint8_t dcf77_monitor_led = PC13;
uint8_t ledpin(const int8_t led) {
    return led;
}
#else
const uint8_t dcf77_sample_pin = 53;
const uint8_t dcf77_inverted_samples = 1;

// const uint8_t dcf77_pin_mode = INPUT;  // disable internal pull up
const uint8_t dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up

const uint8_t dcf77_monitor_led = 19;

uint8_t ledpin(const uint8_t led) {
    return led<14? led: led+(54-14);
}
#endif

unsigned long ticks = 0;

uint8_t sample_input_pin() {// Called every 1ms
  // vereinfachen!?
  ticks++;
  const uint8_t sampled_data =
    #if defined(__AVR__)
    dcf77_inverted_samples ^ (dcf77_analog_samples? (analogRead(dcf77_analog_sample_pin) > 200)
                                                  : digitalRead(dcf77_sample_pin));
    #else
    dcf77_inverted_samples ^ digitalRead(dcf77_sample_pin);
    #endif

  digitalWrite(ledpin(dcf77_monitor_led), sampled_data);
  return sampled_data;
}

// Turn piezo buzzer on or off
void setBuzzer(uint8_t x) { digitalWrite(BUZZER_PIN, x); }

void paddedPrint(BCD::bcd_t n) {
  sprint(n.digit.hi);
  sprint(n.digit.lo);
}

bool first = true;

void everySecond(void) {
  Clock::time_t now;
  
  if (first) {
    first = false;
    setBuzzer(ON); delay(100); setBuzzer(OFF); // sync ready
  }
  
  DCF77_Clock::get_current_time(now);

  if (now.month.val > 0) {
#ifdef DEBUG
    switch (DCF77_Clock::get_clock_state()) {
        case Clock::useless: sprint(F("useless ")); break;
        case Clock::dirty:   sprint(F("dirty:  ")); break;
        case Clock::synced:  sprint(F("synced: ")); break;
        case Clock::locked:  sprint(F("locked: ")); break;
    }
    sprint(' ');

    sprint(F("20"));
    paddedPrint(now.year);
    sprint('-');
    paddedPrint(now.month);
    sprint('-');
    paddedPrint(now.day);
    sprint(' ');

    paddedPrint(now.hour);
    sprint(':');
    paddedPrint(now.minute);
    sprint(':');
    paddedPrint(now.second);

    sprint("+0");
    sprint(now.uses_summertime? '2': '1');
    sprintln();
#endif
    disp1.setDigit(DIGIT_1, now.hour.digit.hi);
    disp1.setDigit(DIGIT_2, now.hour.digit.lo);
    disp1.setDigit(DIGIT_3, now.minute.digit.hi);
    disp1.setDigit(DIGIT_4, now.minute.digit.lo);
    disp1.sendLed();
  }  
}

void setupDCF77_Uhr(void) {
  analogReference(DEFAULT); 

  disp1.begin(DISP1_ADR);
  disp1.sleep();

  Serial.begin(BAUDRATE);
  while (!Serial) ;
#ifdef DEBUG
  sprintln();
  sprintln();
  sprintln();
  sprintln("=== setupDCF77_Uhr ====");
  sprintln();
  sprintln(F("Simple DCF77 Clock V3.1.1"));
  sprintln(F("(c) Udo Klein 2016"));
  sprintln(F("www.blinkenlight.net"));
  sprintln();
  sprint(F("Sample Pin:      ")); sprintln(dcf77_sample_pin);
  sprint(F("Sample Pin Mode: ")); sprintln(dcf77_pin_mode);
  sprint(F("Inverted Mode:   ")); sprintln(dcf77_inverted_samples);
  sprint(F("Analog Mode:     ")); sprintln(dcf77_analog_samples);
  sprint(F("Monitor Pin:     ")); sprintln(ledpin(dcf77_monitor_led));
  sprintln();
  sprintln();
  sprintln(F("Initializing..."));
#endif

  ///pinMode(ledpin(dcf77_monitor_led), OUTPUT); // ???
  ///pinMode(dcf77_sample_pin, dcf77_pin_mode);  // ???
  
  pinMode(DCF77_MONITOR_LED, OUTPUT);
  pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(ON); delay(100); setBuzzer(OFF); // setup begin

  using namespace Clock;
  DCF77_Clock::setup();
  restart_timer_0();
  DCF77_Clock::set_input_provider(sample_input_pin);

  static uint8_t count = 0;

  // Wait till clock is synced, depending on the signal quality this may take
  // rather long. About 5 minutes with a good signal, 30 minutes or longer
  // with a bad signal
  for (uint8_t state = Clock::useless;
    state == Clock::useless || state == Clock::dirty;
    state = DCF77_Clock::get_clock_state()) {

    // wait for next sec
    Clock::time_t now;
    DCF77_Clock::get_current_time(now);
#ifdef DEBUG
    // render one dot per second while initializing
    ///sprint('.');
    sprint(state);
    ++count;
    if (count == 60) {
      count = 0;
      Serial.println();
    }
#endif
  }
  disp1.normal();
  disp1.setBrightness(16);  // 0..16
  disp1.setPoint(COLON); 
}
/*
useless  = 0,  // waiting for good enough signal
dirty    = 1,  // time data available but unreliable
free     = 2,  // clock was once synced but now may deviate more than 200 ms, must not re-lock if valid phase is detected
unlocked = 3,  // lock was once synced, inaccuracy below 200 ms, may re-lock if a valid phase is detected
locked   = 4,  // clock driven by accurate phase, time is accurate but not all decoder stages have sufficient quality for sync
synced   = 5   // best possible quality, clock is 100% synced
*/
