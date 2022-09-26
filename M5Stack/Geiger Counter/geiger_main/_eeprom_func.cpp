#include "_eeprom_func.h"
#include "_batt_led_vib_func.h"
#include "_touch_func.h"
#include <EEPROM.h>

#define EEPROM_SIZE 12
#define ID_RESTART 4

int cpm_per_uSv_per_h_new;
int cpm_per_uSv_per_h_diff;
int cpm_per_uSv_per_h_old;
extern bool calibration_cancelled;
extern bool calibration_saved;

void init_eeprom(void) {
  EEPROM.begin(EEPROM_SIZE);
  cpm_per_uSv_per_h_old = eeprom_get_cpm_per_uSv_per_h();
}

void eeprom_update(void) {
  if (calibration_cancelled) {
    cpm_per_uSv_per_h_diff = 0;
    calibration_cancelled = false;
  }
  if (calibration_saved) {
    if (cpm_per_uSv_per_h_new != eeprom_get_cpm_per_uSv_per_h()) {
      //preferences.begin("gm-pref", false);
      noInterrupts();
      delay(20);
      EEPROM.put(0, cpm_per_uSv_per_h_new);
      EEPROM.end();
      restart_now();
    }
    calibration_saved = false;
  }
}

int eeprom_get_cpm_per_uSv_per_h(void) {
  //preferences.begin("gm-pref", false);
  int v;
  EEPROM.get(0, v);
  //preferences.end();
  return v;
}
