#include "_rtc_func.h"
#include "_sntp_func.h"
#include <TimeLib.h>
#include <Wire.h>

bool time_obtained_rtc;
extern bool time_obtained_sntp;
DateTime now;

void rtc_init (void) {
  Wire.begin();
  now = RTC.now();
}

void rtc_update (void) {
}
