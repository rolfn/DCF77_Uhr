/*
 * DCF77_Uhr.cpp
 * 
 * A radio clock (DCF77) with two alarm times and date display.
 * See: https://github.com/rolfn/DCF77_Uhr
 * 
 * Copyright:  Rolf Niepraschk
 * License: MIT
 *
 */

#include "DCF77_Uhr.h"

uint8_t x_second = 0, x_minute = 0, x_hour = 0;

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  while (!Serial)
    ;
  setupDCF77_Uhr();
}

#define PERIOD 1000
uint8_t lastLevel = LOW;

void longPeriod() {  // TODO: move to DCF77_utils.cpp
  uint8_t len;
  uint8_t i;
  updateAlarmSettings();
  handleSnooze();
  ///Serial.print("alarm.state: "); Serial.println(alarm.state);
  Serial.print("alarm.mode: "); Serial.println(alarm.mode);
  digitalWrite(DCF77_MONITOR_LED, lastLevel);
  if (lastLevel == LOW) lastLevel = HIGH; else lastLevel = LOW;

  if (x_second == 59) {
    x_second = 0;
  } else {
    x_second++;
  }

  if (x_minute == 59) {
    x_minute = 0;
    if (x_hour == 23) {
      x_hour = 0;
    } else {
      x_hour++;
    }
  } else {
    x_minute++;
  }
  if (viewMode != lastViewMode) {
    lastViewMode = viewMode;
    disp2.clearAll();
    disp3.clearAll();
    switch (viewMode) {
      case VIEW_SEC:
        disp2.setDigit(DIGIT_1, 1);
        disp2.setDigit(DIGIT_2, 5, true);
        disp2.setDigit(DIGIT_3, 0);
        disp2.setDigit(DIGIT_4, 8, true);
        //disp3.setPoint(COLON);
        break;
      case VIEW_DATE:
        disp2.setDigit(DIGIT_1, 1);
        disp2.setDigit(DIGIT_2, 5, true);
        disp2.setDigit(DIGIT_3, 0);
        disp2.setDigit(DIGIT_4, 8, true);
        disp3.setDigit(DIGIT_1, 2);
        disp3.setDigit(DIGIT_2, 0);
        disp3.setDigit(DIGIT_3, 2);
        disp3.setDigit(DIGIT_4, 3);
        break;
      case VIEW_QTY:
        len = sizeof(sync.quality) / sizeof(sync.quality[0]);
        i = len - 4;
        disp2.setDigit(DIGIT_1, sync.quality[i] / 10);
        disp2.setDigit(DIGIT_2, sync.quality[i++] % 10, true);
        disp2.setDigit(DIGIT_3, sync.quality[i] / 10);
        disp2.setDigit(DIGIT_4, sync.quality[i++] % 10, true);
        disp3.setDigit(DIGIT_1, sync.quality[i] / 10);
        disp3.setDigit(DIGIT_2, sync.quality[i++] % 10, true);
        disp3.setDigit(DIGIT_3, sync.quality[i] / 10);
        disp3.setDigit(DIGIT_4, sync.quality[i] % 10);
        break;
      case VIEW_VERSION:
        //Serial.print("viewMode: "); Serial.println(viewMode);
        disp2.setDigit(DIGIT_1, DCF77_UHR_MAJOR_VERSION / 10);
        disp2.setDigit(DIGIT_2, DCF77_UHR_MAJOR_VERSION % 10, true);
        disp2.setDigit(DIGIT_3, DCF77_UHR_MINOR_VERSION / 10);
        disp2.setDigit(DIGIT_4, DCF77_UHR_MINOR_VERSION % 10, true);
        disp3.setDigit(DIGIT_1, DCF77_UHR_PATCH_VERSION / 10);
        disp3.setDigit(DIGIT_2, DCF77_UHR_PATCH_VERSION % 10);
        break;
      default:
        break;
    }
  }

  disp1.setDigit(DIGIT_1, x_hour / 10);
  disp1.setDigit(DIGIT_2, x_hour % 10);
  disp1.setDigit(DIGIT_3, x_minute / 10);
  disp1.setDigit(DIGIT_4, x_minute % 10);
  if (viewMode == VIEW_SEC) {
    disp3.setDigit(DIGIT_3, x_second / 10);
    disp3.setDigit(DIGIT_4, x_second % 10);
  }
  
  disp1.sendLed();
  disp2.sendLed();
  disp3.sendLed();
}

void loop() {
  uniButton.tick();
  handleBuzzer();
  handleSnooze();

  if (periodTimer.cycleTrigger(PERIOD)) {  // Gets called every PERIOD once
    longPeriod();
  }

  /*

  Serial.print(millis());
  Serial.print("   ");
  Serial.print(DCF77_UHR_VERSION_STRING);
  Serial.print("   ");
  Serial.print(alarm.mode);
  Serial.println();

  */
}
