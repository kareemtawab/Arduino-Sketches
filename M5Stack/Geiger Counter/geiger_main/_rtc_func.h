#ifndef _rtc_func_h
#define _rtc_func_h

#include <Arduino.h>

void init_rtc (void);
int rtc_get_day_phase(void);
void rtc_set_time_date (int, int, int, int, int, int, int);
void rtc_update (void);
uint32_t rtc_get_unix (void);

#endif
