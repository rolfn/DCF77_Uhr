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
views viewMode = VIEW_SEC, lastViewMode = VIEW_UNDEFINED;
alarmModes lastAlarmMode = UNDEFINED;

muTimer periodTimer = muTimer();
muTimer buzzerTimer = muTimer();
muTimer snoozeTimer = muTimer();

alarm_time_t alarm = {.hour = {.val = ALARM_UNDEFINED},
                      .minute = {.val = ALARM_UNDEFINED},
                      .snoozeStart = 0,
                      .mode = ONE,
                      .state = WAITING};

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

// this function will be called several times after SHORT_PRESS_TIME has
// expired.
void handleDuringLongPress(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();  // TODO: simplify
  if (delta >= MEDIUM_PRESS_TIME && delta < LONG_PRESS_TIME) {
    alarm.state = WAITING;  // Should also be effective for >=LONG_PRESS_TIME
    setBuzzer(OFF);         // TODO: akustische Rückmeldung?!
  } else if (!sync.syncing && delta >= LONG_PRESS_TIME) {
    sync.syncing = true;
    setSleepMode();                 // + syncing, sync.syncing = true
    alarm.state = ACTIVE;           // provisorisch zum Testen
    buzzerTimer.cycleResetToOff();  // Suppresses noise
    // beides später einmalig beim Zeit-Alarm-Vergleich
  }
}

// this function will be called when the key is released after SHORT_PRESS_TIME
// has expired.
void handleLongPressStop(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= SHORT_PRESS_TIME && delta < MEDIUM_PRESS_TIME) {
    if (sync.syncing) {
      sync.syncing = false;
      setNormalMode();
    }
    if (alarm.state == ACTIVE) {
      alarm.state = SNOOZE;  // TODO: Per 5-Minuten-Timer auf ACTIVE setzen
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

uint8_t getAlarmMode(void) {
  /*
    Determines the alarm mode as folows:

          o  VCC
          |
          o
        \
         \   Alarm Off / On
          o
          |
          o-----.
          |     |
         .-.    o
         | |  \
         | |   \  Alarm 1 / Alarm 2
         '-'    o
          |     |
          o-----o-----------> ALARM_MODE_PIN
          |
         .-.
         | |
         | |
         '-'
          |
         --- GND
  */
  int rawADC = analogRead(ALARM_MODE_PIN);
  // float val = ((float) rawADC  + 0.5) / 1024.0 * AREF;
  if (rawADC < ADC_20_PERCENT)
    return DISABLED;
  else if (rawADC > ADC_80_PERCENT)
    return TWO;
  else
    return ONE;
}

uint8_t bcdRead(uint8_t pos) {
  uint8_t val;
  digitalWrite(pos, LOW);       // selecting the specific BCD switch
  delayMicroseconds(2);         // necessary?
  val = ALARM_BCD_PORT & 0x0f;  // mask the 4 high order bits
  digitalWrite(pos, HIGH);      // deselecting the specific BCD switch
  return val;
}

void updateAlarmSettings(void) {
  if (alarm.mode != lastAlarmMode) {
    lastAlarmMode = alarm.mode;
    if (alarm.mode == ONE) {
      disp1.clearPoint(POINT_LOWER_LEFT);
      disp1.setPoint(POINT_UPPER_LEFT);
      alarm.minute.digit.lo = bcdRead(ALARM1_MINUTE_LO_PIN);
      alarm.minute.digit.hi = bcdRead(ALARM1_MINUTE_HI_PIN);
      alarm.hour.digit.lo = bcdRead(ALARM1_HOUR_LO_PIN);
      alarm.hour.digit.hi = bcdRead(ALARM1_HOUR_HI_PIN);
    } else if (alarm.mode == TWO) {
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.setPoint(POINT_LOWER_LEFT);
      alarm.minute.digit.lo = bcdRead(ALARM2_MINUTE_LO_PIN);
      alarm.minute.digit.hi = bcdRead(ALARM2_MINUTE_HI_PIN);
      alarm.hour.digit.lo = bcdRead(ALARM2_HOUR_LO_PIN);
      alarm.hour.digit.hi = bcdRead(ALARM2_HOUR_HI_PIN);
    } else {  // DISABLED
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.clearPoint(POINT_LOWER_LEFT);
      alarm.minute.val = ALARM_UNDEFINED;
      alarm.hour.val = ALARM_UNDEFINED;
    }
  }
}

void handleSnooze(void) {
  if (alarm.state == SNOOZE) {
    if ((unsigned long)(millis() - alarm.snoozeStart) >= ALARM_SNOOZE_MAX) {
      // activate the alarm after the end of the snooze time
      alarm.state = ACTIVE;
    }
  }
  return;
}

// Turn piezo buzzer on or off
void setBuzzer(uint8_t x) { digitalWrite(BUZZER_PIN, x); }

void handleBuzzer(void) {
  if (alarm.state != ACTIVE) {
    return;
  }
  switch (buzzerTimer.cycleOnOffTrigger(BUZZER_ON_TIME, BUZZER_OFF_TIME)) {
      // TODO: #define
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

void setupDCF77_Uhr(void) {
  disp1.begin(DISP1_ADR);
  disp2.begin(DISP2_ADR);
  disp3.begin(DISP3_ADR);
  disp1.setBrightness(16);  // 0..16
  disp2.setBrightness(3);   // 0..16
  disp3.setBrightness(3);   // 0..16
  disp1.setPoint(COLON);
  // TODO: setupPins()
  pinMode(ALARM_BCD1, INPUT_PULLUP);
  pinMode(ALARM_BCD2, INPUT_PULLUP);
  pinMode(ALARM_BCD4, INPUT_PULLUP);
  pinMode(ALARM_BCD8, INPUT_PULLUP);
  DDRD = DDRD | B11111100;             // PD2..PD7 as output (BCD selection)
  DDRB = DDRB | B00000011;             // PB0..PB1 as output (BCD selection)
  pinMode(DCF77_MONITOR_LED, OUTPUT);  // nötig?
  // pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP);  // nötig?
  uniButton = OneButton(UNI_BUTTON_PIN,  // Input pin for the button
                        true,            // Button is active LOW
                        true             // Enable internal pull-up resistor
  );
  /// uniButton.setClickMs(SINGLE_CLICK_TIME);
  uniButton.setPressMs(SHORT_PRESS_TIME);
  /// uniButton.attachClick(handleSingleClick); // Single Click event
  uniButton.attachDuringLongPress(handleDuringLongPress,
                                  &uniButton);  // During Long Press events
  uniButton.attachLongPressStop(handleLongPressStop,
                                &uniButton);  // Long Press Stop event
  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(ON);
  delay(500);
  setBuzzer(OFF);
}
