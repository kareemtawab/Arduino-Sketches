#include "_serial_func.h"
#include "_rtc_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_batt_led_vib_func.h"
#include "_eeprom_func.h"
#include "_file_transfer_func.h"

extern int hours, minutes, seconds, days, months, years;
extern uint32_t unix;
extern const char *ft_server_ssid;
extern IPAddress ft_server_ip;

void init_serial(void) {
  Serial.begin(115200);
}

void system_print_serial(void) {
  Serial.printf("SYSTEM - Audio: %d, Lights: %d, Vibrate: %d, Logger: %d, Temperature: %4.1fC %5.1fF\n", eeprom_get_audio(), eeprom_get_lights(), eeprom_get_vibrate(), eeprom_get_log(), get_internal_temp_c(), get_internal_temp_f());
}

void power_print_serial(void) {
  Serial.printf("POWER  - VIN: %4.2fV, Voltage: %4.2fV, SOC: %03d%, Battery Current: %04dmA\n",
                (batt_get_vin_voltage() == -1 ? 0 : batt_get_vin_voltage()),
                (batt_get_voltage() == -1 ? 0 : batt_get_voltage()),
                (batt_get_percent() == -1 ? 0 : batt_get_percent()),
                (get_battery_current() == -1 ? 0 : get_battery_current()));
}

void rtc_print_serial(void) {
  Serial.printf("RTC    - Time: %02d:%02d:%02d, Date: %02d/%02d/%04d, Day Phase: %d, UNIX: %ld\n", hours, minutes, seconds, days, months, years, rtc_get_day_phase(), unix);
}

void server_print_serial(void) {
  Serial.printf("SERVER - SSID: %s, IP: %03d.%03d.%03d.%03d, Connections: %d, Last Record UNIX: %ld\n", ft_server_ssid, ft_server_ip[0], ft_server_ip[1], ft_server_ip[2], ft_server_ip[3], file_transfer_get_connections(), get_last_record_unix());
}

void gps_print_serial(void) {
  Serial.printf("GPS    - Long.: %09.6f, Lat.: %09.6f, Alt.: %06.1f, ", gps_get_lat(), gps_get_long(), gps_get_alt());
  Serial.printf("Sat.: %02d, World Map X Offset: %03d, World Map Y Offset: %03d\n", gps_get_sat(), gps_get_draw_x(), gps_get_draw_y());
}

void geiger_print_serial(void) {
  Serial.printf("GEIGER - CPM: %04d, 60s SMA uSV/h: %05.2f, uSV/h: %05.2f, mRem/h: %04.2f, mR/h: %06.2f, Danger Level: %d, CPM/uSV/h: %03d\n",
                (geiger_getCPM() == -1) ? 9999 : geiger_getCPM(),
                geiger_getAvgSieverts(),
                (geiger_getSieverts() == -1) ? 99.99 : geiger_getSieverts(),
                (geiger_getRems() == -1) ? 9.99 : geiger_getRems(),
                (geiger_getRoentgens() == -1) ? 999.99 : geiger_getRoentgens(),
                geiger_getDangerLevel(),
                eeprom_get_cpm_per_uSv_per_h());
}

void serial_print_all(void) {
  Serial.println(F("------------------------------------------------------------------------------------------------------------------------"));
  rtc_print_serial();
  system_print_serial();
  power_print_serial();
  server_print_serial();
  gps_print_serial();
  geiger_print_serial();
}
