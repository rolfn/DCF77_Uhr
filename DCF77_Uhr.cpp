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

void setup() {
  setupDCF77_Uhr();
}

#define PERIOD 1000L

void dummyUpdateDisplay() {
  uint8_t i;
  static uint8_t lastLevel = LOW;
  static uint16_t x_year = 2023;
  static uint8_t x_month = 8, x_day = 25, x_hour = 10, x_minute = 30, x_second = 0;

  //Serial.print("alarm.state: "); Serial.println(alarm.state);

  digitalWrite(DCF77_MONITOR_LED, lastLevel);
  lastLevel = !lastLevel;

  if (false) {

  } else { // dummy clock
    if (x_second == 59) {
      x_second = 0;
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
    } else {
      x_second++;
    }  
  }

  // Outputs independent of 'viewMode'
  showNumber(disp1, 1, x_hour);
  showNumber(disp1, 3, x_minute);
 
  // Outputs dependent on 'viewMode'
  if (viewMode != lastViewMode) {
    lastViewMode = viewMode;
    disp2.clearAll();
    disp3.clearAll();
  }
  switch (viewMode) {
    case VIEW_SEC:
      showNumber(disp2, 1, x_day, true);
      showNumber(disp2, 3, x_month, true);
      showNumber(disp3, 3, x_second);
      break;
    case VIEW_DATE:
      showNumber(disp2, 1, x_day, true);
      showNumber(disp2, 3, x_month, true);
      showNumber(disp3, 1, x_year / 100);
      showNumber(disp3, 3, x_year % 100);
      break;
    case VIEW_QTY:
      i = ARRAYSIZE(sync.quality) - 4;
      showNumber(disp2, 1, sync.quality[i++], true);
      showNumber(disp2, 3, sync.quality[i++], true);
      showNumber(disp3, 1, sync.quality[i++], true);
      showNumber(disp3, 3, sync.quality[i],   true);
      break;
    case VIEW_VERSION:
      showNumber(disp2, 1, DCF77_UHR_MAJOR_VERSION, true);
      showNumber(disp2, 3, DCF77_UHR_MINOR_VERSION, true);
      showNumber(disp3, 1, DCF77_UHR_PATCH_VERSION);
      break;
    default:
      break;
  }
  refreshDisplays();
}

void loop() {
  uniButton.tick();    // Call is often required
  updateBuzzerCycle(); // Call is often required

  if (millis() % PERIOD == 0) {
    dummyUpdateDisplay();
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
