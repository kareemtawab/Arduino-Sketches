#ifndef _display_func_h
#define _display_func_h

void init_display(void);
void display_refresh(void);
void display_batt_time_date(void);
void display_main_bkg(void);
void display_update(void);
void display_draw_about_screen(void);
void display_return_to_settings(void);
void drawGraph(float*, bool, bool, bool, int, bool);

#endif
