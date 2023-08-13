/**
 * DCF77_Uhr_utils.h
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#ifndef DCF77_UHR_UTILS_H
#define DCF77_UHR_UTILS_H

typedef struct {
  BCD::bcd_t hour;   // 0..23
  BCD::bcd_t minute; // 0..59
  uint8_t mode;
} alarm_time_t;

extern alarm_time_t alarm;
void getAlarmMode();
void getAlarmTime();

// ???

#endif

