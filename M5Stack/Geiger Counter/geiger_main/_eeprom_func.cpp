#include "_eeprom_func.h"
#include "_batt_led_vib_func.h"
#include "_touch_func.h"
#include <EEPROM.h>

#define EEPROM_SIZE 15
#define CPM_PER_USV_PER_H_ADDRESS 2
#define ALL_SETTINGS_ADDRESS 8

int cpm_per_uSv_per_h;
unsigned char all_settings;
extern bool calibration_cancelled;
extern bool calibration_saved;

void init_eeprom(void) {
  EEPROM.begin(EEPROM_SIZE);
  all_settings = eeprom_get_all_settings();
  cpm_per_uSv_per_h = eeprom_get_cpm_per_uSv_per_h();
}

void eeprom_update(void) {
  // for (int i = 7; i >= 0; i--) {
  //   Serial.print(bitRead(eeprom_get_all_settings(), i) ? "1" : "0");  // will reverse bit order!
  // }
  // Serial.println();
  if (calibration_cancelled) {
    cpm_per_uSv_per_h = eeprom_get_cpm_per_uSv_per_h();
    calibration_cancelled = false;
  }
  if (calibration_saved) {
    eeprom_set_cpm_per_uSv_per_h();
    restart_now();
    calibration_saved = false;
  }
  eeprom_set_all_settings();
}

int eeprom_get_cpm_per_uSv_per_h(void) {
  int a;
  EEPROM.get(CPM_PER_USV_PER_H_ADDRESS, a);
  EEPROM.commit();
  return a;
}

unsigned char eeprom_get_all_settings(void) {
  unsigned char a = EEPROM.read(ALL_SETTINGS_ADDRESS);
  EEPROM.commit();
  return a;
}

int eeprom_get_screen_mode(void) {
  unsigned char v = all_settings;
  int w;
  bitClear(v, 3);
  bitClear(v, 4);
  bitClear(v, 5);
  bitClear(v, 6);
  bitClear(v, 7);
  bitRead(v, 0) ? bitSet(w, 0) : bitClear(w, 0);
  bitRead(v, 1) ? bitSet(w, 1) : bitClear(w, 1);
  bitRead(v, 2) ? bitSet(w, 2) : bitClear(w, 2);
  return w;
}

bool eeprom_get_audio(void) {
  if (bitRead(all_settings, 3)) {
    return true;
  } else {
    return false;
  }
}

bool eeprom_get_lights(void) {
  if (bitRead(all_settings, 4)) {
    return true;
  } else {
    return false;
  }
}

bool eeprom_get_vibrate(void) {
  if (bitRead(all_settings, 5)) {
    return true;
  } else {
    return false;
  }
}

bool eeprom_get_ft(void) {
  if (bitRead(all_settings, 6)) {
    return true;
  } else {
    return false;
  }
}

bool eeprom_get_log(void) {
  if (bitRead(all_settings, 7)) {
    return true;
  } else {
    return false;
  }
}


void eeprom_set_cpm_per_uSv_per_h(void) {
  EEPROM_update_int(CPM_PER_USV_PER_H_ADDRESS, cpm_per_uSv_per_h);
}

void eeprom_set_all_settings(void) {
  EEPROM_update_byte(ALL_SETTINGS_ADDRESS, all_settings);
}

void eeprom_set_screen_mode(int s) {
  if bitRead (s, 0) bitSet(all_settings, 0);
  else bitClear(all_settings, 0);
  if bitRead (s, 1) bitSet(all_settings, 1);
  else bitClear(all_settings, 1);
  if bitRead (s, 2) bitSet(all_settings, 2);
  else bitClear(all_settings, 2);
}

void eeprom_set_audio(bool a) {
  if (a) bitSet(all_settings, 3);
  else bitClear(all_settings, 3);
}

void eeprom_set_lights(bool l) {
  if (l) bitSet(all_settings, 4);
  else bitClear(all_settings, 4);
}

void eeprom_set_vibrate(bool v) {
  if (v) bitSet(all_settings, 5);
  else bitClear(all_settings, 5);
}

void eeprom_set_ft(bool w) {
  if (w) bitSet(all_settings, 6);
  else bitClear(all_settings, 6);
}


void eeprom_set_log(bool d) {
  if (d) bitSet(all_settings, 7);
  else bitClear(all_settings, 7);
}

void EEPROM_update_int(int a, int d) {
  int temp;
  EEPROM.get(a, temp);
  EEPROM.commit();
  if (d != temp) {
    EEPROM.put(a, d);
    EEPROM.commit();
  }
}

void EEPROM_update_byte(int a, unsigned char d) {
  unsigned char temp;
  temp = EEPROM.read(a);
  EEPROM.commit();
  if (d != temp) {
    EEPROM.write(a, d);
    EEPROM.commit();
  }
}