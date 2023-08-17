/**
 * DCF77_Uhr_utils.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#include "DCF77_Uhr.h"

Adafruit_7Seg disp1;
Adafruit_7Seg disp2;
Adafruit_7Seg disp3;
OneButton uniButton; 
views viewMode = VIEW_SEC, lastViewMode = VIEW_UNDEFINED;

alarm_time_t alarm = {
  .hour =   { .val = ALARM_UNDEFINED },
  .minute = { .val = ALARM_UNDEFINED }, 
  .period = 0,
  .mode = ALARM_DISABLED,
  .active = false
};

alarm_time_t sync = {
  .hour =   { .val = SYNC_HOUR },
  .minute = { .val = SYNC_MINUTE },
  .period = 0, 
  .mode = ALARM_DISABLED,
  .active = false
};

void setNormalMode() {
  sync.active = false;
  disp1.normal();
  disp2.normal();
  disp3.normal();
}

void setSleepMode() {
  sync.active = true;
  disp1.sleep();
  disp2.sleep();
  disp3.sleep();
}

// this function will be called when the button is released within SINGLE_CLICK_TIME.
void handleSingleClick() {
  Serial.println("SINGLE_CLICK_TIME");
  if (sync.active) {
    // TODO: stop syncing
    setNormalMode();
  } else {
    if (alarm.active) {
      // TODO: Alarm in snooze mode
    } else {// set next view mode
      if (viewMode == VIEW_SEC) viewMode = VIEW_DATE;
      else if (viewMode == VIEW_DATE) viewMode = VIEW_QTY;
      else viewMode = VIEW_SEC;
    }
  }
}

// this function will be called several times after PRESS_TIME has expired.
void handleDuringLongPress(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta >= LONG_PRESS_TIME && !sync.active) {
    Serial.print("Delta Time (ms): "); Serial.println(delta);
    // TODO: start syncing
    Serial.println(">= LONG_PRESS_TIME");
    setSleepMode();
  }
}

// this function will be called when the key is released after PRESS_TIME has expired.
void handleLongPressStop(void *btn) {
  unsigned long delta = ((OneButton *)btn)->getPressedMs();
  if (delta < LONG_PRESS_TIME) {
    Serial.print("Delta Time (ms): "); Serial.println(delta);
    Serial.println("< LONG_PRESS_TIME");
    // TODO: Alarm deactivate
    alarm.active = false;
  } 
}

uint8_t getAlarmMode() {
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
    return ALARM_DISABLED;
  else if (rawADC > ADC_80_PERCENT)
    return ALARM_2;
  else
    return ALARM_1;
}

uint8_t bcdRead(uint8_t pos) {
  uint8_t val;
  digitalWrite(pos, LOW); // selecting a specific BCD switch
  delayMicroseconds(2); // necessary?
  val = ALARM_BCD_PORT & 0x0f; // mask the 4 high order bits
  digitalWrite(pos, HIGH); // deselecting a specific BCD switch
  return val;
}
   
void updateAlarmSettings() {
  //alarm.mode = getAlarmMode(); TODO: activate if possible
  if (alarm.mode == ALARM_1) {
    alarm.minute.digit.lo = bcdRead(ALARM1_MINUTE_LO_PIN);
    alarm.minute.digit.hi = bcdRead(ALARM1_MINUTE_HI_PIN);
    alarm.hour.digit.lo = bcdRead(ALARM1_HOUR_LO_PIN);
    alarm.hour.digit.hi = bcdRead(ALARM1_HOUR_HI_PIN);    
  } else if (alarm.mode == ALARM_2) {
    alarm.minute.digit.lo = bcdRead(ALARM2_MINUTE_LO_PIN);
    alarm.minute.digit.hi = bcdRead(ALARM2_MINUTE_HI_PIN);
    alarm.hour.digit.lo = bcdRead(ALARM2_HOUR_LO_PIN);
    alarm.hour.digit.hi = bcdRead(ALARM2_HOUR_HI_PIN);  
  } else {// ALARM_DISABLED
    alarm.minute.val = ALARM_UNDEFINED;
    alarm.hour.val = ALARM_UNDEFINED;
  }
}

// Turn piezo buzzer on or off
void setBuzzer(uint8_t x) {
  digitalWrite(BUZZER_PIN, x); 
}

void setupDCF77_Uhr() {
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
  //pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP);  // nötig?
  uniButton = OneButton(
    UNI_BUTTON_PIN, // Input pin for the button
    true,           // Button is active LOW
    true            // Enable internal pull-up resistor
  );
  uniButton.setClickMs(SINGLE_CLICK_TIME);
  uniButton.setPressMs(PRESS_TIME);
  uniButton.attachClick(handleSingleClick); // Single Click event
  uniButton.attachDuringLongPress(handleDuringLongPress,
    &uniButton); // During Long Press events
  uniButton.attachLongPressStop(handleLongPressStop,
    &uniButton); // Long Press Stop event
  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(ON);
  delay(500);
  setBuzzer(OFF);
}
