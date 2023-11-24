#include "_rtc_func.h"
#include "_sntp_func.h"
#include "_jokes_func.h"
#include <TimeLib.h>
#include <Wire.h>
#include "RTClib.h"

extern bool time_obtained_sntp;
bool rtc_fail_init = false;
bool rtc_fail_lost_power = false;
bool rtc_ok = false;

RTC_DS3231 rtc;

time_t time_provider() {
  return rtc.now().unixtime();
}

void rtc_init (void) {
  //Wire.begin();
  if (!rtc.begin()) {
    rtc_fail_init = true;
    rtc_fail_lost_power = true;
  }
  else {
    rtc_fail_init = false;
    if (rtc.lostPower()) {
      if (time_obtained_sntp) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        //rtc.adjust(DateTime(sntp_get_epoch()));
      }
      else {
        rtc_fail_lost_power = true;
      }
    }
    else {
      setSyncProvider(time_provider);
      setSyncInterval(5);
      rtc_ok = true;
      rtc_fail_init = false;
      rtc_fail_lost_power = false;
    }
  }
}

void rtc_update (void) {
}
