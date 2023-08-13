/**
 * DCF77_Uhr_utils.cpp
 * Copyright:  Rolf Niepraschk <Rolf.Niepraschk AT gmx.de>
 * https://github.com/rolfn/DCF77_Uhr
 */

#include <Arduino.h>
#include "DCF77_Uhr.h"

alarm_time_t alarm = {
  .hour =   { .val = ALARM_VAL_UNDEFINED },
  .minute = { .val = ALARM_VAL_UNDEFINED }, 
  .mode = ALARM_MODE_DISABLED
};

void getAlarmMode() {
  /*

    Determines the alarm mode as folows:
    
          o  VCC
          |
          o
        \
         \   Alarm Off / On
          o
          |
          o-----. 
          |     |
         .-.    o
         | |  \
         | |   \  Alarm 1 / Alarm 2
         '-'    o
          |     |
          o-----o-----------> ALARM_MODE_PIN
          |
         .-.
         | |
         | |
         '-'
          |
         --- GND   

  */
  int rawADC = analogRead(ALARM_MODE_PIN);
  // float val = ((float) rawADC  + 0.5) / 1024.0 * AREF;
  if (rawADC < LEVEL_20_PERCENT)
    alarm.mode = ALARM_MODE_DISABLED;
  else if (rawADC > LEVEL_80_PERCENT)
    alarm.mode = ALARM_MODE_2;
  else
    alarm.mode = ALARM_MODE_1;
}

uint8_t bcdRead(uint8_t pos) {
  uint8_t val;
  digitalWrite(pos, LOW); // selecting a specific BCD switch
  delayMicroseconds(2); // necessary?
  val = ALARM_BCD_PORT & 0x0f; // mask the 4 high order bits
  digitalWrite(pos, HIGH); // deselecting a specific BCD switch
  return val;
}
   
void getAlarmTime() {
  if (alarm.mode == ALARM_MODE_1) {
    alarm.minute.digit.lo = bcdRead(ALARM1_MINUTE_LO_PIN);
    alarm.minute.digit.hi = bcdRead(ALARM1_MINUTE_HI_PIN);
    alarm.hour.digit.lo = bcdRead(ALARM1_HOUR_LO_PIN);
    alarm.hour.digit.hi = bcdRead(ALARM1_HOUR_HI_PIN);    
  } else if (alarm.mode == ALARM_MODE_2) {
    alarm.minute.digit.lo = bcdRead(ALARM2_MINUTE_LO_PIN);
    alarm.minute.digit.hi = bcdRead(ALARM2_MINUTE_HI_PIN);
    alarm.hour.digit.lo = bcdRead(ALARM2_HOUR_LO_PIN);
    alarm.hour.digit.hi = bcdRead(ALARM2_HOUR_HI_PIN);  
  } else {// ALARM_MODE_DISABLED
    alarm.minute.val = ALARM_VAL_UNDEFINED;
    alarm.hour.val = ALARM_VAL_UNDEFINED;
  }
} 