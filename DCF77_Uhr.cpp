/**
 * DCF77_Uhr.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#include "DCF77_Uhr.h"

uint8_t xxx;
uint8_t x_minute = 0;
uint8_t x_hour = 0;

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println(__FILE__);
  //
  setupDCF77_Uhr();
}

#define PERIOD 1000
unsigned long time_now = 0;
uint8_t lastLevel = LOW;

void myPeriod() {
  updateAlarmSettings();
  digitalWrite(DCF77_MONITOR_LED, lastLevel);
  if (lastLevel == LOW) {
    disp1.clearPoint(POINT_LOWER_LEFT);
    disp1.setPoint(POINT_UPPER_LEFT);
  } else {
    disp1.clearPoint(POINT_UPPER_LEFT);
    disp1.setPoint(POINT_LOWER_LEFT);
  }
  lastLevel = !lastLevel;
  if (x_minute < 59) x_minute++;
  else {
    x_minute = 0;
    x_hour++;
  }
  if (x_hour == 24) x_hour = 0;

  disp1.setDigit(DIGIT_1, x_hour / 10);
  disp1.setDigit(DIGIT_2, x_hour % 10);
  disp1.setDigit(DIGIT_3, x_minute / 10);
  disp1.setDigit(DIGIT_4, x_minute % 10);

  if (viewMode != lastViewMode) {
    disp2.clearAllDigits();
    disp3.clearAllDigits();
    switch (viewMode) {
      case VIEW_SEC:
        disp2.setDigit(DIGIT_1, 1);
        disp2.setDigit(DIGIT_2, 5, true);
        disp2.setDigit(DIGIT_3, 0);
        disp2.setDigit(DIGIT_4, 8, true);
        disp3.setDigit(DIGIT_3, 5);
        disp3.setDigit(DIGIT_4, 9);
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
        disp3.setDigit(DIGIT_1, 4);
        disp3.setDigit(DIGIT_2, 7);
    }
  }
  lastViewMode = viewMode;

}

void loop() {
  uniButton.tick();

  if((unsigned long)(millis() - time_now) > PERIOD) {
    time_now = millis();
    myPeriod();
  }
  
  delay(10);

  /*

  Serial.print(millis());
  Serial.print("   ");
  Serial.print(DCF77_UHR_VERSION_STRING);
  Serial.print("   ");
  Serial.print(alarm.mode);
  Serial.println();
  
  */
}
