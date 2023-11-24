#include "_display_func.h"
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_audio_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_touch_func.h"
#include "_eeprom_func.h"
#include "_file_transfer_func.h"
#include "_FreeSans5pt7b.h"
#include "_FreeSans7pt7b.h"
#include "_FreeSans9pt7b.h"
#include "_FreeSansBold9pt7b.h"
#include "_FreeSans12pt7b.h"
#include "_FreeSans25pt7b.h"
#include "_FreeSans35pt7b.h"
#include "_FreeSans40pt7b.h"
#include "bkg_data.c"
#include "icons_data.c"
#include <M5GFX.h>
#include <esp_task_wdt.h>

#define display_interval 100

// colors are in are encoded in RGB565
// http://www.rinkydinkelectronics.com/calc_rgb565.php
#define colorvalue_default_bg 0x0000
#define colorvalue_default_fg 0xF607

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

#define PHASE_NIGHT 0
#define PHASE_SUNRISE 1
#define PHASE_MORNING 2
#define PHASE_SUNSET 3

#define LCD_BRIGHTNESS 145

unsigned int page_id = MAIN;
bool bkg_drawn;
int update_count;
int day_phase_old;

int ox;
float oy;
extern float graphYPlotPointsOLD[60];
extern float graphYPlotPoints[60];
extern float graphSMAYPlotPointsOLD[60];
extern float graphSMAYPlotPoints[60];
extern int hours, minutes, seconds, days, months, years;
extern uint32_t unix;
extern int cpm_per_uSv_per_h;
extern IPAddress ft_server_ip;
extern const char *ft_server_ssid;

M5GFX disp;

void init_display(void) {
  disp.init();
  disp.setBrightness(LCD_BRIGHTNESS);
  disp.drawBitmap(0, 0, 320, 240, (uint16_t *)splash);
  vTaskDelay(500);
  for (int x = 0; x <= 152; x++) {
   esp_task_wdt_reset();
   disp.fillRoundRect(143, 129, x, 9, 2, WHITE);
    vTaskDelay(40);
  }
  vTaskDelay(500);
  display_refresh();
}

void display_refresh(void) {
  disp.fillRect(0, 0, 320, 240, BLACK);
  // disp.init();
}

void display_batt_time_date(void) {
  // time and date
  disp.setTextDatum(TR_DATUM);
  disp.setFont(&_FreeSans7pt7b);
  char timeStrbuff[20];
  sprintf(timeStrbuff, "%02d:%02d %02d.%02d\n", hours, minutes, days, months);
  if (eeprom_get_screen_mode() == MODE_CLOCK || page_id == ID_ABOUT) {
#define colorvalue_main4_bg 0x0861
#define colorvalue_main4_fg 0xFFFF
    disp.setTextColor(colorvalue_main4_fg, colorvalue_main4_bg);
    disp.drawString(timeStrbuff, 319, -1);
  } else {
    disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
    disp.drawString(timeStrbuff, 319, -1);
  }

  // battery bar
  if (eeprom_get_screen_mode() == MODE_CLOCK || page_id == ID_ABOUT) {
    disp.drawRoundRect(240, 12, 80, 9, 2, colorvalue_main4_fg);
  } else {
    disp.drawRoundRect(240, 12, 80, 9, 2, colorvalue_default_fg);
  }
  int batt_bar_max_width = 76;
  int batt_bar_width = 0;
  batt_bar_width = (float)batt_get_percent() / 100 * (float)batt_bar_max_width;
  if (get_battery_current() == 0) {
    disp.fillRect(242, 14, batt_bar_max_width, 5, CYAN);
  } else {
    if (batt_get_percent() <= 100 && batt_get_percent() > 50) {
      disp.fillRect(242, 14, batt_bar_width, 5, GREEN);
      disp.fillRect(242 + batt_bar_width + 1, 14, batt_bar_max_width - batt_bar_width - 1, 5, colorvalue_default_bg);
    }
    if (batt_get_percent() <= 50 && batt_get_percent() > 25) {
      disp.fillRect(242, 14, batt_bar_width, 5, ORANGE);
      disp.fillRect(242 + batt_bar_width + 1, 14, batt_bar_max_width - batt_bar_width - 1, 5, colorvalue_default_bg);
    }
    if (batt_get_percent() <= 25 && batt_get_percent() > 0) {
      disp.fillRect(242, 14, batt_bar_width, 5, RED);
      disp.fillRect(242 + batt_bar_width + 1, 14, batt_bar_max_width - batt_bar_width - 1, 5, colorvalue_default_bg);
    }
  }
}

void display_main_bkg(void) {
  if (bkg_drawn && day_phase_old != rtc_get_day_phase()) {
    bkg_drawn = false;
    day_phase_old = rtc_get_day_phase();
  }
  switch (page_id) {
    case ID_SWITCHOFF:  // switch off screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
        disp.drawBitmap(0, 22, 320, 204, (uint16_t *)switchoff);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)yes_restart_no_botbar);
        bkg_drawn = true;
      }
      break;
    case MAIN:  // world screen
      if (!bkg_drawn) {
        switch (eeprom_get_screen_mode()) {
          case MODE_WORLD:  // world screen
            disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
            disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main1);
            disp.drawBitmap(0, 226, 320, 14, (uint16_t *)cal_set_soff_botbar);
            break;
          case MODE_GRAPH:  // graph screen
            disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
            disp.fillRect(0, 22, 320, 204, colorvalue_default_bg);
            disp.drawBitmap(0, 226, 320, 14, (uint16_t *)cal_set_soff_botbar);
            disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
            disp.setTextDatum(TC_DATUM);
            disp.setFont(&_FreeSans7pt7b);
            disp.drawString("Seconds", 160, 211);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans12pt7b);
            disp.drawString("Radiation History", 6, 26);
            disp.setFont(&_FreeSans7pt7b);
            disp.drawString("uSv/h", 30, 50);
            disp.drawString("True", 288, 25);
            disp.drawString("SMA", 288, 37);
            disp.drawRoundRect(265, 24, 55, 26, 3, colorvalue_default_fg);
            disp.drawLine(270, 30, 285, 30, WHITE);
            disp.drawLine(270, 31, 285, 31, WHITE);
            disp.drawLine(270, 42, 285, 42, BLUE);
            disp.drawLine(270, 43, 285, 43, BLUE);
            drawGraph(NULL, false, true, false, NULL, false);
            break;
          case MODE_GEIGER:  // geiger screen
            disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
            disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main3);
            disp.drawBitmap(0, 226, 320, 14, (uint16_t *)cal_set_soff_botbar);
            break;
          case MODE_CLOCK:  // clock screen
            disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar_dark);
            switch (rtc_get_day_phase()) {
              case PHASE_NIGHT:
                disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main4_night);
                break;
              case PHASE_SUNRISE:
                disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main4_sunrise);
                break;
              case PHASE_MORNING:
                disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main4_morning);
                break;
              case PHASE_SUNSET:
                disp.drawBitmap(0, 22, 320, 204, (uint16_t *)main4_sunset);
                break;
              default:
                disp.fillRect(0, 22, 320, 204, colorvalue_default_bg);
                break;
            }
            disp.drawBitmap(0, 226, 320, 14, (uint16_t *)cal_set_soff_botbar_dark);
            break;
          case MODE_NERD:  // nerd screen
            disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
            disp.fillRect(0, 22, 320, 204, colorvalue_default_bg);
            disp.fillRoundRect(5, 24, 150, 67, 6, GREEN);
            disp.fillRoundRect(160, 24, 155, 67, 6, CYAN);
            disp.fillRoundRect(5, 96, 165, 67, 6, RED);
            disp.fillRoundRect(175, 96, 140, 67, 6, ORANGE);
            disp.fillRoundRect(5, 168, 140, 53, 6, MAGENTA);
            disp.fillRoundRect(150, 168, 165, 53, 6, WHITE);
            disp.drawBitmap(0, 226, 320, 14, (uint16_t *)cal_set_soff_botbar);
            break;
        }
        bkg_drawn = true;
      }
      break;
    case ID_CALIBRATION:  // calibration screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
        disp.drawBitmap(0, 22, 320, 204, (uint16_t *)calibration);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)save_cancel_botbar);
        bkg_drawn = true;
      }
      break;
    case ID_SETTINGS:  // settings screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar);
        disp.drawBitmap(0, 22, 320, 204, (uint16_t *)settings);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)ok_botbar);
        bkg_drawn = true;
      }
      break;
    case ID_ABOUT:  // about screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 240, 22, (uint16_t *)topbar_dark);
        disp.drawBitmap(0, 22, 320, 218, (uint16_t *)about);
        bkg_drawn = true;
      }
      break;
  }
}

void display_update(void) {
  update_count++;
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_SWITCHOFF) {
    display_main_bkg();
    display_batt_time_date();
  }

  //-------------------------------------------------------------------------------------------------------------
  if (page_id == MAIN) {
    switch (eeprom_get_screen_mode()) {
      case MODE_WORLD:
        display_main_bkg();
        display_batt_time_date();
        disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
        disp.fillCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 2, RED);  //Draw a filled red circle of gps location on map with radius of 3
        disp.drawCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 4, RED);  //Draw a red circle of gps location on map with radius of 4
        disp.drawCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 5, RED);  //Draw a red circle of gps location on map with radius of 5

        disp.setTextDatum(TR_DATUM);
        disp.setFont(&_FreeSans12pt7b);
        if (geiger_getSieverts() == -1) {
          disp.drawString("99.99", 70, 184);
        } else {
          char sv[7];
          sprintf(sv, "%05.2f\n", geiger_getSieverts());
          disp.drawString(sv, 70, 184);
        }
        disp.setFont(&_FreeSans9pt7b);

        if (geiger_getCPM() == -1) {
          disp.drawString("9999", 70, 206);
        } else {
          char c[6];
          sprintf(c, "%04d\n", geiger_getCPM());
          disp.drawString(c, 70, 206);
        }

        disp.setTextDatum(TR_DATUM);
        disp.setFont(&_FreeSans9pt7b);
        char g[10];
        sprintf(g, "%02d\n", gps_get_sat());
        disp.drawString(g, 212, 153);
        disp.setTextDatum(TL_DATUM);
        sprintf(g, "%09.6f\n", gps_get_lat());
        disp.drawString(g, 232, 170);
        sprintf(g, "%09.6f\n", gps_get_long());
        disp.drawString(g, 232, 188);
        sprintf(g, "%06.1f\n", gps_get_alt());
        disp.drawString(g, 232, 206);
        break;
      case MODE_GRAPH:
        display_main_bkg();
        display_batt_time_date();

        disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
        disp.setTextDatum(TC_DATUM);
        char q[12];
        if (geiger_getSieverts() == -1) {
          disp.drawString("99.99", 140, 50);
        } else {
          sprintf(q, "T: %05.2f\n", geiger_getSieverts());
          disp.drawString(q, 140, 50);
        }
        if (geiger_getAvgSieverts() == -1) {
          disp.drawString("99.99", 210, 50);
        } else {
          sprintf(q, "A: %05.2f\n", geiger_getAvgSieverts());
          disp.drawString(q, 210, 50);
        }
        if (update_count > 1000 / display_interval) {
          // drawGraph(float *arr, bool drawSamplesInBkgColor, bool drawGraphOutlinesOnly, bool drawGraphOutlinesWithGrid, int samplesColor, bool drawArrow)
          drawGraph(graphSMAYPlotPointsOLD, true, false, false, NULL, false);
          drawGraph(graphSMAYPlotPoints, false, false, false, BLUE, false);
          drawGraph(graphYPlotPointsOLD, true, false, false, NULL, true);
          drawGraph(graphYPlotPoints, false, false, false, WHITE, true);
          update_count = 0;
        }
        break;
      case MODE_GEIGER:
        display_main_bkg();
        display_batt_time_date();

#define colorvalue_geiger_bg 0xB73F
#define colorvalue_geiger_fg 0x3186
        if (eeprom_get_lights()) {
          disp.drawBitmap(181, 59, 28, 28, (uint16_t *)lamp_on);
        } else {
          disp.drawBitmap(181, 59, 28, 28, (uint16_t *)lamp_off);
        }
        if (eeprom_get_audio()) {
          disp.drawBitmap(214, 64, 23, 21, (uint16_t *)sound_on);
        } else {
          disp.drawBitmap(214, 64, 23, 21, (uint16_t *)sound_off);
        }
        if (geiger_getDangerLevel() > 0) {
          disp.drawBitmap(244, 59, 33, 28, (uint16_t *)warning_on);
        } else {
          disp.drawBitmap(244, 59, 33, 28, (uint16_t *)warning_off);
        }
        disp.setTextColor(colorvalue_geiger_fg, colorvalue_geiger_bg);
        // disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
        disp.fillRect(108, 113, 5, 5, colorvalue_geiger_fg);
        disp.setTextDatum(TL_DATUM);
        disp.setFont(&_FreeSans35pt7b);
        char m[6];
        if (geiger_getSieverts() == -1) {
          disp.drawString("99", 32, 64);
          disp.setFont(&_FreeSans25pt7b);
          disp.drawString("99", 115, 80);
        } else {
          sprintf(m, "%d%d\n", ((int)(geiger_getSieverts() * 100) / 1000) % 10, ((int)(geiger_getSieverts() * 100) / 100) % 10);
          disp.drawString(m, 32, 64);
          disp.setFont(&_FreeSans25pt7b);
          sprintf(m, "%d%d\n", ((int)(geiger_getSieverts() * 100) / 10) % 10, (int)(geiger_getSieverts() * 100) % 10);
          disp.drawString(m, 115, 80);
        }
        disp.setFont(&_FreeSans25pt7b);
        if (geiger_getCPM() == -1) {
          disp.drawString("9999", 61, 131);
        } else {
          sprintf(m, "%04d\n", geiger_getCPM());
          disp.drawString(m, 61, 131);
        }
        break;
      case MODE_CLOCK:
        display_main_bkg();
        display_batt_time_date();
        char td[25];
        switch (rtc_get_day_phase()) {
          case PHASE_NIGHT:
#define colorvalue_main4_night_bg 0x10C4
#define colorvalue_main4_night_fg 0xD65E
            disp.setTextColor(colorvalue_main4_night_fg, colorvalue_main4_night_bg);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans25pt7b);
            sprintf(td, "%02d:%02d\n", hours, minutes);
            disp.drawString(td, 5, 171);
            disp.setFont(&_FreeSans12pt7b);
            sprintf(td, ":%02d\n", seconds);
            disp.drawString(td, 127, 191);
            disp.setFont(&_FreeSans9pt7b);
            sprintf(td, "%02d.%02d.%04d\n", days, months, years);
            disp.drawString(td, 6, 154);
            sprintf(td, "%04.1f   C\n", get_internal_temp_c());
            disp.drawString(td, 181, 193);
            disp.fillCircle(224, 198, 3, colorvalue_main4_night_fg);  // outer diameter circle of the degrees symbol
            disp.fillCircle(224, 198, 1, colorvalue_main4_night_bg);  // inner diameter circle of the degrees symbol
            break;
          case PHASE_SUNRISE:
#define colorvalue_main4_sunrise_bg 0x28E3
#define colorvalue_main4_sunrise_fg 0xDBCB
            disp.setTextColor(colorvalue_main4_sunrise_fg, colorvalue_main4_sunrise_bg);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans25pt7b);
            sprintf(td, "%02d:%02d\n", hours, minutes);
            disp.drawString(td, 5, 167);
            disp.setFont(&_FreeSans12pt7b);
            sprintf(td, ":%02d\n", seconds);
            disp.drawString(td, 127, 187);
            disp.setFont(&_FreeSans9pt7b);
            sprintf(td, "%02d.%02d.%04d\n", days, months, years);
            disp.drawString(td, 6, 150);
            sprintf(td, "%04.1f   C\n", get_internal_temp_c());
            disp.drawString(td, 181, 193);
            disp.fillCircle(224, 198, 4, colorvalue_main4_sunrise_fg);  // outer diameter circle of the degrees symbol
            disp.fillCircle(224, 198, 2, colorvalue_main4_sunrise_bg);  // inner diameter circle of the degrees symbol
            break;
          case PHASE_MORNING:
#define colorvalue_main4_morning_bg 0xCD12
#define colorvalue_main4_morning_fg 0x4A69
            disp.setTextColor(colorvalue_main4_morning_fg, colorvalue_main4_morning_bg);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans25pt7b);
            sprintf(td, "%02d:%02d\n", hours, minutes);
            disp.drawString(td, 5, 164);
            disp.setFont(&_FreeSans12pt7b);
            sprintf(td, ":%02d\n", seconds);
            disp.drawString(td, 127, 184);
            disp.setFont(&_FreeSans9pt7b);
            sprintf(td, "%02d.%02d.%04d\n", days, months, years);
            disp.drawString(td, 6, 147);
            sprintf(td, "%04.1f   C\n", get_internal_temp_c());
            disp.drawString(td, 181, 190);
            disp.fillCircle(224, 195, 3, colorvalue_main4_morning_fg);  // outer diameter circle of the degrees symbol
            disp.fillCircle(224, 195, 1, colorvalue_main4_morning_bg);  // inner diameter circle of the degrees symbol
            break;
          case PHASE_SUNSET:
#define colorvalue_main4_sunset_bg 0x20E5
#define colorvalue_main4_sunset_fg 0xCD12
            disp.setTextColor(colorvalue_main4_sunset_fg, colorvalue_main4_sunset_bg);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans25pt7b);
            sprintf(td, "%02d:%02d\n", hours, minutes);
            disp.drawString(td, 5, 171);
            disp.setFont(&_FreeSans12pt7b);
            sprintf(td, ":%02d\n", seconds);
            disp.drawString(td, 127, 191);
            disp.setFont(&_FreeSans9pt7b);
            sprintf(td, "%02d.%02d.%04d\n", days, months, years);
            disp.drawString(td, 6, 154);
            sprintf(td, "%04.1f   C\n", get_internal_temp_c());
            disp.drawString(td, 6, 135);
            disp.fillCircle(49, 140, 3, colorvalue_main4_sunset_fg);  // outer diameter circle of the degrees symbol
            disp.fillCircle(49, 140, 1, colorvalue_main4_sunset_bg);  // inner diameter circle of the degrees symbol
            break;
          default:
            disp.setTextColor(colorvalue_main4_sunrise_fg, colorvalue_main4_sunrise_bg);
            disp.setTextDatum(TL_DATUM);
            disp.setFont(&_FreeSans35pt7b);
            sprintf(td, "%02d:%02d\n", hours, minutes);
            disp.drawString(td, 39, 64);
            disp.setFont(&_FreeSans25pt7b);
            sprintf(td, ":%02d\n", seconds);
            disp.drawString(td, 210, 80);
            disp.setFont(&_FreeSans12pt7b);
            sprintf(td, "%02d.%02d.%04d\n", days, months, years);
            disp.drawString(td, 41, 130);
            sprintf(td, "%04.1f  C\n", get_internal_temp_c());
            disp.drawString(td, 201, 130);
            disp.fillCircle(255, 135, 3, colorvalue_default_fg);  // outer diameter circle of the degrees symbol
            disp.fillCircle(255, 135, 1, colorvalue_default_bg);  // inner diameter circle of the degrees symbol
            break;
        }
        break;
      case MODE_NERD:
        display_main_bkg();
        display_batt_time_date();

        disp.setTextDatum(TL_DATUM);
        disp.setFont(&_FreeSansBold9pt7b);
        disp.setTextColor(BLACK, GREEN);
        disp.drawString("Time:", 10, 27);
        disp.setTextColor(BLACK, CYAN);
        disp.drawString("System:", 165, 27);
        disp.setTextColor(WHITE, RED);
        disp.drawString("Power:", 10, 99);
        disp.setTextColor(BLACK, ORANGE);
        disp.drawString("File Server:", 180, 99);
        disp.setTextColor(WHITE, MAGENTA);
        disp.drawString("GPS:", 10, 171);
        disp.setTextColor(BLACK, WHITE);
        disp.drawString("Geiger:", 155, 171);

        disp.setTextDatum(TL_DATUM);
        disp.setFont(&_FreeSans7pt7b);
        char y[30];

        disp.setTextColor(BLACK, GREEN);
        sprintf(y, "%02d:%02d:%02d  %02d.%02d.%04d\n", hours, minutes, seconds, days, months, years);
        disp.drawString(y, 10, 45);
        sprintf(y, "Day Phase: %d\n", rtc_get_day_phase());
        disp.drawString(y, 10, 60);
        sprintf(y, "UNIX: %d\n", unix);
        disp.drawString(y, 10, 75);

        disp.setTextColor(BLACK, CYAN);
        sprintf(y, "Audio: %d     Lights: %d", eeprom_get_audio(), eeprom_get_lights());
        disp.drawString(y, 165, 45);
        sprintf(y, "Vibrate: %d   Logger: %d", eeprom_get_vibrate(), eeprom_get_log());
        disp.drawString(y, 165, 60);
        if (get_internal_temp_c() == -1) {
          disp.drawString("Temp.: 00.0C", 165, 75);
        } else {
          sprintf(y, "Temp.: %04.1fC", get_internal_temp_c());
          disp.drawString(y, 165, 75);
        }
        if (get_internal_temp_f() == -1) {
          disp.drawString("000.0F", 265, 75);
        } else {
          sprintf(y, "%05.1fF", get_internal_temp_f());
          disp.drawString(y, 265, 75);
        }

        disp.setTextColor(WHITE, RED);
        if (batt_get_voltage() == -1) {
          disp.drawString("Batt.: 0.00V", 10, 117);
        } else {
          sprintf(y, "Batt.: %04.2fV", batt_get_voltage());
          disp.drawString(y, 10, 117);
        }
        if (batt_get_vin_voltage() == -1) {
          disp.drawString("VIN: 0.00V", 95, 117);
        } else {
          sprintf(y, "VIN: %04.2fV", batt_get_vin_voltage());
          disp.drawString(y, 95, 117);
        }
        if (batt_get_percent() == -1) {
          disp.drawString("Batt. SOC: 000", 10, 132);
        } else {
          sprintf(y, "Batt. SOC: %03d\n", batt_get_percent());
          disp.drawString(y, 10, 132);
        }
        sprintf(y, "Batt. Current: %04dmA\n", abs(get_battery_current()));
        disp.drawString(y, 10, 147);

        disp.setTextColor(BLACK, ORANGE);
        sprintf(y, "SSID: %.8s...", ft_server_ssid);
        disp.drawString(y, 180, 117);
        sprintf(y, "IP: %03d.%03d.%03d.%03d\n", file_transfer_get_ip()[0], file_transfer_get_ip()[1], file_transfer_get_ip()[2], file_transfer_get_ip()[3]);
        disp.drawString(y, 180, 132);
        sprintf(y, "Connections: %02d\n", file_transfer_get_connections());
        disp.drawString(y, 180, 147);

        disp.setTextColor(WHITE, MAGENTA);
        sprintf(y, "Lat.: %04.1f Lng.: %04.1f", gps_get_lat(), gps_get_long());
        disp.drawString(y, 10, 189);
        sprintf(y, "Alt.: %06.1f  Sat.: %02d", gps_get_alt(), gps_get_sat());
        disp.drawString(y, 10, 204);

        disp.setTextColor(BLACK, WHITE);
        if (geiger_getSieverts() == -1) {
          disp.drawString("uSv/h: 99.9", 155, 189);
        } else {
          sprintf(y, "uSv/h: %04.1f", geiger_getSieverts());
          disp.drawString(y, 155, 189);
        }
        if (geiger_getRems() == -1) {
          disp.drawString("mRem/h: 9.9", 233, 189);
        } else {
          sprintf(y, "mRem/h: %03.1f", geiger_getRems());
          disp.drawString(y, 233, 189);
        }
        if (geiger_getRoentgens() == -1) {
          disp.drawString("mR/h: 999.9", 155, 204);
        } else {
          sprintf(y, "mR/h: %05.1f", geiger_getRoentgens());
          disp.drawString(y, 155, 204);
        }
        if (geiger_getCPM() == -1) {
          disp.drawString("9999", 243, 204);
        } else {
          sprintf(y, "CPM: %04d", geiger_getCPM());
          disp.drawString(y, 243, 204);
        }
        break;
    }
  }
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_CALIBRATION) {
    display_main_bkg();
    display_batt_time_date();
    disp.setTextDatum(TC_DATUM);
    disp.setTextColor(colorvalue_default_fg, colorvalue_default_bg);
    char p[5];
    sprintf(p, "%03d\n", cpm_per_uSv_per_h);
    disp.setFont(&_FreeSans12pt7b);
    disp.drawString(p, 253, 91);
    sprintf(p, "%02d\n", cpm_per_uSv_per_h / 10);
    disp.setFont(&_FreeSans9pt7b);
    disp.drawString(p, 253, 118);
  }
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_SETTINGS) {
    display_main_bkg();
    display_batt_time_date();
#define colorvalue_page_selector 0xfA00
    switch (eeprom_get_screen_mode()) {
      case MODE_WORLD:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        break;
      case MODE_GRAPH:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        break;
      case MODE_GEIGER:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        break;
      case MODE_CLOCK:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        break;
      case MODE_NERD:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_page_selector);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_page_selector);
        break;
      default:
        disp.drawRoundRect(MODE_WORLD_X1, MODES_Y1, MODE_WORLD_X2 - MODE_WORLD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_WORLD_X1 - 1, MODES_Y1 - 1, MODE_WORLD_X2 - MODE_WORLD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1, MODES_Y1, MODE_GRAPH_X2 - MODE_GRAPH_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GRAPH_X1 - 1, MODES_Y1 - 1, MODE_GRAPH_X2 - MODE_GRAPH_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1, MODES_Y1, MODE_GEIGER_X2 - MODE_GEIGER_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_GEIGER_X1 - 1, MODES_Y1 - 1, MODE_GEIGER_X2 - MODE_GEIGER_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1, MODES_Y1, MODE_CLOCK_X2 - MODE_CLOCK_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_CLOCK_X1 - 1, MODES_Y1 - 1, MODE_CLOCK_X2 - MODE_CLOCK_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1, MODES_Y1, MODE_NERD_X2 - MODE_NERD_X1, MODES_Y2 - MODES_Y1, 4, colorvalue_default_bg);
        disp.drawRoundRect(MODE_NERD_X1 - 1, MODES_Y1 - 1, MODE_NERD_X2 - MODE_NERD_X1 + 2, MODES_Y2 - MODES_Y1 + 2, 4, colorvalue_default_bg);
        break;
    }
    if (eeprom_get_lights() == false) {
      disp.drawBitmap(19, 166, 33, 17, (uint16_t *)off);
    } else {
      disp.drawBitmap(19, 166, 33, 17, (uint16_t *)on);
    }
    if (eeprom_get_audio() == false) {
      disp.drawBitmap(19, 195, 33, 17, (uint16_t *)off);
    } else {
      disp.drawBitmap(19, 195, 33, 17, (uint16_t *)on);
    }
    if (eeprom_get_vibrate() == false) {
      disp.drawBitmap(108, 166, 33, 17, (uint16_t *)off);
    } else {
      disp.drawBitmap(108, 166, 33, 17, (uint16_t *)on);
    }
    if (eeprom_get_ft() == false) {
      disp.drawBitmap(108, 195, 33, 17, (uint16_t *)off);
    } else {
      disp.drawBitmap(108, 195, 33, 17, (uint16_t *)on);
    }
    if (eeprom_get_log() == false) {
      disp.drawBitmap(198, 166, 33, 17, (uint16_t *)off);
    } else {
      disp.drawBitmap(198, 166, 33, 17, (uint16_t *)on);
    }
  }
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_ABOUT) {
    display_main_bkg();
    display_batt_time_date();
  }
}

void display_draw_about_screen(void) {
  page_id = ID_ABOUT;
  bkg_drawn = false;
}

void display_return_to_settings(void) {
  page_id = ID_SETTINGS;
  bkg_drawn = false;
}
//-------------------------------------------------------------------------------------------------------------

void drawGraph(float *arr, bool drawSamplesInBkgColor, bool drawGraphOutlinesOnly, bool drawGraphOutlinesWithGrid, int samplesColor, bool drawArrow) {
  double x;
  double y;
  float true_x_value;
  float true_y_value;
  double gx = 30;
  double gy = 190;
  double w = 281;
  double h = 125;
  double xlo = -59;
  double xhi = 0;
  double xinc = 15;
  double ylo = 0;
  double yhi = 1.5;
  double yinc = 0.25;
  // 30,    // x graph location (lower left)
  // 190,   // y graph location (lower left)
  // 281,   // width of graph
  // 125,   // height of graph
  // -59,   // lower bound of x axis
  // 000,   // upper bound of x axis
  // 15,    // division of x axis (distance not count)
  // 00.0,  // lower bound of y axis
  // 01.5,  // upper bound of y asis
  // 0.25,  // division of y axis (distance not count)
#define colorvalue_graph_axis 0x5200

  for (true_x_value = -59; true_x_value <= 0; true_x_value++) {
    if (arr != NULL) y = arr[59 + int(true_x_value)];

    if (drawGraphOutlinesOnly) {
      // draw y scale
      disp.drawLine(gx, gy + 1, gx + w, gy + 1, colorvalue_graph_axis);
      for (float i = ylo; i <= yhi; i += yinc) {
        float temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        if (i == 0) {
          disp.drawLine(gx - 2, gy + 1, gx - 5, gy + 1, colorvalue_graph_axis);
        } else {
          disp.drawLine(gx - 2, temp, gx - 5, temp, colorvalue_graph_axis);
        }
        disp.drawFloat(i, 1, gx - 26, temp - 5);
      }
      // draw x scale
      disp.drawLine(gx - 1, gy, gx - 1, gy - h, colorvalue_graph_axis);
      for (int i = xlo; i <= xhi; i += xinc) {
        int temp = (i - xlo) * (w) / (xhi - xlo) + gx;
        if (i == xlo) {
          disp.drawLine(gx - 1, gy + 2, gx - 1, gy + 5, colorvalue_graph_axis);
        } else {
          disp.drawLine(temp, gy + 2, temp, gy + 5, colorvalue_graph_axis);
        }
        disp.drawNumber(i, temp, gy + 7);
      }
    }

    else if (drawGraphOutlinesWithGrid) {
      // draw y scale
      for (float i = ylo; i <= yhi; i += yinc) {
        float temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        if (i == 0) {
          disp.drawLine(gx, gy + 1, gx + w, gy + 1, colorvalue_graph_axis);
        } else {
          disp.drawLine(gx, temp, gx + w, temp, colorvalue_graph_axis);
        }
        disp.drawFloat(i, 1, gx - 26, temp - 5);
      }
      // draw x scale
      for (int i = xlo; i <= xhi; i += xinc) {
        int temp = (i - xlo) * (w) / (xhi - xlo) + gx;
        if (i == xlo) {
          disp.drawLine(gx - 1, gy, gx - 1, gy - h, colorvalue_graph_axis);
        } else {
          disp.drawLine(temp, gy, temp, gy - h, colorvalue_graph_axis);
        }
        disp.drawNumber(i, temp, gy + 7);
      }
    }

    //now plot the data
    if (!drawGraphOutlinesOnly && !drawGraphOutlinesWithGrid) {
      true_y_value = y;
      if (true_y_value > yhi) true_y_value = yhi;
      y = (true_y_value - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
      if (true_x_value == 0 && true_y_value != 0.0f && drawArrow) {
        disp.fillTriangle(314, y, 320, y + 6, 320, y - 6, drawSamplesInBkgColor ? colorvalue_default_bg : RED);
      }
      if (true_x_value == -59) {
        ox = -59;
        x = (true_x_value - xlo) * (w) / (xhi - xlo) + gx;
      } else {
        x = (true_x_value - xlo) * (w) / (xhi - xlo) + gx;
        if (y != gy) disp.drawLine(ox, oy, x, y, drawSamplesInBkgColor ? colorvalue_default_bg : samplesColor);
      }
      ox = x;
      oy = y;
    }
  }
}
