#include "_serial_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>
#include "_rtc_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_batt_led_vib_func.h"
#include "_eeprom_func.h"

//extern RTC_TimeTypeDef RTCtime_Now;
//extern RTC_DateTypeDef RTCdate_Now;
extern int hours, minutes, seconds, days, months, years;

void init_serial (void) {
  //Serial.begin(115200);
}

void power_print_serial (void) {
  char Str0[100];
  sprintf(Str0, "POWER  - Voltage: %.4f volts, %d percent", (float)batt_get_voltage(), batt_get_percent());
  Serial.println(Str0);
}

void rtc_print_serial (void) {
  char Str1[70];
  sprintf(Str1, "RTC    - Time: %02d:%02d:%02d, Date: %02d/%02d/%04d", hours, minutes, seconds, days, months, years);
  Serial.println(Str1);
}

void gps_print_serial (void) {
  char Str2[70];
  sprintf(Str2, "GPS    - Long.: %.4f, Lat.: %.4f, Alt.: %.1f, Sat.: %d", (float)gps_get_lat(), (float)gps_get_long(), (float)gps_get_alt(), gps_get_sat());
  Serial.println(Str2);
  sprintf(Str2, "GPS    - Map X Offset: %d, Map Y Offset: %d", gps_get_draw_x(), gps_get_draw_y());
  Serial.println(Str2);
}

void geiger_print_serial (void) {
  char Str2[70];
  sprintf(Str2, "GEIGER - CPM: %d, uSV/h: %.2f, mRem/h: %.1f, CPM/uSV/h: %d", geiger_getCPM(), (float)geiger_getSieverts(), (float)geiger_getRems(), eeprom_get_cpm_per_uSv_per_h());
  Serial.println(Str2);
}

void serial_print_all (void) {
  Serial.println("-------------------------------------------------------------");
  power_print_serial();
  rtc_print_serial();
  gps_print_serial();
  geiger_print_serial();
}
