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

void loop() {
  uniButton.tick();    // Call is often required
  updateBuzzerCycle(); // Call is often required

  if (millis() % PERIOD == 0) {
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
