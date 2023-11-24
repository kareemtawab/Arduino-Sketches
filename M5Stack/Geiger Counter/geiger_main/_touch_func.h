#ifndef _touch_func_h
#define _touch_func_h
#include <FT6X36.h>

void init_touch (void);
void onTouch(TPoint, TEvent);
void touch_update (void);
void mode_button_touched(int);
void about_button_touched(void);
void about_screen_touched(void);
void lights_button_touched (void);
void audio_button_touched (void);
void vibrate_button_touched (void);
void ftp_button_touched (void);
void log_button_touched (void);
void up_button_touched (void);
void down_button_touched (void);
void calibration_cancel_button_touched(void);
void calibration_save_button_touched(void);
void settings_ok_button_touched(void);

#endif
