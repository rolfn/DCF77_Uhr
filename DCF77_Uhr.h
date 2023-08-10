/**
 * DCF77_Uhr.h
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#ifndef DCF77_UHR_H
#define DCF77_UHR_H

#define DCF77_MONITOR_LED PB4
//#define DCF77_MONITOR_LED LED_BUILTIN
#define DCF77_SAMPLE_PIN PB5 // ???
//#define DCF77_INVERTED_SAMPLES 1

#define BCD1 PC0
#define BCD2 PC1
#define BCD4 PC2
#define BCD8 PC3

#define ALARM1_HOUR_HI   PD2
#define ALARM1_HOUR_LO   PD3
#define ALARM1_MINUTE_HI PD4
#define ALARM1_MINUTE_LO PD5

#define ALARM2_HOUR_HI   PD6
#define ALARM2_HOUR_LO   PD7
#define ALARM2_MINUTE_HI PB0
#define ALARM2_MINUTE_LO PB1

#define ALARM_MODE_PIN A6
#define ALARM_MODE_DISABLED 0
#define ALARM_MODE_1 1
#define ALARM_MODE_2 2
#define ALARM_OFF_PIN PB2

#define LEVEL_20_PERCENT 205 // 1023 * 20 / 100 
#define LEVEL_80_PERCENT 818 // 1023 * 80 / 100
#define AREF 5.0

#endif

