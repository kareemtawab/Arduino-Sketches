#ifndef _gps_func_h
#define _gps_func_h

void init_gps (void);
void gps_update (void);
float gps_get_lat (void);
float gps_get_long (void);
int gps_get_alt (void);
int gps_get_sat (void);
int gps_get_hours (void);
int gps_get_minutes (void);
int gps_get_seconds (void);
void gps_print_serial (void);

#endif
