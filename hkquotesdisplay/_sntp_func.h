#ifndef _sntp_func_h
#define _sntp_func_h

void sntp_init (void);
bool sntp_update (void);
long long int sntp_get_epoch (void);

#endif
