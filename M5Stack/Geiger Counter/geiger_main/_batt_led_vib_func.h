#ifndef _batt_led_func_h
#define _batt_led_func_h

float batt_get_voltage(void);
int batt_get_percent(void);
void green_led(bool);
void vibrate(int);
void power_off(void);
void restart_now(void);
bool charging_enabled(void);

#endif
