#include "def_data.c"
#include "_rtc_func.h"
#include <I2C_BM8563.h>
#include <UnixTime.h>
#include <stdint.h>

int hours, minutes, seconds, days, months, years;
uint32_t unix;
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
UnixTime stamp(TIMEZONE);

void init_rtc(void) {
  rtc.begin();
}

void rtc_update(void) {
  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;

  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  hours = timeStruct.hours;
  minutes = timeStruct.minutes;
  seconds = timeStruct.seconds;
  days = dateStruct.date;
  months = dateStruct.month;
  years = dateStruct.year;

  stamp.setDateTime(years, months, days, hours, minutes, seconds);
  unix = rtc_get_unix();
}

int rtc_get_day_phase(void) {
  if (hours >= 5 && hours < 10) {
    return 1;
  } else if (hours >= 10 && hours < 15) {
    return 2;
  } else if (hours >= 15 && hours < 20) {
    return 3;
  } else {
    return 0;
  }
}

void rtc_set_time_date(int h, int mi, int s, int d, int m, int y, int t) {
  I2C_BM8563_DateTypeDef dateStruct;
  dateStruct.date = d;
  dateStruct.month = m;
  dateStruct.year = y;
  rtc.setDate(&dateStruct);

  I2C_BM8563_TimeTypeDef timeStruct;
  timeStruct.hours = h;
  timeStruct.minutes = mi;
  timeStruct.seconds = s + t * 3600;
  rtc.setTime(&timeStruct);
}

uint32_t rtc_get_unix(void) {
  return stamp.getUnix();
}