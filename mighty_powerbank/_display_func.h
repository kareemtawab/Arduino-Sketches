#ifndef _display_func_h
#define _display_func_h

void init_display(void);
void display_draw_menu(void);
void display_main(void);
void display_draw_menu_page(char);
void drawCentreString(char*, int, int);
void drawMenu(unsigned char* menu[]);
void drawJoystick_idle(void);
void drawJoystick_left(void);
void drawJoystick_up(void);
void drawJoystick_right(void);
void drawJoystick_down(void);

#endif
