#include "_rtc_func.h"
#include "Arduino.h"
#include "uRTCLib.h"

uRTCLib rtc;

void init_rtc(void) {
  URTCLIB_WIRE.begin();
  rtc.set_rtc_address(0x68);
  rtc.set_model(URTCLIB_MODEL_DS3232);
  rtc.refresh();
  rtc.set_12hour_mode(true);
  if (rtc.enableBattery()) {
    Serial.println("Battery activated correctly.");
  } else {
    Serial.println("ERROR activating battery.");
  }
  if (rtc.getEOSCFlag())
    Serial.println(F("Oscillator will not use VBAT when VCC cuts off. Time will not increment without VCC!"));
  else
    Serial.println(F("Oscillator will use VBAT when VCC cuts off."));

  Serial.print("Lost power status: ");
  if (rtc.lostPower()) {
    Serial.print("POWER FAILED. Clearing flag...");
    rtc.lostPowerClear();
    Serial.println(" done.");
  } else {
    Serial.println("POWER OK");
  }
  rtc.sqwgSetMode(URTCLIB_SQWG_1H);
}


void rtc_update(void) {
  rtc.refresh();
  // Serial.print("RTC DateTime: ");

  // Serial.print(rtc.day());
  // Serial.print('/');
  // Serial.print(rtc.month());
  // Serial.print('/');
  // Serial.print(rtc.year());
  // Serial.print(' ');

  // Serial.print(rtc.hour());
  // Serial.print(':');
  // Serial.print(rtc.minute());
  // Serial.print(':');
  // Serial.print(rtc.second());
  // if (rtc.hourModeAndAmPm() == 1)
  //   Serial.print(" AM");
  // if (rtc.hourModeAndAmPm() == 2)
  //   Serial.print(" PM");

  // Serial.print(" DOW: ");
  // Serial.print(rtc.dayOfWeek());

  // Serial.print(" - Temp: ");
  // Serial.println((float)rtc.temp() / 100);
}

void rtc_set_time_date(int h, int mi, int s, int dow, int d, int m, int y) {
  rtc.set(s, mi, h, dow, d, m, y);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}

int rtc_get_hour(void) {
  return rtc.hour();
}

int rtc_get_minute(void) {
  return rtc.minute();
}

char* rtc_get_AM_PM(void) {
  if (rtc.hourModeAndAmPm() == 1) return "AM";
  else if (rtc.hourModeAndAmPm() == 2) return "PM";
  else return 0;
}

int rtc_get_day(void) {
  return rtc.day();
}

int rtc_get_month(void) {
  return rtc.month();
}

int rtc_get_year(void) {
  return rtc.year();
}

float rtc_get_temp(void) {
  return (float)rtc.temp() / 100;
}

uint32_t rtc_get_unix(void) {
}