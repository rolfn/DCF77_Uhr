/*
 * DCF77_Uhr.h
 * 
 * A radio clock (DCF77) with two alarm times and date display.
 * See: https://github.com/rolfn/DCF77_Uhr
 * 
 * Copyright:  Rolf Niepraschk
 * License: MIT
 *
 */
 
#ifndef DCF77_UHR_H
#define DCF77_UHR_H

#include <Adafruit_i2c_7seg_LED.h>
#include <Arduino.h>
#include <dcf77.h>  // https://github.com/udoklein/dcf77
#include <muTimer.h>
/*
http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
https://github.com/mathertel/OneButton
*/
#include <OneButton.h>

#define DCF77_UHR_MAJOR_VERSION 1
#define DCF77_UHR_MINOR_VERSION 4
#define DCF77_UHR_PATCH_VERSION 13

#define DCF77_UHR_VERSION_STRING                                             \
  (EXPAND_THEN_STRINGIFY(DCF77_UHR_MAJOR_VERSION) "." EXPAND_THEN_STRINGIFY( \
      DCF77_UHR_MINOR_VERSION) "." EXPAND_THEN_STRINGIFY(DCF77_UHR_PATCH_VERSION))

enum views {
  VIEW_UNDEFINED,
  VIEW_SEC,    // 14.08.  59
  VIEW_DATE,   // 14.08.2023
  VIEW_QTY,    // 49.43.48.50
  VIEW_VERSION // 01.03.15
};

enum alarmModes { UNDEFINED, ONE, TWO, DISABLED };
enum alarmStates { WAITING, SNOOZE, ACTIVE };

#define DCF77_MONITOR_LED 12  // PB4
// #define DCF77_MONITOR_LED LED_BUILTIN
#define DCF77_SAMPLE_PIN 13  // PB5 (???)
#define DCF77_INVERTED_SAMPLES 1

typedef struct {
  BCD::bcd_t hour;    // 0..23
  BCD::bcd_t minute;  // 0..59
  unsigned long snoozeStart;
  alarmModes mode;
  alarmStates state;
} alarm_time_t;

typedef struct {
  uint8_t hour;
  uint8_t minute;
  bool syncing;
  uint8_t quality[32];
} sync_t;

#define ALARM_BCD1 14          // PC0
#define ALARM_BCD2 15          // PC1
#define ALARM_BCD4 16          // PC2
#define ALARM_BCD8 17          // PC3
#define ALARM_BCD_PORT PORTC   // Pins 14, 15, 16, 17

#define ALARM1_HOUR_HI_PIN 2   // PD2
#define ALARM1_HOUR_LO_PIN 3   // PD3
#define ALARM1_MINUTE_HI_PIN 4 // PD4
#define ALARM1_MINUTE_LO_PIN 5 // PD5

#define ALARM2_HOUR_HI_PIN 6   // PD6
#define ALARM2_HOUR_LO_PIN 7   // PD7
#define ALARM2_MINUTE_HI_PIN 8 // PB0
#define ALARM2_MINUTE_LO_PIN 9 // PB1

#define ALARM_MODE_PIN A6
#define UNI_BUTTON_PIN 10      // PB2
#define BUZZER_PIN 11          // PB3

#define BUZZER_ON_TIME  250 // ms
#define BUZZER_OFF_TIME 150 // ms

#define ALARM_UNDEFINED 0xff
//#define ALARM_SNOOZE_MAX 300000L  // 300000 = 5 min
#define ALARM_SNOOZE_MAX 60000L  // 60000 = 1 min

#define SYNC_HOUR 2
#define SYNC_MINUTE 30
#define SYNC_MAX 1800000L        // 1800000 ms = 30 min

#define ADC_20_PERCENT 205  // 1023 * 20 / 100
#define ADC_80_PERCENT 818  // 1023 * 80 / 100
#define AREF 5.0            // 5V

#define DISP1_ADR 0x00  // i2c address: 0x70
#define DISP2_ADR 0x01  // i2c address: 0x71
#define DISP3_ADR 0x02  // i2c address: 0x72

#define SHORT_PRESS_TIME    50L // ms
#define MEDIUM_PRESS_TIME 1000L // ms
#define LONG_PRESS_TIME   3500L // ms

#define ON HIGH
#define OFF LOW

extern Adafruit_7Seg disp1;
extern Adafruit_7Seg disp2;
extern Adafruit_7Seg disp3;
extern alarm_time_t alarm;
extern sync_t sync;
extern views viewMode, lastViewMode;
extern alarmModes lastAlarmMode;
extern void longPeriod(void);
extern void updateAlarmSettings(void);
extern OneButton uniButton;
extern void setupDCF77_Uhr(void);
extern void setNormalMode(void);
extern void setSleepMode(void);
extern void setBuzzer(uint8_t x);
extern muTimer periodTimer;
extern muTimer buzzerTimer;
extern muTimer snoozeTimer;
extern void handleBuzzerCycle(void);
extern void handleSnooze(void);

#endif
