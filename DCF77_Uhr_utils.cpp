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

Adafruit_7Seg disp1;
Adafruit_7Seg disp2;
Adafruit_7Seg disp3;
OneButton uniButton;
viewModes viewMode = VIEW_SEC;
viewModes lastViewMode = VIEW_UNDEFINED;
alarmModes lastAlarmMode = UNDEFINED;

muTimer periodTimer = muTimer();
muTimer buzzerTimer = muTimer();

alarm_time_t alarm = {.hour = 0,
                      .minute = 0,
                      .snoozeStart = 0,
                      .mode = UNDEFINED,
                      .state = INVALID};

sync_t sync = {.hour = SYNC_HOUR,      // nötig?
               .minute = SYNC_MINUTE,  // nötig?
               .syncing = false,
               .quality = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 43, 48, 50}};

void setNormalMode(void) {
  disp1.normal();
  disp2.normal();
  disp3.normal();
}

void setSleepMode(void) {
  disp1.sleep();
  disp2.sleep();
  disp3.sleep();
}

void refreshDisplays(void) {
  disp1.sendLed();
  disp2.sendLed();
  disp3.sendLed();
}

uint8_t BCDselectors[] = {ALARM1_HOUR_HI_PIN,   ALARM1_HOUR_LO_PIN,
                          ALARM1_MINUTE_HI_PIN, ALARM1_MINUTE_LO_PIN,
                          ALARM2_HOUR_HI_PIN,   ALARM2_HOUR_LO_PIN,
                          ALARM2_MINUTE_HI_PIN, ALARM2_MINUTE_LO_PIN};
                  
uint8_t digitPos[] = { DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4 };

// Auxiliary functions for number display
void showNumber(Adafruit_7Seg &disp, uint8_t pos, uint8_t num, bool dp) {
  // note: "call by reference" for parameter 'disp'
  uint8_t p = pos;
  disp.setDigit(digitPos[p--], num % 10, dp);
  disp.setDigit(digitPos[p],   num / 10);
}
void showNumber(Adafruit_7Seg &disp, uint8_t pos, uint8_t num) {
  showNumber(disp, pos, num, false);
}

// this function will be called several times after SHORT_PRESS has
// expired.
void detectingPress(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= MEDIUM_PRESS && delta < LONG_PRESS) {
    alarm.state = WAITING;  // Should also be effective for >=LONG_PRESS
    if (sync.syncing) {
      sync.syncing = false;
      setNormalMode();
    }
    setBuzzer(OFF);  // TODO: akustische Rückmeldung?!
  } else if (!sync.syncing && delta >= LONG_PRESS) {
    sync.syncing = true;
    setSleepMode();                 // + syncing, sync.syncing = true
    alarm.state = ACTIVE;           // provisorisch zum Testen
    buzzerTimer.cycleResetToOff();  // Suppresses noise
    // beides später einmalig beim Zeit-Alarm-Vergleich
  }
}

// this function will be called when the key is released after SHORT_PRESS
// has expired.
void detectingPressStop(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= SHORT_PRESS && delta < MEDIUM_PRESS) {
    if (sync.syncing) {
      sync.syncing = false;
      setNormalMode();
    }
    if (alarm.state == ACTIVE) {
      alarm.state = SNOOZE;
      // Switches back to ACTIVE after ALARM_SNOOZE_MAX has elapsed
      alarm.snoozeStart = millis();
      setBuzzer(OFF);  // TODO: akustische Rückmeldung?!
    } else {           // Set next view mode
      if (viewMode == VIEW_SEC) {
        viewMode = VIEW_DATE;
      } else if (viewMode == VIEW_DATE) {
        viewMode = VIEW_QTY;
      } else if (viewMode == VIEW_QTY) {
        viewMode = VIEW_VERSION;
      } else {
        viewMode = VIEW_SEC;
      }
    }
  }
}

alarmModes getAlarmMode(void) {
  /*
    Determines the alarm mode as follows:

          o  VCC
          |
          o
         \   Alarm Off / On
          o
          |
          o-----.
          |     |
         .-.    o
     10k | |   \  Alarm 1 / Alarm 2
         '-'    o
          |     |
          o-----o-----------> ALARM_MODE_PIN
          |
         .-.
     10k | |
         '-'
          |
         --- GND
  */
  int rawADC = analogRead(ALARM_MODE_PIN);
  // float val = ((float) rawADC  + 0.5) / 1024.0 * AREF;
  if (rawADC < ADC_20_PERCENT)  // GND
    return DISABLED;
  else if (rawADC > ADC_80_PERCENT)
    return TWO;                 // VCC
  else
    return ONE;                 // VCC / 2
}

uint8_t bcdRead(uint8_t pos) {
  /*
    .---------------------------o ALARM_BCD8_PIN (internal pull-up resistor)
    |       .-------------------o ALARM_BCD4_PIN (internal pull-up resistor)
    |       |       .-----------o ALARM_BCD2_PIN (internal pull-up resistor)
    |       |       |       .---o ALARM_BCD1_PIN (internal pull-up resistor)
    |       |       |       |
   ---     ---     ---     ---
   \ / D1  \ / D2  \ / D3  \ / D4 
   ---     ---     ---     ---
    |       |       |       |
    o       o       o       o
   \ "8"   \ "4"   \ "2"   \ "1"      The first of the 8 BCD switches
    o       o       o       o
    |       |       |       |
    '-------o-------o-------'
            |
            o ALARM1_HOUR_HI_PIN (BCD switch selection, low active)
  */  
  uint8_t val;
  digitalWrite(pos, LOW);  // Selecting the specific BCD switch
  delayMicroseconds(10);   // necessary?
  // reading the whole port and masking the 4 higher bits and then
  // inverting the 4 lower bits
  val = (digitalPinToPort(ALARM_BCD1_PIN) & 0b00001111) ^ 0b00001111;
  digitalWrite(pos, HIGH); // Deselecting the specific BCD switch
  return val;
}

uint8_t getHour(uint8_t hiPos, uint8_t loPos) {
  uint8_t x = bcdRead(hiPos) * 10 + bcdRead(loPos);
  // === dummy ===
  if (hiPos == ALARM1_HOUR_HI_PIN) {
    x = 7;  // 7:32
  } else {
    x = 9;  // 9:32
  }
  // =============
  // sets bit 7 if the result is invalid
  if (x > 23) x |= 0x80;
  return x;
}

uint8_t getMinute(uint8_t hiPos, uint8_t loPos) {
  uint8_t x = bcdRead(hiPos) * 10 + bcdRead(loPos);
  // === dummy ===
  x = 32;
  // =============
  // sets bit 7 if the result is invalid
  if (x > 59) x |= 0x80;
  return x;
}

void updateAlarmSettings(void) {
  bool invalid = false;
  uint8_t x;
  alarm.mode = getAlarmMode();
  if (alarm.mode != lastAlarmMode) {
    lastAlarmMode = alarm.mode;
    alarm.state = WAITING;
    if (alarm.mode == ONE) {
      disp1.clearPoint(POINT_LOWER_LEFT);
      disp1.setPoint(POINT_UPPER_LEFT);
      x = getMinute(ALARM1_MINUTE_HI_PIN, ALARM1_MINUTE_LO_PIN);
      // mask bit 7
      alarm.minute = x & 0x7f;
      // mask bit 6..bit 0
      invalid |= x & 0x80;
      x = getHour(ALARM1_HOUR_HI_PIN, ALARM1_HOUR_LO_PIN);
      alarm.hour = x & 0x7f;
      invalid |= x & 0x80;
      Serial.print("ALARM 1: ");
      Serial.print(alarm.hour); Serial.print(":");
      Serial.print(alarm.minute);
      Serial.println((invalid) ? " (invalid)" : "");
    } else if (alarm.mode == TWO) {
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.setPoint(POINT_LOWER_LEFT);
      x = getMinute(ALARM2_MINUTE_HI_PIN, ALARM2_MINUTE_LO_PIN);
      // mask bit 7
      alarm.minute = x & 0x7f;
      // mask bit 6..bit 0
      invalid |= x & 0x80;
      x = getHour(ALARM2_HOUR_HI_PIN, ALARM2_HOUR_LO_PIN);
      alarm.hour = x & 0x7f;
      invalid |= x & 0x80;
      Serial.print("ALARM 2: ");
      Serial.print(alarm.hour); Serial.print(":");
      Serial.print(alarm.minute);
      Serial.println((invalid) ? " (invalid)" : "");
    } else { // DISABLED
      invalid = true;
      alarm.minute = ALARM_VAL_UNDEF;
      alarm.hour = ALARM_VAL_UNDEF;
      Serial.println("ALARM: DISABLED");
    }
    if (invalid) {
      alarm.state = INVALID;
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.clearPoint(POINT_LOWER_LEFT);
    }
    refreshDisplays();
  }
}

void snoozeHandling(void) {
  if (alarm.state != SNOOZE) return;
  if ((unsigned long)(millis() - alarm.snoozeStart) >= ALARM_SNOOZE_MAX) {
    // activate the alarm after the end of the snooze time
    alarm.state = ACTIVE;
  }
  return;
}

// Turn piezo buzzer on or off
void setBuzzer(uint8_t x) { digitalWrite(BUZZER_PIN, x); }

void updateBuzzerCycle(void) {
  if (alarm.state != ACTIVE) return;
  switch (buzzerTimer.cycleOnOffTrigger(BUZZER_ON, BUZZER_OFF)) {
    // state changed from 1->0
    case 0:
      setBuzzer(OFF);
      break;
    // state changed from 0->1
    case 1:
      setBuzzer(ON);
      break;
    // no state change, timer is running
    case 2:
      break;
  }
}

volatile unsigned long tick = 0;

void output_handler(const Clock::time_t &decoded_time) {
  //Serial.print("tick: "); Serial.println(tick);
}

unsigned long myMillis(void) {// ???
  unsigned long m;
  cli();
  m = tick;
  sei(); 
  return m;
}

uint8_t sample_input_pin() {

  tick++; // Ersatz für millis()
  if (tick % 1000 == 0) {
    // Serial.print("1s-tick: "); Serial.println(tick);
  }

  if (sync.syncing)  {
  /*
    const uint8_t sampled_data = DCF77_INVERTED_SAMPLES ^ digitalRead(DCF77_SAMPLE_PIN);
    digitalWrite(DCF77_MONITOR_LED, sampled_data);
    return sampled_data;
  */
    return LOW;
  } else {
    digitalWrite(DCF77_MONITOR_LED, HIGH); // einmalig beim Abschalten von "syncing"
    return LOW;
  }
}

void longPeriod(void) {
  updateAlarmSettings(); // Call is rarely required
  snoozeHandling();      // Call is rarely required
}

void stop_timer_0(void) {
#if defined(TIMSK) && defined(TOIE0)
  cbi (TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
  cbi (TIMSK0, TOIE0);
#endif
}

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

// TODO: In preSyncing stop_timer_0();
// TODO: In postSyncing restart_timer_0();

void setupDCF77_Uhr(void) {
  analogReference(DEFAULT);
  
  Serial.begin(9600);
  //Serial.begin(115200);
  while (!Serial) ;

  pinMode(DCF77_MONITOR_LED, OUTPUT);
  pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP);
  // /*
  using namespace Clock;
  DCF77_Clock::setup();
  DCF77_Clock::set_input_provider(sample_input_pin);
  DCF77_Clock::set_output_handler(output_handler);
  restart_timer_0(); // ???
  // */


  disp1.begin(DISP1_ADR);
  disp2.begin(DISP2_ADR);
  disp3.begin(DISP3_ADR);
  disp1.setBrightness(16);  // 0..16
  disp2.setBrightness(3);   // 0..16
  disp3.setBrightness(3);   // 0..16
  disp1.setPoint(COLON);

  pinMode(ALARM_BCD1_PIN, INPUT_PULLUP);
  pinMode(ALARM_BCD2_PIN, INPUT_PULLUP);
  pinMode(ALARM_BCD4_PIN, INPUT_PULLUP);
  pinMode(ALARM_BCD8_PIN, INPUT_PULLUP);
  // sets all BCD switch selection pins as output and level to high (no selection)
  for (uint8_t i = 0; i < ARRAYSIZE(BCDselectors); i++) {
    pinMode(BCDselectors[i], OUTPUT);
    digitalWrite(BCDselectors[i], HIGH);
  }

  uniButton = OneButton(UNI_BUTTON_PIN,     // Input pin for the button
                        true,               // Button is active LOW
                        true                // Enable internal pull-up resistor
  );
  uniButton.setPressMs(SHORT_PRESS);
  uniButton.attachDuringLongPress(detectingPress,
                                  &uniButton);  // During Long Press events
  uniButton.attachLongPressStop(detectingPressStop,
                                &uniButton);  // Long Press Stop event

  pinMode(BUZZER_PIN, OUTPUT);

  setBuzzer(ON); delay(100); setBuzzer(OFF); // setup ready
}
