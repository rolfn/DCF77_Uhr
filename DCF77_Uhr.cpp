/**
 * DCF77_Uhr.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#include <Arduino.h>
#include "DCF77_Uhr.h"
#include <Adafruit_i2c_7seg_LED.h>

uint8_t rawADC = 0;
uint8_t alarm_mode;
float val = 0.0;

Adafruit_7Seg DISP1;
Adafruit_7Seg DISP2;
Adafruit_7Seg DISP3;

uint8_t getAlarmMode();

uint8_t getAlarmMode() {
  uint8_t rawADC = analogRead(ALARM_MODE_PIN);
  //float val = ((float) rawADC  + 0.5) / 1024.0 * AREF;
  if (rawADC < LEVEL_20_PERCENT) return ALARM_MODE_DISABLED;
  else if (rawADC > LEVEL_80_PERCENT) return ALARM_MODE_2;
  else return ALARM_MODE_1;
}

uint8_t xxx;

static const uint8_t ALARM1_SEL_PINS[] = { // ?
  ALARM1_HOUR_HI, ALARM1_HOUR_LO, ALARM1_MINUTE_HI, ALARM1_MINUTE_LO
};

static const uint8_t ALARM2_SEL_PINS[] = { // ?
  ALARM2_HOUR_HI, ALARM2_HOUR_LO, ALARM2_MINUTE_HI, ALARM2_MINUTE_LO
};

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  while (!Serial); 
  Serial.println(__FILE__);
  DISP1.begin(0x00); DISP2.begin(0x01); DISP3.begin(0x02);
  DISP1.setBrightness(16); // 0..16
  DISP2.setBrightness(3); // 0..16
  DISP3.setBrightness(3); // 0..16
  DISP1.setPoint(COLON); DISP2.setPoint(COLON); DISP3.setPoint(COLON);
  //
  pinMode(BCD1, INPUT_PULLUP);
  pinMode(BCD2, INPUT_PULLUP);
  pinMode(BCD4, INPUT_PULLUP);
  pinMode(BCD8, INPUT_PULLUP); 
  pinMode(ALARM_OFF_PIN, INPUT_PULLUP); 
  DDRD = DDRD | B11111100; // PD2..PD7 as output
  DDRB = DDRB | B00000011; // PB0..PB1 as output
  pinMode(DCF77_MONITOR_LED, OUTPUT);
  pinMode(DCF77_SAMPLE_PIN
}

void loop() {
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(DCF77_MONITOR_LED, HIGH);
  
  DISP1.setPoint(POINT_UPPER_LEFT);
  DISP1.clearPoint(POINT_LOWER_LEFT);

  xxx = 3;

  DISP1.setDigit(DIGIT_1, ++xxx); delay(500);
  DISP1.setDigit(DIGIT_2, ++xxx); delay(500);
  DISP1.setDigit(DIGIT_3, ++xxx); delay(500);
  DISP1.setDigit(DIGIT_4, ++xxx); delay(500);

  DISP2.setDigit(DIGIT_1, ++xxx, true); delay(500);
  DISP2.setDigit(DIGIT_2, ++xxx, true); delay(500);
  DISP2.setDigit(DIGIT_3, ++xxx, true); delay(500);
  DISP2.setDigit(DIGIT_4, ++xxx, true); delay(500);

  DISP3.setDigit(DIGIT_1, ++xxx, true); delay(500);
  DISP3.setDigit(DIGIT_2, ++xxx, true); delay(500);
  DISP3.setDigit(DIGIT_3, ++xxx, true); delay(500);
  DISP3.setDigit(DIGIT_4, ++xxx, true); delay(500);

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
