#include "_vfd_func.h"
#include "_sntp_func.h"
#include "_rtc_func.h"
#include "_quotes_func.h"
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

char daysOfTheWeek[7][5] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
int line_1_content, line_2_content;
extern bool time_obtained_sntp;
extern String quote_string;
extern bool quote_obtained;

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
  display_intro();
}

void vfd_update (void) {
  if (!time_obtained_sntp) {
    line_1_content = 0;
  }
  if (!quote_obtained) {
    line_2_content = 0;
  }
  if (time_obtained_sntp) {
    line_1_content = 1;
  }
  if (quote_obtained) {
    line_2_content = 1;
  }

  set_cursor_top_line();
  switch (line_1_content) {
    case 0: // display time needs resync
      sprintf(top, "Getting time/date ...");
      Serial.write(top);
      break;
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
    case 2:
      break;
  }

  set_cursor_bot_line();
  switch (line_2_content) {
    case 0: //
      sprintf(top, "Getting quote ...");
      Serial.write(bot);
      break;
    case 1: //
      string_scroller();
      Serial.write(bot);
      break;
    case 2:
      break;
  }
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
  String StrProcess = "                        " + quote_string + "                        ";
  result = StrProcess.substring(Li, Lii);
  Li++;
  Lii++;
  if (Li > StrProcess.length()) {
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
  sprintf(top, "Quotes + Clock");
  Serial.write(top);

  set_cursor_bot_line();
  sprintf(bot, "SNTP + RTC + WiFi");
  Serial.write(bot);

  delay(5000);
  Serial.write(0x0C);
}
