#ifndef _batt_led_func_h
#define _batt_led_func_h

void batt_led_vib_init(void);
void batt_led_vib_update(void);
float batt_get_vin_voltage(void);
float batt_get_voltage(void);
int batt_get_percent(void);
float get_internal_temp_c(void);
float get_internal_temp_f(void);
void green_led(bool);
void vibrate(int);
void power_off(void);
void restart_now(void);
int get_battery_current(void);

#endif
