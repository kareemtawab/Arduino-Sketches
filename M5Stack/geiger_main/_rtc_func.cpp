#include "_rtc_func.h"
#include <M5Core2.h>

RTC_TimeTypeDef RTCtime_Now;
RTC_DateTypeDef RTCdate_Now;

void init_rtc (void) {
  M5.Rtc.GetTime(&RTCtime_Now);
  M5.Rtc.GetDate(&RTCdate_Now);
}

void rtc_set_time (int h, int m, int s) {
  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Hours   = h;    //Set the specific time of the real-time clock structure.
  TimeStruct.Minutes = m;
  TimeStruct.Seconds = s;
  M5.Rtc.SetTime(&TimeStruct);    //Writes the set time to the real-time clock.
}

void rtc_set_date (int w, int m, int d, int y) {
  RTC_DateTypeDef DateStruct;
  DateStruct.WeekDay = w;
  DateStruct.Month   = m;
  DateStruct.Date    = d;
  DateStruct.Year    = y;
  M5.Rtc.SetDate(&DateStruct);
}

void rtc_print_serial (void) {
  char Strbuff[64];
  sprintf(Strbuff, "Time: %02d:%02d:%02d, Date: %02d/%02d/%04d", RTCtime_Now.Hours, RTCtime_Now.Minutes, RTCtime_Now.Seconds, RTCdate_Now.Date, RTCdate_Now.Month, RTCdate_Now.Year);
  Serial.println(Strbuff);
}

void rtc_run (void) {
  M5.Rtc.GetTime(&RTCtime_Now);
  M5.Rtc.GetDate(&RTCdate_Now);
}
