#include "_display_func.h"
#include "_rtc_func.h"
#include "_inputs_func.h"
#include "_power_func.h"
#include "bitmaps.h"
#include "Timer.h"
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/Roboto_10.h>
#include <Fonts/DejaVu_Sans_Mono_10.h>
#include <Fonts/Roboto_Black_16.h>
#include <Fonts/Roboto_Black_20.h>
#include <Fonts/Roboto_Black_48.h>
#include <Fonts/Org_01.h>
#include <Fonts/TomThumb.h>

GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS=*/30, /*DC=*/28, /*RES=*/26, /*BUSY=*/24));  // DEPG0290BS 128x296, SSD1680

bool draw_main_after_idle = true;
int display_timer_event;
extern float v_b[7], v_b_total, c_b;
extern int soc_b, t_b[5];

const unsigned char* menu_0[4] = { " , , ", " , , ", " , , ", " ,MENU, " };
const unsigned char* menu_a[4] = { "UPDATE,MODES,CONFIG.", " ,POWER,OPTIONS", "WIRE-,LESS,CONFIG.", " ,EXIT, " };
const unsigned char* menu_b[4] = { "BLUE-,TOOTH, ", " ,HOME, ", " ,WIFI, ", " ,BACK, " };
const unsigned char* menu_c[4] = { "TURN,ON,BT.", " ,HOME, ", "TURN,OFF,BT.", " ,BACK, " };
const unsigned char* menu_d[4] = { "TURN,ON,WIFI", " ,HOME, ", "TURN,OFF,WIFI", " ,BACK, " };
const unsigned char* menu_e[4] = { "30 SECS,UPDATE,MODE", " ,HOME, ", "60 SECS,UPDATE,MODE", " ,BACK, " };
const unsigned char* menu_f[4] = { "TURN,USB-A,ON", "TURN BOTH,USB POWER,OFF", "TURN,USB-C,ON", " ,BACK, " };

void init_display(void) {
  display.init();
  display.setRotation(3);
  display.setTextWrap(false);
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
  display_main();
  // drawJoystick_idle();
  drawMenu(menu_0);
}

void display_draw_menu_page(char pid) {
  // drawJoystick_left();
  // drawJoystick_idle();
  switch (pid) {
    case '0':
      drawMenu(menu_0);
      break;
    case 'a':
      drawMenu(menu_a);
      break;
    case 'b':
      drawMenu(menu_b);
      break;
    case 'c':
      drawMenu(menu_c);
      break;
    case 'd':
      drawMenu(menu_d);
      break;
    case 'e':
      drawMenu(menu_e);
      break;
    case 'f':
      drawMenu(menu_f);
      break;
  }
}

void display_main() {
  do {
    // display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, 149, 64, GxEPD_BLACK);
    display.fillRect(200, 0, 98, 15, GxEPD_BLACK);
    display.drawBitmap(149, 0, banner, 43, 64, GxEPD_BLACK);
    display.drawBitmap(196, 0, banner2, 4, 15, GxEPD_BLACK);
    display.drawBitmap(150, 6, heart, 11, 10, GxEPD_WHITE);
    display.drawRoundRect(3, 128 - 21 - 21 - 10 - 5, 29, 29, 3, GxEPD_BLACK);
    display.drawRoundRect(34, 128 - 21 - 21 - 10 - 5, 29, 29, 3, GxEPD_BLACK);
    display.drawRoundRect(65, 128 - 21 - 21 - 10 - 5, 29, 29, 3, GxEPD_BLACK);
    display.drawRoundRect(96, 128 - 21 - 21 - 10 - 5, 29, 29, 3, GxEPD_BLACK);
    display.drawRoundRect(127, 128 - 21 - 21 - 10 - 5, 29, 29, 3, GxEPD_BLACK);
    display.drawBitmap(7, 128 - 21 - 21 - 11, wifi, 21, 21, GxEPD_BLACK);
    display.drawBitmap(38, 128 - 21 - 21 - 11, bt, 21, 21, GxEPD_BLACK);
    display.drawBitmap(69, 128 - 21 - 21 - 11, asleep, 21, 21, GxEPD_BLACK);
    display.drawBitmap(100, 128 - 21 - 21 - 11, charging_s, 21, 21, GxEPD_BLACK);
    display.drawBitmap(131, 128 - 21 - 21 - 11, charging_s, 21, 21, GxEPD_BLACK);

    display.drawBitmap(222, 37, arrow_up, 11, 8, GxEPD_BLACK);
    display.drawBitmap(222, 76, arrow_down, 11, 8, GxEPD_BLACK);
    display.drawBitmap(204, 55, arrow_left, 8, 11, GxEPD_BLACK);
    display.drawBitmap(243, 55, arrow_right, 8, 11, GxEPD_BLACK);

    display.fillRect(0, 128 - 21, 296, 21, GxEPD_BLACK);
    display.drawBitmap(7, 128 - 21, temp, 21, 21, GxEPD_WHITE);
    display.drawBitmap(65, 128 - 21, temp, 21, 21, GxEPD_WHITE);
    display.drawBitmap(123, 128 - 21, temp, 21, 21, GxEPD_WHITE);
    display.drawBitmap(182, 128 - 21, temp, 21, 21, GxEPD_WHITE);
    display.drawBitmap(243, 128 - 21, ic, 21, 21, GxEPD_WHITE);
    display.drawCircle(49, 113, 2, GxEPD_WHITE);
    display.drawCircle(109, 113, 2, GxEPD_WHITE);
    display.drawCircle(167, 113, 2, GxEPD_WHITE);
    display.drawCircle(226, 113, 2, GxEPD_WHITE);
    display.drawCircle(287, 113, 2, GxEPD_WHITE);

    display.setTextColor(GxEPD_WHITE);
    display.setFont(&TomThumb);
    display.setCursor(19, 122);
    display.print("1");
    display.setCursor(77, 122);
    display.print("2");
    display.setCursor(135, 122);
    display.print("3");
    display.setCursor(194, 122);
    display.print("4");
    display.setFont(&Roboto_Black_48);
    display.setCursor(3, 37);
    display.print(v_b_total, 1);
    display.setFont(&Roboto_Black_16);
    display.setCursor(106, 17);
    display.print(soc_b);
    display.print('%');
    display.setFont(&Roboto_Black_20);
    display.setCursor(106, 37);
    display.print("volts");
    display.setFont(&TomThumb);
    display.setCursor(5, 45);
    // char* str;
    // sprintf(str, "%.1fv    %.1fv    %.1fv    %.1fv    %.1fv    %.1fv    %.1fv", v_b[0], v_b[1], v_b[2], v_b[3], v_b[4], v_b[5], v_b[6]);
    // sprintf(str, "%.1f", v_b[0]);
    for (int i = 0; i < sizeof(v_b) / sizeof(float); i++) {
      display.print(v_b[i]);
      display.print(' ');
      display.print(' ');
      display.print(' ');
      display.print(' ');
    }
    display.setFont(&Roboto_Black_16);
    display.setCursor(3, 60);
    if (abs(c_b) < 0) display.print('-');
    else display.print('+');
    if (abs(c_b) < 10) {
      display.print('0');
      display.print(c_b);
    } else display.print(c_b);
    display.print('A');
    display.setFont(&Roboto_Black_16);
    display.setCursor(26, 124);
    display.print(t_b[1]);
    display.setCursor(86, 124);
    display.print(t_b[2]);
    display.setCursor(144, 124);
    display.print(t_b[3]);
    display.setCursor(203, 124);
    display.print(t_b[4]);
    display.setCursor(264, 124);
    display.print(t_b[0]);
    display.setFont(&DejaVu_Sans_Mono_10);
    char time_to_display[18];
    sprintf(time_to_display, "%02d/%02d %02d:%02d%s", rtc_get_day(), rtc_get_month(), rtc_get_hour(), rtc_get_minute(), rtc_get_AM_PM());
    display.setCursor(203, 10);
    display.print(time_to_display);
    display.setFont(&Org_01);
    display.setCursor(75, 52);
    display.print("READING");

    display.fillRect(213, 46, 29, 29, GxEPD_WHITE);
    display.drawRoundRect(213, 46, 29, 29, 3, GxEPD_BLACK);
    display.drawCircle(227, 60, 10, GxEPD_BLACK);
    display.fillCircle(227, 60, 8, GxEPD_BLACK);
    display.fillCircle(229, 57, 1, GxEPD_WHITE);

  } while (display.nextPage());
  do {
    display.drawBitmap(150, 6, heart, 11, 10, GxEPD_BLACK);
  } while (display.nextPage());
}

void drawCentreString(char* buf, int x, int y) {
  int x1, y1;
  int w, h;
  char lines[3][10] = { "", "", "" };  // three lines with 10 characters max
  sscanf(buf, "%[^,],%[^,],%s", lines[0], lines[1], lines[2]);
  display.getTextBounds(lines[0], x, y, &x1, &y1, &w, &h);  //calc width of new string
  display.setCursor(x - w / 2, y);
  display.print(lines[0]);
  display.getTextBounds(lines[1], x, y, &x1, &y1, &w, &h);  //calc width of new string
  display.setCursor(x - w / 2, y + h + 2);
  display.print(lines[1]);
  display.getTextBounds(lines[2], x, y, &x1, &y1, &w, &h);  //calc width of new string
  display.setCursor(x - w / 2, y + h + 2 + h + 2);
  display.print(lines[2]);
}

void drawMenu(unsigned char* menu[]) {
  do {
    display.fillRect(160, 46, 44, 28, GxEPD_WHITE);
    display.fillRect(251, 46, 44, 28, GxEPD_WHITE);
    display.fillRect(195, 15, 64, 22, GxEPD_WHITE);
    display.fillRect(195, 84, 64, 22, GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&Org_01);
    drawCentreString(menu[0], 180, 55);
    drawCentreString(menu[1], 227, 21);
    drawCentreString(menu[2], 273, 55);
    drawCentreString(menu[3], 227, 90);
  } while (display.nextPage());
}

void drawJoystick_idle() {
  do {
    display.fillRect(213, 46, 29, 29, GxEPD_WHITE);
    display.drawRoundRect(213, 46, 29, 29, 3, GxEPD_BLACK);
    display.drawCircle(227, 60, 10, GxEPD_BLACK);

    display.fillCircle(227, 60, 8, GxEPD_BLACK);
    display.fillCircle(229, 57, 1, GxEPD_WHITE);
  } while (display.nextPage());
}

void drawJoystick_left() {
  do {
    display.drawCircle(227, 60, 10, GxEPD_BLACK);
    display.fillCircle(227, 60, 8, GxEPD_WHITE);
    display.fillCircle(222, 60, 8, GxEPD_BLACK);
    display.fillCircle(224, 57, 1, GxEPD_WHITE);
  } while (display.nextPage());
}

void drawJoystick_up() {
  do {
    display.drawCircle(227, 60, 10, GxEPD_BLACK);
    display.fillCircle(227, 60, 8, GxEPD_WHITE);
    display.fillCircle(227, 55, 8, GxEPD_BLACK);
    display.fillCircle(229, 52, 1, GxEPD_WHITE);
  } while (display.nextPage());
}

void drawJoystick_right() {
  do {
    display.drawCircle(227, 60, 10, GxEPD_BLACK);
    display.fillCircle(227, 60, 8, GxEPD_WHITE);
    display.fillCircle(232, 60, 8, GxEPD_BLACK);
    display.fillCircle(234, 57, 1, GxEPD_WHITE);
  } while (display.nextPage());
}

void drawJoystick_down() {
  do {
    display.drawCircle(227, 60, 10, GxEPD_BLACK);
    display.fillCircle(227, 60, 8, GxEPD_WHITE);
    display.fillCircle(227, 65, 8, GxEPD_BLACK);
    display.fillCircle(229, 62, 1, GxEPD_WHITE);
  } while (display.nextPage());
}
