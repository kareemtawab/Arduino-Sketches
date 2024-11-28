#ifndef _power_func_h
#define _power_func_h

void init_power (void);
void power_type_a_on (void);
void power_type_a_off (void);
void power_type_c_on (void);
void power_type_c_off (void);
void power_bt_on (void);
void power_bt_off (void);
void power_inverter_on (void);
void power_inverter_off (void);
void power_activate_30(void);
void power_activate_60 (void);
void power_update(void);
void parseBMSdata(void);
void printBMSdata(void);

#endif
