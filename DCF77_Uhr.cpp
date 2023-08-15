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
  lastLevel = !lastLevel;
  digitalWrite(DCF77_MONITOR_LED, lastLevel);
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

  switch (viewMode) {
    case VIEW_SEC:
      disp3.setDigit(DIGIT_4, VIEW_SEC);
      break;
    case VIEW_DATE:
      disp3.setDigit(DIGIT_4, VIEW_DATE);
      break;
    case VIEW_QTY:
      disp3.setDigit(DIGIT_4, VIEW_QTY);
  }

}

void loop() {
  uniButton.tick();
  //updateAlarmSettings(); // TODO: only once per second
  //updateModeSettings(); // TODO: only once per second

  if((unsigned long)(millis() - time_now) > PERIOD) {
    time_now = millis();
    myPeriod();
  }
  
  delay(10);

  /*

  // turn the LED on (HIGH is the voltage level)
  digitalWrite(DCF77_MONITOR_LED, HIGH);

  disp1.setPoint(POINT_UPPER_LEFT);
  disp1.clearPoint(POINT_LOWER_LEFT);

  xxx = 0;

  disp1.setDigit(DIGIT_1, ++xxx);
  delay(300);
  disp1.setDigit(DIGIT_2, ++xxx);
  delay(300);
  disp1.setDigit(DIGIT_3, ++xxx);
  delay(300);
  disp1.setDigit(DIGIT_4, ++xxx);
  delay(300);

  disp2.setDigit(DIGIT_1, ++xxx, true);
  delay(300);
  disp2.setDigit(DIGIT_2, ++xxx, true);
  delay(300);
  disp2.setDigit(DIGIT_3, ++xxx, true);
  delay(300);
  disp2.setDigit(DIGIT_4, ++xxx, true);
  delay(300);

  disp3.setDigit(DIGIT_1, ++xxx, true);
  delay(300);
  disp3.setDigit(DIGIT_2, ++xxx, true);
  delay(300);
  disp3.setDigit(DIGIT_3, ++xxx, true);
  delay(300);
  disp3.setDigit(DIGIT_4, ++xxx, true);
  delay(300);

  delay(3500);

  // turn the LED off by making the voltage LOW
  digitalWrite(DCF77_MONITOR_LED, LOW);

  disp1.clearPoint(POINT_UPPER_LEFT);
  disp1.setPoint(POINT_LOWER_LEFT);

  disp1.clearPosition(DIGIT_1);
  delay(300);
  disp1.clearPosition(DIGIT_2);
  delay(300);
  disp1.clearPosition(DIGIT_3);
  delay(300);
  disp1.clearPosition(DIGIT_4);
  delay(300);

  disp2.clearPosition(DIGIT_1);
  delay(300);
  disp2.clearPosition(DIGIT_2);
  delay(300);
  disp2.clearPosition(DIGIT_3);
  delay(300);
  disp2.clearPosition(DIGIT_4);
  delay(300);

  disp3.clearPosition(DIGIT_1);
  delay(300);
  disp3.clearPosition(DIGIT_2);
  delay(300);
  disp3.clearPosition(DIGIT_3);
  delay(300);
  disp3.clearPosition(DIGIT_4);
  delay(300);

  // disp1.clearAll();

  // wait for a second
  delay(500);

  Serial.print(millis());
  Serial.print("   ");
  Serial.print(DCF77_UHR_VERSION_STRING);
  Serial.print("   ");
  Serial.print(alarm.mode);
  Serial.println();
  
  */
}
