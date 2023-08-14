/**
 * DCF77_Uhr.h
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#ifndef DCF77_UHR_H
#define DCF77_UHR_H

#include <Arduino.h>
#include <Adafruit_i2c_7seg_LED.h>
#include <dcf77.h> // https://github.com/udoklein/dcf77
/*
http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
https://github.com/mathertel/OneButton
*/
#include <OneButton.h>

#define DCF77_UHR_MAJOR_VERSION 1
#define DCF77_UHR_MINOR_VERSION 0
#define DCF77_UHR_PATCH_VERSION 0

#define DCF77_UHR_VERSION_STRING \
   (EXPAND_THEN_STRINGIFY(DCF77_UHR_MAJOR_VERSION) "." \
    EXPAND_THEN_STRINGIFY(DCF77_UHR_MINOR_VERSION) "." \
    EXPAND_THEN_STRINGIFY(DCF77_UHR_PATCH_VERSION))

#define VIEW_SEC        0 // 14.08.__59
#define VIEW_DATE       1 // 14.08.2023
#define VIEW_QTY        2 // ___47_____

#define DCF77_MONITOR_LED 12 // PB4
//#define DCF77_MONITOR_LED LED_BUILTIN
#define DCF77_SAMPLE_PIN 13  // PB5 (???)
#define DCF77_INVERTED_SAMPLES 1

#define ALARM_BCD1 14 // PC0
#define ALARM_BCD2 15 // PC1
#define ALARM_BCD4 16 // PC2
#define ALARM_BCD8 17 // PC3
#define ALARM_BCD_PORT PORTC // Pins 14, 15, 16, 17

#define ALARM1_HOUR_HI_PIN   2 // PD2
#define ALARM1_HOUR_LO_PIN   3 // PD3
#define ALARM1_MINUTE_HI_PIN 4 // PD4
#define ALARM1_MINUTE_LO_PIN 5 // PD5

#define ALARM2_HOUR_HI_PIN   6 // PD6
#define ALARM2_HOUR_LO_PIN   7 // PD7
#define ALARM2_MINUTE_HI_PIN 8 // PB0
#define ALARM2_MINUTE_LO_PIN 9 // PB1

#define ALARM_MODE_DISABLED 0
#define ALARM_MODE_1        1
#define ALARM_MODE_2        2
#define ALARM_MODE_PIN A6
#define MULTI_BUTTON_PIN 10 // PB2
#define ALARM_BUZZER_PIN 11 // PB3

#define ALARM_VAL_UNDEFINED 0xff

#define LEVEL_20_PERCENT 205 // 1023 * 20 / 100 
#define LEVEL_80_PERCENT 818 // 1023 * 80 / 100
#define AREF 5.0

#define DISP1_ADR 0x00 // --> 0x70
#define DISP2_ADR 0x01 // --> 0x71
#define DISP3_ADR 0x02 // --> 0x72

typedef struct {
  BCD::bcd_t hour;   // 0..23
  BCD::bcd_t minute; // 0..59
  uint8_t mode;
  bool active;
} alarm_time_t;

extern alarm_time_t alarm;
extern uint8_t viewMode;
extern void updateAlarmSettings();
extern OneButton btn;
extern void setupDCF77_Uhr();

#endif

