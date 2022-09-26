#include <TVout.h>
#include <fontALL.h>
#include "intro.h"
#include <TimeLib.h>

TVout TV;

void setup()  {
  TV.begin(PAL, 220, 240);	//for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  TV.clear_screen();
  int w = pgm_read_byte(intro);
  int h = pgm_read_byte(intro + 1);
  TV.bitmap(0, 0, intro);
  delay(5000);
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.println("Kareem");
  delay(5000);
}

void loop() {
  //TV.clear_screen();
  TV.draw_rect(15, 30, 30, 30, WHITE);
  TV.draw_circle(50, 75, 30, WHITE);
  TV.select_font(font8x8);
  TV.set_cursor(0, 0);
  TV.print("Hello World...");
  TV.set_cursor(120, 0);
  TV.print(random(1, 99999999999));
  TV.set_cursor(95, 85);
  TV.print(analogRead(0));
  TV.set_cursor(180, 50);
  TV.print(millis() / 1000);
  TV.set_cursor(60, 180);
  TV.print(millis());
  delay(10);
}

