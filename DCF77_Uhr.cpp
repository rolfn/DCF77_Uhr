#include <Adafruit_i2c_7seg_LED.h>

/**
 * DCF77_Uhr.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#include <Adafruit_i2c_7seg_LED.h>
#include <Arduino.h>

#include "DCF77_Uhr.h"

int rawADC = 0;
int alarm_mode;
float val = 0.0;

Adafruit_7Seg disp1;
Adafruit_7Seg disp2;
Adafruit_7Seg disp3;

int getAlarmMode();

int getAlarmMode() {
  int rawADC = analogRead(ALARM_MODE_PIN);
  // float val = ((float) rawADC  + 0.5) / 1024.0 * AREF;
  if (rawADC < LEVEL_20_PERCENT)
    return ALARM_MODE_DISABLED;
  else if (rawADC > LEVEL_80_PERCENT)
    return ALARM_MODE_2;
  else
    return ALARM_MODE_1;
}

int xxx;

static const int ALARM1_SEL_PINS[] = {  // ?
  ALARM1_HOUR_HI_PIN, ALARM1_HOUR_LO_PIN, ALARM1_MINUTE_HI_PIN,
  ALARM1_MINUTE_LO_PIN
};

static const int ALARM2_SEL_PINS[] = {  // ?
  ALARM2_HOUR_HI_PIN, ALARM2_HOUR_LO_PIN, ALARM2_MINUTE_HI_PIN,
  ALARM2_MINUTE_LO_PIN
};

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println(__FILE__);
  disp1.begin(DISP1_ADR);
  disp2.begin(DISP2_ADR);
  disp3.begin(DISP3_ADR);
  disp1.setBrightness(16);  // 0..16
  disp2.setBrightness(3);   // 0..16
  disp3.setBrightness(3);   // 0..16
  disp1.setPoint(COLON);
  disp2.setPoint(COLON);
  disp3.setPoint(COLON);
  //
  pinMode(BCD1, INPUT_PULLUP);
  pinMode(BCD2, INPUT_PULLUP);
  pinMode(BCD4, INPUT_PULLUP);
  pinMode(BCD8, INPUT_PULLUP);
  pinMode(ALARM_OFF_PIN, INPUT_PULLUP);
  DDRD = DDRD | B11111100;             // PD2..PD7 as output (BCD selection)
  DDRB = DDRB | B00000011;             // PB0..PB1 as output (BCD selection)
  pinMode(DCF77_MONITOR_LED, OUTPUT);  // nötig?
  //pinMode(DCF77_SAMPLE_PIN, INPUT_PULLUP);  // nötig?
}

void loop() {
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

  alarm_mode = getAlarmMode();

  Serial.print(millis());
  Serial.print("   ");
  Serial.print(alarm_mode);
  Serial.print("   ");
  Serial.println(val);
}
