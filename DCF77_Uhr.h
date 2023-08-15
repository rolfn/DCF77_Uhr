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

enum views {
  VIEW_UNDEFINED,
  VIEW_SEC,  // 14.08.__59
  VIEW_DATE, // 14.08.2023
  VIEW_QTY   // ___47_____
};

enum alarmModes { ALARM_DISABLED, ALARM_1, ALARM_2 };

#define DCF77_MONITOR_LED 12 // PB4
//#define DCF77_MONITOR_LED LED_BUILTIN
#define DCF77_SAMPLE_PIN 13  // PB5 (???)
#define DCF77_INVERTED_SAMPLES 1

typedef struct {
  BCD::bcd_t hour;   // 0..23
  BCD::bcd_t minute; // 0..59
  unsigned long period; // useful?
  alarmModes mode;
  bool active;
} alarm_time_t;

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

#define ALARM_MODE_PIN A6
#define UNI_BUTTON_PIN 10 // PB2
#define ALARM_BUZZER_PIN 11 // PB3

#define ALARM_UNDEFINED 0xff

#define SYNC_HOUR 2
#define SYNC_MINUTE 30

#define LEVEL_20_PERCENT 205 // 1023 * 20 / 100 
#define LEVEL_80_PERCENT 818 // 1023 * 80 / 100
#define AREF 5.0

#define DISP1_ADR 0x00 // --> 0x70
#define DISP2_ADR 0x01 // --> 0x71
#define DISP3_ADR 0x02 // --> 0x72

extern Adafruit_7Seg disp1;
extern Adafruit_7Seg disp2;
extern Adafruit_7Seg disp3;
extern alarm_time_t alarm;
extern alarm_time_t sync;
extern views viewMode, lastViewMode;
extern void updateAlarmSettings();
extern OneButton uniButton;
extern void setupDCF77_Uhr();

#endif

