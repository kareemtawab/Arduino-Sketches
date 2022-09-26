#include "_display_func.h"
#include "splash_data.c"
#include "bkg_data.c"
#include <M5Core2.h>
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_audio_func.h"
#include "_gps_func.h"

extern RTC_TimeTypeDef RTCtime_Now;
extern RTC_DateTypeDef RTCdate_Now;

void init_display(void) {
  M5.Lcd.setBrightness(50); // from a scale of 255
}

void display_intro(void) {
  M5.Lcd.drawBitmap(0, 0, 320, 240, (uint16_t *)splash);
  audio_dingdong();
  delay(1000);
  for (int x = 0; x <= 280; x++) {
    M5.Lcd.fillRect(20, 123, x, 3, WHITE);
    delay(30);
  }
  delay(2000);
}

void display_main_bkg(void) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.drawXBitmap(0, 17, bkg, bkgWidth, bkgHeight, TFT_LIGHTGREY);
  //M5.Lcd.setFreeFont(FS9);
  //M5.Lcd.setTextColor(RED, BLACK);
  //M5.Lcd.setTextDatum(TL_DATUM);
  //M5.Lcd.drawString("Geiger  Muller Counter", 0, 0);  // Draw the text string in the selected GFX free font
  //M5.Lcd.drawLine(50, 8, 56, 8, RED);  //Draw a straight line
  //M5.Lcd.drawRect(76, 1, 2, 2, RED);
  //M5.Lcd.drawRect(80, 1, 2, 2, RED);
}
void display_main(void) { // x= 0, y = 17
  char timeBatStrbuff[64];
  sprintf(timeBatStrbuff, "%02d:%02d B:%2d", RTCtime_Now.Hours, RTCtime_Now.Minutes, batt_get_percent());
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.setTextDatum(TR_DATUM);
  M5.Lcd.drawString(timeBatStrbuff, 320, 0);  // Draw the text string in the selected GFX free font
  //sprintf(GPSStrbuff, "%f, %f, %f, %d", gps_get_lat(), gps_get_long(), gps_get_alt(), gps_get_sat());
  //M5.Lcd.drawString(GPSStrbuff, 320, 220);  // Draw the text string in the selected GFX free font
  M5.Lcd.drawFloat(gps_get_lat(), 3, 0, 200);
  M5.Lcd.drawFloat(gps_get_long(), 3, 0, 220);
  M5.Lcd.drawFloat(gps_get_alt(), 1, 160, 200);
  M5.Lcd.drawNumber(gps_get_sat(), 160, 220);
  M5.Lcd.drawNumber(gps_get_hours(), 200, 220);
  M5.Lcd.drawNumber(gps_get_minutes(), 230, 220);
  M5.Lcd.drawNumber(gps_get_seconds(), 260, 220);
}
