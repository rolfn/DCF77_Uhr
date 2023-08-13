/**
 * DCF77_Uhr_utils.h
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#ifndef DCF77_UHR_UTILS_H
#define DCF77_UHR_UTILS_H

#include <Arduino.h>
#include <Adafruit_i2c_7seg_LED.h>
#include <dcf77.h> // https://github.com/udoklein/dcf77
/*
http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
https://github.com/mathertel/OneButton
*/
#include <OneButton.h>

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

// ???

#endif

