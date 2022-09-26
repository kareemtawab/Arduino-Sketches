#include "_batt_led_vib_func.h"
#include <M5Core2.h>

int batt_get_percent(void) {
  float batVoltage;
  int batPercentage; batVoltage = M5.Axp.GetBatVoltage();
  batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;
  return batPercentage;
}

void green_led(bool s) {
  M5.Axp.SetLed(s);
}

void vibrate(int _delay) {
  M5.Axp.SetLDOEnable(3, true);
  delay(_delay);
  M5.Axp.SetLDOEnable(3, false);
}

void power_off(void) {
  M5.Axp.PowerOff();
}
