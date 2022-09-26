#ifndef _rtc_func_h
#define _rtc_func_h

void init_rtc (void);
void rtc_set_time (int, int, int);
void rtc_set_date (int, int, int, int);
void rtc_print_serial (void);
void rtc_run (void);

#endif
