#include <M5Core2.h>
//#include <TimerEvent.h>
#include "_display_func.h"
#include "_audio_func.h"
#include "_neopixel_func.h"
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_touch_func.h"
#include "_gps_func.h"

int touch_id;
float _lat, _long, _alt;
int _sat;
const unsigned int display_main_interval = 100;
const unsigned int gps_interval = 25;
//TimerEvent display_main_timer;
//TimerEvent gps_timer;

void setup() {
  //kMBusModeOutput：Powered by USB or battery
  //kMBusModeInput: Powered by external 5V or DC jack
  M5.begin(true, true, true, false, kMBusModeOutput);  // void begin(bool LCDEnable=true, bool SDEnable=true, bool SerialEnable=true,bool I2CEnable=false)
  //display_main_timer.set(display_main_interval, display_main);
  //gps_timer.set(gps_interval, gps_update);
  init_display();
  init_audio();
  init_neopixel();
  init_rtc();
  init_gps();

  green_led(false);
  vibrate(100);
  //display_intro();
  vibrate(100);
  display_main_bkg();
  rtc_print_serial();
}

void loop() {
  M5.update();
  //display_main_timer.update();
  //gps_timer.update();
  touch_id = touch_get();
  switch (touch_id) {
    case 0:
      green_led(false);
      break;
    case 1:
      green_led(true);
      break;
    case 2:
      green_led(true);
      break;
    case 3:
      power_off();
      break;
  }
  display_main();
  gps_update();
  neopixel_bar(0);
  neopixel_update();
  //audio_geiger_click();
  //delay(random(50, 1000));
}
