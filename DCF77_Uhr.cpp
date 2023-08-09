/**
 * DCF77_Uhr.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 *
 */

#include <Arduino.h>
#include "DCF77_Uhr.h"
#include <Adafruit_i2c_7seg_LED.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

#define DCF77_MONITOR_LED 12
//#define DCF77_MONITOR_LED LED_BUILTIN

#define ALARM_MODE_PIN A6
#define ALARM_OFF 0
#define ALARM_1 1
#define ALARM_2 2
#define LEVEL_20_PERCENT 204 // 1023 * 0.2 
#define LEVEL_80_PERCENT 818 // 1023 * 0.8
#define AREF 5.0

int rawADC = 0;
int alarm_mode;
float val = 0.0;

Adafruit_7Seg DISP1;
Adafruit_7Seg DISP2;
Adafruit_7Seg DISP3;

int getAlarmMode();

int getAlarmMode() {
  int rawADC = analogRead(ALARM_MODE_PIN);
  //float val = ((float) rawADC  + 0.5 ) / 1024.0 * AREF;
  if (rawADC < LEVEL_20_PERCENT) return ALARM_OFF;
  else if (rawADC > LEVEL_80_PERCENT) return ALARM_2;
  else return ALARM_1;
}

uint8_t led;

void setup() {
  analogReference(DEFAULT);
  // initialize LED digital pin as an output.
  pinMode(DCF77_MONITOR_LED, OUTPUT);
  Serial.begin(9600);
  while (!Serial); // Leonardo: wait for serial monitor
  Serial.println(__FILE__);
  DISP1.begin(0x00); DISP2.begin(0x01); DISP3.begin(0x02);
  DISP1.setBrightness(16); // 0..16
  DISP2.setBrightness(2); // 0..16
  DISP3.setBrightness(2); // 0..16
  DISP1.setPoint(COLON); DISP2.setPoint(COLON); DISP3.setPoint(COLON);
}

void loop() {
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(DCF77_MONITOR_LED, HIGH);
  
  DISP1.setPoint(POINT_UPPER_LEFT);
  DISP1.clearPoint(POINT_LOWER_LEFT);

  DISP1.setDigit(DIGIT_1, 0); delay(500);
  DISP1.setDigit(DIGIT_2, 1); delay(500);
  DISP1.setDigit(DIGIT_3, 2); delay(500);
  DISP1.setDigit(DIGIT_4, 3); delay(500);

  DISP2.setDigit(DIGIT_1, 4, true); delay(500);
  DISP2.setDigit(DIGIT_2, 5, true); delay(500);
  DISP2.setDigit(DIGIT_3, 6, true); delay(500);
  DISP2.setDigit(DIGIT_4, 7, true); delay(500);

  DISP3.setDigit(DIGIT_1, 8, true); delay(500);
  DISP3.setDigit(DIGIT_2, 9, true); delay(500);
  DISP3.setDigit(DIGIT_3, 10, true); delay(500);
  DISP3.setDigit(DIGIT_4, 11, true); delay(500);

  // turn the LED off by making the voltage LOW
  digitalWrite(DCF77_MONITOR_LED, LOW);

  DISP1.clearPoint(POINT_UPPER_LEFT);
  DISP1.setPoint(POINT_LOWER_LEFT);

  DISP1.clearPosition(DIGIT_1); delay(500);
  DISP1.clearPosition(DIGIT_2); delay(500);
  DISP1.clearPosition(DIGIT_3); delay(500);
  DISP1.clearPosition(DIGIT_4); delay(500);
  
  DISP2.clearPosition(DIGIT_1); delay(500);
  DISP2.clearPosition(DIGIT_2); delay(500);
  DISP2.clearPosition(DIGIT_3); delay(500);
  DISP2.clearPosition(DIGIT_4); delay(500);

  DISP3.clearPosition(DIGIT_1); delay(500);
  DISP3.clearPosition(DIGIT_2); delay(500);
  DISP3.clearPosition(DIGIT_3); delay(500);
  DISP3.clearPosition(DIGIT_4); delay(500);
  
  //DISP1.clearAll();

   // wait for a second
  delay(1000);

  rawADC = analogRead(ALARM_MODE_PIN);
  val = ((float) rawADC  + 0.5 ) / 1024.0 * AREF;
  alarm_mode = getAlarmMode();
  Serial.print(millis());
  Serial.print("   ");
  Serial.print(alarm_mode);
  Serial.print("   ");
  Serial.println(val);
}
