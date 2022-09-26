#include "_eeprom_func.h"
#include "_display_func.h"
#include "_touch_func.h"
#include "_batt_led_vib_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>

#define ID_SWITCHOFF   0
#define ID_MAIN        1
#define ID_CALIBRATION 2
#define ID_SETTINGS    3
#define ID_RESTART     4

bool geiger_audio_enabled = true;
bool neopixel_enabled = true;
bool increase_cpm_per_uSv_per_h = false;
bool decrease_cpm_per_uSv_per_h = false;
bool calibration_cancelled = false;
bool calibration_saved = false;
extern int page_id;
extern int bkg_drawn;
struct _data {
  int x;
  int y;
  int w;
  int h;
} typedef button_parameters;

button_parameters audio_button_main,
                  audio_button_settings,
                  lights_button_main,
                  lights_button_settings,
                  up_button_calibration,
                  down_button_calibration
                  ;
//HotZone audio_button_main(15, 93, 45, 123, &audio_button_touched);
//HotZone audio_button_settings(15, 93, 45, 123, &audio_button_touched);
//HotZone lights_button_main(15, 135, 45, 165, &lights_button_touched);
//HotZone lights_button_settings(15, 135, 45, 165, &lights_button_touched);
//HotZone up_button_calibration(195, 55, 306, 85, &up_button_touched);
//HotZone down_button_calibration(195, 133, 306, 163, &down_button_touched);

void init_touch (void) {
  // audio_button_main
  audio_button_main.x = 15;
  audio_button_main.y = 93;
  audio_button_main.w = 45;
  audio_button_main.h = 123;
  // audio_button_settings
  audio_button_settings.x = 15;
  audio_button_settings.y = 93;
  audio_button_settings.w = 45;
  audio_button_settings.h = 123;
  // lights_button_main
  lights_button_main.x = 15;
  lights_button_main.y = 135;
  lights_button_main.w = 45;
  lights_button_main.h = 165;
  // lights_button_settings
  lights_button_settings.x = 15;
  lights_button_settings.y = 135;
  lights_button_settings.w = 45;
  lights_button_settings.h = 165;
  // up_button_calibration
  up_button_calibration.x = 195;
  up_button_calibration.y = 55;
  up_button_calibration.w = 306;
  up_button_calibration.h = 85;
  // down_button_calibration
  down_button_calibration.x = 195;
  down_button_calibration.y = 133;
  down_button_calibration.w = 306;
  down_button_calibration.h = 163;
}

int touch_get (void) {
  if (M5.BtnA.wasReleased()) {
    return 1;
  }
  else if (M5.BtnB.wasReleased()) {
    return 2;
  }
  else if (M5.BtnC.wasReleased()) {
    return 3;
  }
  else {
    return 0;
  }
}

void touch_update (void) {
  M5.update();
  int touch_id;
  touch_id = touch_get();
  switch (touch_id) {
    case 1: // left button
      vibrate(50);
      switch (page_id) {
        case ID_SWITCHOFF:
          power_off();
          break;
        case ID_MAIN:
          bkg_drawn = false;
          page_id = ID_CALIBRATION;
          break;
        case ID_CALIBRATION:
          // save to EEPROM
          calibration_save_button_touched();
          bkg_drawn = false;
          page_id = ID_MAIN;
          break;
        case ID_SETTINGS:
          // save to EEPROM
          bkg_drawn = false;
          page_id = ID_MAIN;
          break;
      }
      break;
    case 2: // middle button
      vibrate(50);
      switch (page_id) {
        case ID_SWITCHOFF:
          break;
        case ID_MAIN:
          bkg_drawn = false;
          page_id = ID_SETTINGS;
          break;
        case ID_CALIBRATION:
          break;
        case ID_SETTINGS:
          break;
      }
      break;
    case 3: // right button
      vibrate(50);
      switch (page_id) {
        case ID_SWITCHOFF:
          bkg_drawn = false;
          page_id = ID_MAIN;
          break;
        case ID_MAIN:
          bkg_drawn = false;
          page_id = ID_SWITCHOFF;
          break;
        case ID_CALIBRATION:
          calibration_cancel_button_touched();
          bkg_drawn = false;
          page_id = ID_MAIN;
          break;
        case ID_SETTINGS:
          bkg_drawn = false;
          page_id = ID_MAIN;
          break;
      }
  }
  auto pos = M5.Touch.getDetail();
  if (pos.wasReleased()) {
    switch (page_id) {
      case ID_SWITCHOFF:
        break;
      case ID_MAIN:
        if (pos.x > audio_button_main.x && pos.x < audio_button_main.w && pos.y > audio_button_main.y && pos.y < audio_button_main.h) {
          audio_button_touched();
          vibrate(50);
        }
        if (pos.x > lights_button_main.x && pos.x < lights_button_main.w && pos.y > lights_button_main.y && pos.y < lights_button_main.h) {
          lights_button_touched();
          vibrate(50);
        }
        break;
      case ID_CALIBRATION:
        if (pos.x > up_button_calibration.x && pos.x < up_button_calibration.w && pos.y > up_button_calibration.y && pos.y < up_button_calibration.h) {
          up_button_touched();
          vibrate(50);
        }
        if (pos.x > down_button_calibration.x && pos.x < down_button_calibration.w && pos.y > down_button_calibration.y && pos.y < down_button_calibration.h) {
          vibrate(50);
          down_button_touched();
        }
        break;
      case ID_SETTINGS:
        if (pos.x > audio_button_settings.x && pos.x < audio_button_settings.w && pos.y > audio_button_settings.y && pos.y < audio_button_settings.h) {
          audio_button_touched();
          vibrate(50);
        }
        if (pos.x > lights_button_settings.x && pos.x < lights_button_settings.w && pos.y > lights_button_settings.y && pos.y < lights_button_settings.h) {
          lights_button_touched();
          vibrate(50);
        }
        break;
    }
  }
}

void audio_button_touched(void) {
  geiger_audio_enabled = !geiger_audio_enabled;
}

void lights_button_touched(void) {
  neopixel_enabled = !neopixel_enabled;
}

void up_button_touched(void) {
  increase_cpm_per_uSv_per_h = true;
}

void down_button_touched(void) {
  decrease_cpm_per_uSv_per_h = true;
}

void calibration_cancel_button_touched(void) {
  calibration_cancelled = true;
}

void calibration_save_button_touched(void) {
  calibration_saved = true;
}
