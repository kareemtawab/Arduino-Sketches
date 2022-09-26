#ifndef _touch_func_h
#define _touch_func_h

void init_touch (void);
int touch_get (void);
void touch_update (void);
void audio_button_touched (void);
void lights_button_touched (void);
void up_button_touched (void);
void down_button_touched (void);
void calibration_cancel_button_touched(void);
void calibration_save_button_touched(void);

#endif
