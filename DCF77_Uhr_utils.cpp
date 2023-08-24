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

alarm_time_t alarm = {.hour = {.val = ALARM_VAL_UNDEF},
                      .minute = {.val = ALARM_VAL_UNDEF},
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

void refreshDisplays(void) {
  disp1.sendLed();
  disp2.sendLed();
  disp3.sendLed();
}

uint8_t BCDselectors[] = { 
  ALARM1_HOUR_HI_PIN, 
  ALARM1_HOUR_LO_PIN, 
  ALARM1_MINUTE_HI_PIN, 
  ALARM1_MINUTE_LO_PIN,  
  ALARM2_HOUR_HI_PIN, 
  ALARM2_HOUR_LO_PIN, 
  ALARM2_MINUTE_HI_PIN, 
  ALARM2_MINUTE_LO_PIN
 };

// this function will be called several times after SHORT_PRESS_TIME has
// expired.
void detectingPress(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= MEDIUM_PRESS_TIME && delta < LONG_PRESS_TIME) {
    alarm.state = WAITING;  // Should also be effective for >=LONG_PRESS_TIME
    if (sync.syncing) {
      sync.syncing = false;
      setNormalMode();
    }
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
void detectingPressStop(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= SHORT_PRESS_TIME && delta < MEDIUM_PRESS_TIME) {
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
  if (rawADC < ADC_20_PERCENT) // about GND
    return DISABLED;
  else if (rawADC > ADC_80_PERCENT)
    return TWO;                // about VCC
  else
    return ONE;                // about VCC / 2
}

uint8_t bcdRead(uint8_t pos) {
  uint8_t val;
  digitalWrite(pos, LOW);            // Selecting the specific BCD switch
  delayMicroseconds(10);             // necessary?
  // reading the whole port and mask the 4 high order bits
  val = digitalPinToPort(ALARM_BCD1_PIN) & 0b00001111; 
  digitalWrite(pos, HIGH);           // Deselecting the specific BCD switch
  return val;
}

void updateAlarmSettings(void) {
  alarm.mode = getAlarmMode();
  if (alarm.mode != lastAlarmMode) {
    lastAlarmMode = alarm.mode;
    alarm.state = WAITING;
    if (alarm.mode == ONE) {
      disp1.clearPoint(POINT_LOWER_LEFT);
      disp1.setPoint(POINT_UPPER_LEFT);
      alarm.minute.digit.lo = bcdRead(ALARM1_MINUTE_LO_PIN);
      alarm.minute.digit.hi = bcdRead(ALARM1_MINUTE_HI_PIN);
      alarm.hour.digit.lo   = bcdRead(ALARM1_HOUR_LO_PIN);
      alarm.hour.digit.hi   = bcdRead(ALARM1_HOUR_HI_PIN);
    } else if (alarm.mode == TWO) {
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.setPoint(POINT_LOWER_LEFT);
      alarm.minute.digit.lo = bcdRead(ALARM2_MINUTE_LO_PIN);
      alarm.minute.digit.hi = bcdRead(ALARM2_MINUTE_HI_PIN);
      alarm.hour.digit.lo   = bcdRead(ALARM2_HOUR_LO_PIN);
      alarm.hour.digit.hi   = bcdRead(ALARM2_HOUR_HI_PIN);
    } else { // DISABLED
      disp1.clearPoint(POINT_UPPER_LEFT);
      disp1.clearPoint(POINT_LOWER_LEFT);
      alarm.minute.val = ALARM_VAL_UNDEF;
      alarm.hour.val = ALARM_VAL_UNDEF;
    }
    refreshDisplays();
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

void updateBuzzerCycle(void) {
  if (alarm.state != ACTIVE) {
    return;
  }
  switch (buzzerTimer.cycleOnOffTrigger(BUZZER_ON_TIME, BUZZER_OFF_TIME)) {
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

void longPeriod(void) {
  updateAlarmSettings();
  handleSnooze();
}

void setupDCF77_Uhr(void) {
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

  /*
  DDRD  |= B11111100; // sets PD2..PD7 as output  
  PORTD |= B11111100; // sets PD2..PD7 to a high level
  DDRB  |= B00000011; // sets PB0..PB1 as output 
  PORTB |= B00000011; // sets PB0..PB1 to a high level
  */
  // sets all BCD switch selection pins as output and level to high (no selection)
  for (uint8_t i = 0; i < ARRAYSIZE(BCDselectors); i++) {
    pinMode(BCDselectors[i], OUTPUT);
    digitalWrite(BCDselectors[i], HIGH);
  }

  pinMode(DCF77_MONITOR_LED, OUTPUT);      // nötig?
  pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP); // nötig?
  uniButton = OneButton(UNI_BUTTON_PIN, // Input pin for the button
                        true,           // Button is active LOW
                        true            // Enable internal pull-up resistor
  );
  uniButton.setPressMs(SHORT_PRESS_TIME);
  uniButton.attachDuringLongPress(detectingPress,
                                &uniButton); // During Long Press events
  uniButton.attachLongPressStop(detectingPressStop,
                                &uniButton); // Long Press Stop event
  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(ON);
  delay(100);
  setBuzzer(OFF);
}
