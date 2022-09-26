#ifndef _geiger_func_h
#define _geiger_func_h

void init_geiger (void);
void geiger_update (void);
int geiger_getCPM(void);
float geiger_getSieverts(void);
float geiger_getRems(void);
int geiger_getDangerLevel(void);
void geiger_ISR(void);

#endif
