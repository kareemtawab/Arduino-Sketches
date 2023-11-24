#include "_vfd_func.h"
#include "_sntp_func.h"
#include "_rtc_func.h"
#include "_jokes_func.h"
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

char daysOfTheWeek[7][5] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
int line_1_content, line_2_content;
extern bool time_obtained_sntp;
extern String jerky_bottom_string;
extern bool joke_obtained;
extern bool rtc_fail_init;
extern bool rtc_fail_lost_power;
extern bool rtc_ok;

char top[21];
char bot[21];

int Li          = 20;
int Lii         = 0;
int Ri          = -1;
int Rii         = -1;

void vfd_init (void) {
  Serial.begin(9600);
  Serial.write(0x1B);
  Serial.write(0x40);
  Serial.write(0x0C);
  Serial.write(0x0C);
  Serial.write(0x1B);
  Serial.write(0x40);
  display_intro();
}

void vfd_update (void) {
  if (rtc_ok) {
    line_1_content = 1;
  }
  if (rtc_fail_init && rtc_fail_lost_power) {
    line_1_content = 2;
  }
  if (!rtc_fail_init && rtc_fail_lost_power) {
    line_1_content = 3;
  }

  set_cursor_top_line();
  switch (line_1_content) {
    case 1: // display normal time
      char a[3];
      if (isAM()) {
        sprintf(a, "AM");
      }
      else {
        sprintf(a, "PM");
      }
      sprintf(top, "%02d:%02d:%02d%s  %02d.%02d.%02d", hourFormat12(), minute(), second(), a , day(), month(), abs(year() - 2000));
      Serial.write(top);
      break;
    case 2: // display rtc init fail
      sprintf(top, "ERR: RTC FAIL INIT!");
      Serial.write(top);
      break;
    case 3: // display rtc lost power
      sprintf(top, "ERR: RTC LOST POWER!");
      Serial.write(top);
      break;
  }

  set_cursor_bot_line();
  string_scroller();
  Serial.write(bot);
}

void set_cursor_top_line (void) {
  Serial.write(0x0B);
}

void set_cursor_bot_line (void) {
  Serial.write(0x1F);
  Serial.write(0x24);
  Serial.write(0x01);
  Serial.write(0x02);
}

void string_scroller (void) {
  String result;
  String strProcess = "                        " + jerky_bottom_string + "                        ";
  result = strProcess.substring(Li, Lii);
  Li++;
  Lii++;
  if (Li > strProcess.length()) {
    Li = 20;
    Lii = 0;
  }
  result.toCharArray(bot, sizeof(bot));;
}

void Clear_Scroll_LCD_Left(void) {
  Li = 20;
  Lii = 0;
}

void display_intro (void) {
  set_cursor_top_line();
  sprintf(top, "The Joking Clock");
  Serial.write(top);

  set_cursor_bot_line();
  sprintf(bot, "SNTP + RTC + WiFi");
  Serial.write(bot);

  delay(8000);
  Serial.write(0x0C);
}
