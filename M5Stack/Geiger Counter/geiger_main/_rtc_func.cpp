#include "_rtc_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>

//RTC_TimeTypeDef RTCtime_Now;
//RTC_DateTypeDef RTCdate_Now;

int hours, minutes, seconds, days, months, years;

void init_rtc (void) {
  auto dt = M5.Rtc.getDateTime();
  //M5.Rtc.GetTime(&RTCtime_Now);
  //M5.Rtc.GetDate(&RTCdate_Now);
}

void rtc_set_time_date (int h, int mi, int s, int d, int m, int y) {
  //                      YYYY  MM  DD      hh  mm  ss
  M5.Rtc.setDateTime( { { y, m, d }, { h, mi, s } } );
}

void rtc_update (void) {
  auto dt = M5.Rtc.getDateTime();
  hours = dt.time.hours;
  minutes = dt.time.minutes;
  seconds = dt.time.seconds;
  days = dt.date.date;
  months = dt.date.month;
  years = dt.date.year;
}
