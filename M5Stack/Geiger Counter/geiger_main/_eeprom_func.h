#ifndef _eeprom_func_h
#define _eeprom_func_h

void init_eeprom(void);
void eeprom_update(void);
int eeprom_get_cpm_per_uSv_per_h(void);
unsigned char eeprom_get_all_settings(void);
int eeprom_get_screen_mode(void);
bool eeprom_get_audio(void);
bool eeprom_get_lights(void);
bool eeprom_get_vibrate(void);
bool eeprom_get_ft(void);
bool eeprom_get_log(void);
void eeprom_set_cpm_per_uSv_per_h(void);
void eeprom_set_all_settings(void);
void eeprom_set_screen_mode(int);
void eeprom_set_audio(bool);
void eeprom_set_lights(bool);
void eeprom_set_vibrate(bool);
void eeprom_set_ft(bool);
void eeprom_set_log(bool);

void EEPROM_update_int(int, int);
void EEPROM_update_byte(int, unsigned char);
#endif
