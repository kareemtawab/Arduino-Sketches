#ifndef _rtc_func_h
#define _rtc_func_h
#include "Arduino.h"

void init_rtc (void);
void rtc_set_time_date (int, int, int, int, int, int, int);
void rtc_update (void);
int rtc_get_hour (void);
int rtc_get_minute (void);
char* rtc_get_AM_PM (void);
int rtc_get_day (void);
int rtc_get_month (void);
int rtc_get_year (void);
float rtc_get_temp (void);
uint32_t rtc_get_unix (void);

#endif
