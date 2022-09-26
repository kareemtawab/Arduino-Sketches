#include "_batt_led_vib_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>

#define BATTERY_VOLTAGE_MIN_LIMIT 3.6
#define BATTERY_VOLTAGE_MAX_LIMIT 4.15

float batt_get_voltage(void) {
  return 0;
}

int batt_get_percent(void) {
  return M5.Power.getBatteryLevel();
}

void green_led(bool s) {
  M5.Power.setLed(s);
}

void vibrate(int _delay) {
  M5.Power.Axp192.setLDO3(3300);
  delay(_delay);
  M5.Power.Axp192.setLDO3(0);
}

void power_off(void) {
  M5.Power.Axp192.powerOff();
}

void restart_now(void) {
  ESP.restart();
}

bool charging_enabled(void) {
  if (M5.Power.isCharging()) {
    green_led(true);
    return 1;
  }
  else {
    green_led(false);
    return 0;
  }
}
