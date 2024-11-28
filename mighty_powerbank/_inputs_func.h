#ifndef _inputs_func_h
#define _inputs_func_h

void init_inputs (void);
void up_click(void);
void up_long_start(void);
void down_click(void);
void down_long_start(void);
void left_click(void);
void left_long_start(void);
void right_click(void);
void right_long_start(void);
void inputs_update (void);
bool inputs_get_joystick_event(void);
int inputs_get_joystick_direction(void);


#endif