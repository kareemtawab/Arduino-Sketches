#include "_eeprom_func.h"
#include "_display_func.h"
#include "_touch_func.h"
#include "_batt_led_vib_func.h"
#include <FT6X36.h>

FT6X36 ts(&Wire, 39);

#define ID_SWITCHOFF -1
#define MAIN 0
#define MODE_WORLD 0
#define MODE_GRAPH 1
#define MODE_GEIGER 2
#define MODE_CLOCK 3
#define MODE_NERD 4
#define ID_CALIBRATION 5
#define ID_SETTINGS 6
#define ID_ABOUT 7

#define MODE_WORLD_X1 15
#define MODE_WORLD_X2 70
#define MODE_GRAPH_X1 75
#define MODE_GRAPH_X2 129
#define MODE_GEIGER_X1 134
#define MODE_GEIGER_X2 188
#define MODE_CLOCK_X1 193
#define MODE_CLOCK_X2 247
#define MODE_NERD_X1 252
#define MODE_NERD_X2 306
#define MODES_Y1 81
#define MODES_Y2 149

extern int page_id;
extern int bkg_drawn;
extern int cpm_per_uSv_per_h;
int touch_x, touch_y;
bool calibration_cancelled = false;
bool calibration_saved = false;
struct _data {
  int x1;
  int y1;
  int x2;
  int y2;
} typedef button_parameters;

button_parameters
  world_button_settings,
  graph_button_settings,
  geiger_button_settings,
  clock_button_settings,
  nerd_button_settings,
  lights_button_settings,
  audio_button_settings,
  vibrate_button_settings,
  ftp_button_settings,
  log_button_settings,
  about_button_settings,
  about_screen,
  up_button_calibration,
  down_button_calibration,
  button_a,
  button_b,
  button_c;

void init_touch(void) {
  // world_button_settings
  world_button_settings.x1 = MODE_WORLD_X1;
  world_button_settings.y1 = MODES_Y1;
  world_button_settings.x2 = MODE_WORLD_X2;
  world_button_settings.y2 = MODES_Y2;
  // graph_button_settings
  graph_button_settings.x1 = MODE_GRAPH_X1;
  graph_button_settings.y1 = MODES_Y1;
  graph_button_settings.x2 = MODE_GRAPH_X2;
  graph_button_settings.y2 = MODES_Y2;
  // geiger_button_settings
  geiger_button_settings.x1 = MODE_GEIGER_X1;
  geiger_button_settings.y1 = MODES_Y1;
  geiger_button_settings.x2 = MODE_GEIGER_X2;
  geiger_button_settings.y2 = MODES_Y2;
  // clock_button_settings
  clock_button_settings.x1 = MODE_CLOCK_X1;
  clock_button_settings.y1 = MODES_Y1;
  clock_button_settings.x2 = MODE_CLOCK_X2;
  clock_button_settings.y2 = MODES_Y2;
  // nerd_button_settings
  nerd_button_settings.x1 = MODE_NERD_X1;
  nerd_button_settings.y1 = MODES_Y1;
  nerd_button_settings.x2 = MODE_NERD_X2;
  nerd_button_settings.y2 = MODES_Y2;
  // lights_button_settings
  lights_button_settings.x1 = 19;
  lights_button_settings.y1 = 167;
  lights_button_settings.x2 = 52;
  lights_button_settings.y2 = 184;
  // audio_button_settings
  audio_button_settings.x1 = 19;
  audio_button_settings.y1 = 196;
  audio_button_settings.x2 = 52;
  audio_button_settings.y2 = 213;
  // vibrate_button_settings
  vibrate_button_settings.x1 = 107;
  vibrate_button_settings.y1 = 167;
  vibrate_button_settings.x2 = 140;
  vibrate_button_settings.y2 = 184;
  // ftp_button_settings
  ftp_button_settings.x1 = 107;
  ftp_button_settings.y1 = 196;
  ftp_button_settings.x2 = 140;
  ftp_button_settings.y2 = 213;
  // log_button_settings
  log_button_settings.x1 = 197;
  log_button_settings.y1 = 167;
  log_button_settings.x2 = 230;
  log_button_settings.y2 = 184;
  // about_button_settings
  about_button_settings.x1 = 238;
  about_button_settings.y1 = 59;
  about_button_settings.x2 = 308;
  about_button_settings.y2 = 80;
  // about_screen
  about_screen.x1 = 0;
  about_screen.y1 = 22;
  about_screen.x2 = 320;
  about_screen.y2 = 240;
  // up_button_calibration
  up_button_calibration.x1 = 195;
  up_button_calibration.y1 = 55;
  up_button_calibration.x2 = 306;
  up_button_calibration.y2 = 85;
  // down_button_calibration
  down_button_calibration.x1 = 195;
  down_button_calibration.y1 = 133;
  down_button_calibration.x2 = 306;
  down_button_calibration.y2 = 163;
  // button_a
  button_a.x1 = 0;
  button_a.y1 = 240;
  button_a.x2 = 106;
  button_a.y2 = 280;
  // button_b
  button_b.x1 = 106;
  button_b.y1 = 240;
  button_b.x2 = 212;
  button_b.y2 = 280;
  // button_c
  button_c.x1 = 212;
  button_c.y1 = 240;
  button_c.x2 = 306;
  button_c.y2 = 280;

  ts.begin();
  ts.registerTouchHandler(onTouch);
}

void onTouch(TPoint pos, TEvent e) {
  touch_x = pos.x;
  touch_y = pos.y;
  // Serial.print("X: ");
  // Serial.print(touch_x);
  // Serial.print(", Y: ");
  // Serial.println(touch_y);
}

void touch_update(void) {
  touch_x = 0;
  touch_y = 0;
  ts.loop();
  if (eeprom_get_vibrate() && touch_x && touch_y) vibrate(50);
  if (touch_x > button_a.x1 && touch_x < button_a.x2 && touch_y > button_a.y1 && touch_y < button_a.y2) {
    switch (page_id) {
      case ID_SWITCHOFF:
        power_off();
        break;
      case MAIN:
        bkg_drawn = false;
        page_id = ID_CALIBRATION;
        break;
      case ID_CALIBRATION:
        // save to EEPROM
        calibration_save_button_touched();
        bkg_drawn = false;
        page_id = MAIN;
        break;
      case ID_SETTINGS:
        break;
    }
  }
  if (touch_x > button_b.x1 && touch_x < button_b.x2 && touch_y > button_b.y1 && touch_y < button_b.y2) {
    switch (page_id) {
      case ID_SWITCHOFF:
        restart_now();
        break;
      case MAIN:
        bkg_drawn = false;
        page_id = ID_SETTINGS;
        break;
      case ID_CALIBRATION:
        break;
      case ID_SETTINGS:
        bkg_drawn = false;
        page_id = MAIN;
        break;
    }
  }
  if (touch_x > button_c.x1 && touch_x < button_c.x2 && touch_y > button_c.y1 && touch_y < button_c.y2) {
    switch (page_id) {
      case ID_SWITCHOFF:
        bkg_drawn = false;
        page_id = MAIN;
        break;
      case MAIN:
        bkg_drawn = false;
        page_id = ID_SWITCHOFF;
        break;
      case ID_CALIBRATION:
        calibration_cancel_button_touched();
        bkg_drawn = false;
        page_id = MAIN;
        break;
      case ID_SETTINGS:
        break;
    }
  }
  if (touch_y < button_a.y1) {
    switch (page_id) {
      case ID_SWITCHOFF:
        break;
      case MAIN:
        break;
      case ID_CALIBRATION:
        if (touch_x > up_button_calibration.x1 && touch_x < up_button_calibration.x2 && touch_y > up_button_calibration.y1 && touch_y < up_button_calibration.y2) {
          up_button_touched();
        }
        if (touch_x > down_button_calibration.x1 && touch_x < down_button_calibration.x2 && touch_y > down_button_calibration.y1 && touch_y < down_button_calibration.y2) {
          down_button_touched();
        }
        break;
      case ID_SETTINGS:
        if (touch_x > world_button_settings.x1 && touch_x < world_button_settings.x2 && touch_y > world_button_settings.y1 && touch_y < world_button_settings.y2) {
          mode_button_touched(MODE_WORLD);
        }
        if (touch_x > graph_button_settings.x1 && touch_x < graph_button_settings.x2 && touch_y > graph_button_settings.y1 && touch_y < graph_button_settings.y2) {
          mode_button_touched(MODE_GRAPH);
        }
        if (touch_x > geiger_button_settings.x1 && touch_x < geiger_button_settings.x2 && touch_y > geiger_button_settings.y1 && touch_y < geiger_button_settings.y2) {
          mode_button_touched(MODE_GEIGER);
        }
        if (touch_x > clock_button_settings.x1 && touch_x < clock_button_settings.x2 && touch_y > clock_button_settings.y1 && touch_y < clock_button_settings.y2) {
          mode_button_touched(MODE_CLOCK);
        }
        if (touch_x > nerd_button_settings.x1 && touch_x < nerd_button_settings.x2 && touch_y > nerd_button_settings.y1 && touch_y < nerd_button_settings.y2) {
          mode_button_touched(MODE_NERD);
        }
        if (touch_x > about_button_settings.x1 && touch_x < about_button_settings.x2 && touch_y > about_button_settings.y1 && touch_y < about_button_settings.y2) {
          about_button_touched();
        }
        if (touch_x > lights_button_settings.x1 && touch_x < lights_button_settings.x2 && touch_y > lights_button_settings.y1 && touch_y < lights_button_settings.y2) {
          lights_button_touched();
        }
        if (touch_x > audio_button_settings.x1 && touch_x < audio_button_settings.x2 && touch_y > audio_button_settings.y1 && touch_y < audio_button_settings.y2) {
          audio_button_touched();
        }
        if (touch_x > vibrate_button_settings.x1 && touch_x < vibrate_button_settings.x2 && touch_y > vibrate_button_settings.y1 && touch_y < vibrate_button_settings.y2) {
          vibrate_button_touched();
        }
        if (touch_x > ftp_button_settings.x1 && touch_x < ftp_button_settings.x2 && touch_y > ftp_button_settings.y1 && touch_y < ftp_button_settings.y2) {
          ftp_button_touched();
        }
        if (touch_x > log_button_settings.x1 && touch_x < log_button_settings.x2 && touch_y > log_button_settings.y1 && touch_y < log_button_settings.y2) {
          log_button_touched();
        }
        break;
      case ID_ABOUT:
        if (touch_x > about_screen.x1 && touch_x < about_screen.x2 && touch_y > about_screen.y1 && touch_y < about_screen.y2) {
          about_screen_touched();
        }
        break;
    }
  }
}

void mode_button_touched(int m) {
  eeprom_set_screen_mode(m);
}
void about_button_touched(void) {
  display_draw_about_screen();
}
void about_screen_touched(void) {
  display_return_to_settings();
}
void audio_button_touched(void) {
  eeprom_set_audio(!eeprom_get_audio());
}
void lights_button_touched(void) {
  eeprom_set_lights(!eeprom_get_lights());
}
void vibrate_button_touched(void) {
  eeprom_set_vibrate(!eeprom_get_vibrate());
}
void ftp_button_touched(void) {
  eeprom_set_ft(!eeprom_get_ft());
}
void log_button_touched(void) {
  eeprom_set_log(!eeprom_get_log());
}
void up_button_touched(void) {
  if (cpm_per_uSv_per_h < 999) cpm_per_uSv_per_h++;
}
void down_button_touched(void) {
  if (cpm_per_uSv_per_h > 1) cpm_per_uSv_per_h--;
}

void calibration_cancel_button_touched(void) {
  calibration_cancelled = true;
}

void calibration_save_button_touched(void) {
  calibration_saved = true;
}
