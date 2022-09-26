#include <M5Unified.h>
#include <M5GFX.h>
#include "_display_func.h"
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_audio_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_touch_func.h"
#include "_eeprom_func.h"
#include "splash_data.c"
#include "bkg_data.c"
#include "icons_data.c"

#define colorvalue_bg 0x1082
#define colorvalue_fg 0xF607
#define ID_SWITCHOFF   0
#define ID_MAIN        1
#define ID_CALIBRATION 2
#define ID_SETTINGS    3
#define ID_RESTART     4

extern bool geiger_audio_enabled;
extern bool neopixel_enabled;
extern int cpm_per_uSv_per_h_new;
unsigned int page_id = 1;
bool bkg_drawn;

extern int hours, minutes, seconds, days, months, years;

M5GFX disp;

void init_display(void) {
  disp.init();
  display_intro();
  display_main_bkg();
}

void display_intro(void) {
  disp.drawBitmap(0, 0, 320, 240, (uint16_t *)splash);
  audio_dingdong();
  delay(1000);
  for (int x = 0; x <= 280; x++) {
    disp.fillRect(20, 130, x, 3, WHITE);
    delay(20);
  }
  delay(2000);
}

void display_batt_time_date(void) {
  disp.setTextDatum(TL_DATUM);

  // battery voltage
  //disp.setFont(&fonts::TT1);
  //disp.setTextSize(1);
  //char s[10];
  //sprintf(s, "%.2fV", batt_get_voltage());
  //disp.drawString(s, 0, 23);  // Draw the text string in the selected GFX free font

  // battery bar
  disp.fillRoundRect(296, 5, 18, 6, 2, colorvalue_bg);
  int batt_bar_max_width = 18;
  int batt_bar_width = 0;
  batt_bar_width = (float)batt_get_percent() / 100 * (float)batt_bar_max_width;
  if (batt_get_percent() <= 100 && batt_get_percent() > 50) {
    disp.fillRoundRect(296, 5, batt_bar_width, 6, 2, GREEN);
  }
  if (batt_get_percent() <= 50 && batt_get_percent() > 25) {
    disp.fillRoundRect(296, 5, batt_bar_width, 6, 2, ORANGE);
  }
  if (batt_get_percent() <= 25 && batt_get_percent() > 0) {
    disp.fillRoundRect(296, 5, batt_bar_width, 6, 2, RED);
  }

  // time and date
  disp.setTextColor(colorvalue_fg, colorvalue_bg);

  disp.setTextDatum(TR_DATUM);
  disp.setFont(&fonts::TomThumb); //TT1
  disp.setTextSize(2);
  char timeStrbuff[64];
  sprintf(timeStrbuff, " %02d:%02d\n", hours, minutes);
  disp.drawString(timeStrbuff, 292, 3);  // Draw the text string in the selected GFX free font

  disp.setFont(&fonts::TomThumb); //TT1
  disp.setTextSize(2);
  char dateStrbuff[64];
  sprintf(dateStrbuff, "  %02d/%02d/%02d\n", days, months, years - 2000);
  disp.drawString(dateStrbuff, 325, 19);  // Draw the text string in the selected GFX free font

  // charge state icons
  if (charging_enabled()) {
    disp.drawBitmap(284, 0, 7, 16, (uint16_t *)charging_s);
  }
  else {
    disp.drawBitmap(284, 0, 7, 16, (uint16_t *)discharging_s);
  }
}

void display_main_bkg(void) {
  switch (page_id) {
    case 0: // switch off screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 320, 226, (uint16_t *)bkg);
        disp.fillRect(0, 22, 320, 204, colorvalue_bg);
        disp.drawBitmap(0, 85, 320, 60, (uint16_t *)switchoff_message);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)bar0);
        bkg_drawn = true;
        break;
      }
      else {
        break;
      }
    case 1: // default screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 320, 226, (uint16_t *)bkg);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)bar1);
        bkg_drawn = true;
        break;
      }
      else {
        break;
      }
    case 2: // calibration screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 320, 226, (uint16_t *)bkg);
        disp.fillRect(0, 22, 320, 204, colorvalue_bg);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)bar2);
        disp.drawBitmap(0, 25, 320, 200, (uint16_t *)calibration);

        bkg_drawn = true;
        break;
      }
      else {
        break;
      }
      break;
    case 3: // settings screen
      if (!bkg_drawn) {
        disp.drawBitmap(0, 0, 320, 226, (uint16_t *)bkg);
        disp.fillRect(0, 22, 320, 204, colorvalue_bg);
        disp.drawBitmap(0, 226, 320, 14, (uint16_t *)bar2);
        disp.drawBitmap(0, 25, 137, 32, (uint16_t *)settings_title);
        bkg_drawn = true;
        break;
      }
      else {
        break;
      }
      break;
  }
}

void display_update(void) {

  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_SWITCHOFF) {
    display_main_bkg();
    display_batt_time_date();
  }

  //-------------------------------------------------------------------------------------------------------------
/*
  if (page_id == ID_RESTART) {
    //display_main_bkg();
    //display_batt_time_date();
    disp.fillScreen(colorvalue_bg);
    disp.setTextDatum(TC_DATUM);
    disp.setFont(&fonts::FF20);
    disp.setTextSize(1);
    disp.drawString("RESTARTING...", 160, 100);  // Draw the text string in the selected GFX free font
  }
*/
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_MAIN) {
    display_main_bkg();
    display_batt_time_date();

    disp.fillCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 2, RED);   //Draw a filled red circle of gps location on map with radius of 3
    disp.drawCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 4, RED);   //Draw a red circle of gps location on map with radius of 4
    disp.drawCircle(gps_get_draw_x(), gps_get_draw_y() + 21, 5, RED);   //Draw a red circle of gps location on map with radius of 5

    disp.fillRect(0, 182, 72, 42, colorvalue_bg); // background behind geiger data
    disp.fillRect(238, 169, 75, 52, colorvalue_bg); // background behind gps data
    disp.fillRect(202, 153, 17, 15, colorvalue_bg); // background behind satellites count

    disp.setTextDatum(TR_DATUM);
    disp.setTextSize(1);
    disp.setFont(&fonts::FreeMonoBold12pt7b);
    if (geiger_getSieverts() > 999.9) {
      disp.drawString("*OVL*", 70, 184);  // Draw the text string in the selected GFX free font
    }
    if (geiger_getSieverts() >= 100.0 && geiger_getSieverts() < 999.9) {
      disp.drawFloat(geiger_getSieverts(), 1, 70, 184);
    }
    if (geiger_getSieverts() < 100.0) {
      disp.drawFloat(geiger_getSieverts(), 2, 70, 184);
    }
    disp.setFont(&fonts::FreeMonoBold9pt7b);
    if (geiger_getCPM() > 999999) {
      disp.drawString("*OVL*", 70, 206);  // Draw the text string in the selected GFX free font
    }
    else {
      disp.drawNumber(geiger_getCPM(), 70, 206);
    }

    disp.setTextDatum(TR_DATUM);
    disp.setFont(&fonts::FreeMonoBold9pt7b);
    disp.drawNumber(gps_get_sat(), 220, 153);

    disp.setTextDatum(TL_DATUM);
    disp.setFont(&fonts::FreeMonoBold9pt7b);
    disp.drawFloat(gps_get_lat(), 3, 240, 170);
    disp.drawFloat(gps_get_long(), 3, 240, 188);
    disp.drawFloat(gps_get_alt(), 1, 240, 206);

    if (geiger_audio_enabled == false) {
      disp.drawBitmap(16, 97, 26, 22, (uint16_t *)speaker);
      disp.drawLine(15, 93, 45, 123, RED);
      disp.drawLine(16, 93, 46, 123, RED);
      disp.drawLine(17, 93, 47, 123, RED);
      disp.drawLine(18, 93, 48, 123, RED);
    }
    else {
      disp.fillRect(15, 93, 36, 32, colorvalue_bg);
      disp.drawBitmap(16, 97, 26, 22, (uint16_t *)speaker);
    }

    if (neopixel_enabled == false) {
      disp.drawBitmap(17, 134, 28, 31, (uint16_t *)bulb);
      disp.drawLine(15, 135, 45, 165, RED);
      disp.drawLine(16, 135, 46, 165, RED);
      disp.drawLine(17, 135, 47, 165, RED);
      disp.drawLine(18, 135, 48, 165, RED);
    }
    else {
      disp.fillRect(15, 135, 36, 32, colorvalue_bg);
      disp.drawBitmap(17, 134, 28, 31, (uint16_t *)bulb);
    }
  }
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_CALIBRATION) {
    display_main_bkg();
    display_batt_time_date();
    disp.fillRect(198, 83, 105, 52, colorvalue_bg);
    disp.setTextDatum(TC_DATUM);
    disp.setTextSize(1);
    disp.setFont(&fonts::DejaVu18);
    disp.drawNumber(cpm_per_uSv_per_h_new, 253, 87);
    disp.setFont(&fonts::FreeMonoBold9pt7b);
    disp.drawNumber(cpm_per_uSv_per_h_new * 0.1, 253, 118);
  }
  //-------------------------------------------------------------------------------------------------------------
  if (page_id == ID_SETTINGS) {
    display_main_bkg();
    display_batt_time_date();
  }
}
