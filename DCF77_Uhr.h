/**
 * DCF77_Uhr.h
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#ifndef DCF77_UHR_H
#define DCF77_UHR_H

#define DCF77_MONITOR_LED 12 // PB4
//#define DCF77_MONITOR_LED LED_BUILTIN
#define DCF77_SAMPLE_PIN 13  // PB5 (???)
//#define DCF77_INVERTED_SAMPLES 1

#define BCD1 14 // PC0
#define BCD2 15 // PC1
#define BCD4 16 // PC2
#define BCD8 17 // PC3

#define ALARM1_HOUR_HI_PIN   2 // PD2
#define ALARM1_HOUR_LO_PIN   3 // PD3
#define ALARM1_MINUTE_HI_PIN 4 // PD4
#define ALARM1_MINUTE_LO_PIN 5 // PD5

#define ALARM2_HOUR_HI_PIN   6 // PD6
#define ALARM2_HOUR_LO_PIN   7 // PD7
#define ALARM2_MINUTE_HI_PIN 8 // PB0
#define ALARM2_MINUTE_LO_PIN 9 // PB1

#define ALARM_MODE_PIN A6
#define ALARM_MODE_DISABLED 0
#define ALARM_MODE_1 1
#define ALARM_MODE_2 2
#define ALARM_OFF_PIN 10 // PB2

#define LEVEL_20_PERCENT 205 // 1023 * 20 / 100 
#define LEVEL_80_PERCENT 818 // 1023 * 80 / 100
#define AREF 5.0

#define DISP1_ADR 0x00 // --> 0x70
#define DISP2_ADR 0x01 // --> 0x71
#define DISP3_ADR 0x02 // --> 0x72

#endif

