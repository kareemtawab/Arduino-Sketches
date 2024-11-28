#include "_system_func.h"
#include "_display_func.h"
#include "_audio_func.h"
#include "_rtc_func.h"
#include "_serial_func.h"
#include "_neopixel_func.h"
#include "_power_func.h"
#include "_inputs_func.h"
#include "_wifi_func.h"

unsigned long rtc_update_previousMillis = 0;
unsigned long inputs_update_previousMillis = 0;
unsigned long neopixel_update_previousMillis = 0;
unsigned long wifi_update_previousMillis = 0;
unsigned long bms_update_previousMillis = 0;
unsigned long display_update_previousMillis = 0;
const long rtc_update_interval = 60000;
const long inputs_update_interval = 50;
const long neopixel_update_interval = 1000;
const long wifi_update_interval = 500;
const long bms_update_interval = 1000;
const long display_update_interval = 30000;

char page_id = '0';
int joy_dir;

void init_system(void) {

  init_rtc();
  init_inputs();
  init_neopixel();
  // init_wifi();
  init_serial();
  serial_update();
  init_power();
  power_update();
  init_display();

  // rtc_set_time_date(19, 32, 30, 2, 29, 4, 24);
}

void run_system(void) {

  unsigned long currentMillis = millis();
  if (currentMillis - rtc_update_previousMillis >= rtc_update_interval) {
    rtc_update_previousMillis = currentMillis;
    rtc_update();
  }
  if (currentMillis - inputs_update_previousMillis >= inputs_update_interval) {
    inputs_update_previousMillis = currentMillis;
    inputs_update();
  }
  // if (currentMillis - neopixel_update_previousMillis >= neopixel_update_interval) {
  //   neopixel_update_previousMillis = currentMillis;
  //   // neopixel_update();
  // }
  // if (currentMillis - wifi_update_previousMillis >= wifi_update_interval) {
  //   wifi_update_previousMillis = currentMillis;
  //   // wifi_update();
  // }
  if (currentMillis - bms_update_previousMillis >= bms_update_interval) {
    bms_update_previousMillis = currentMillis;
    serial_update();
    power_update();
  }

  if (inputs_get_joystick_event()) {
    display_update_previousMillis = currentMillis;
    neopixel_white();
    switch (inputs_get_joystick_direction()) {
      case 1:
        switch (page_id) {
          case '0':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'a':
            page_id = 'e';
            display_draw_menu_page(page_id);
            break;
          case 'b':
            page_id = 'c';
            display_draw_menu_page(page_id);
            break;
          case 'c':
            power_bt_on();
            break;
          case 'd':
            wifi_on();
            break;
          case 'e':
            // activate 30 secs update
            break;
          case 'f':
            power_type_a_on();
            break;
        }
        break;
      case 2:
        switch (page_id) {
          case '0':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'a':
            page_id = 'f';
            display_draw_menu_page(page_id);
            break;
          case 'b':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'c':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'd':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'e':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'f':
            power_type_a_off();
            power_type_c_off();
            break;
        }
        break;
      case 3:
        switch (page_id) {
          case '0':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'a':
            page_id = 'b';
            display_draw_menu_page(page_id);
            break;
          case 'b':
            page_id = 'd';
            display_draw_menu_page(page_id);
            break;
          case 'c':
            power_bt_off();
            break;
          case 'd':
            wifi_off();
            break;
          case 'e':
            // activate 60 secs update
            break;
          case 'f':
            power_type_c_on();
            break;
        }
        break;
      case 4:
        switch (page_id) {
          case '0':
            page_id = 'a';
            display_draw_menu_page(page_id);
            break;
          case 'a':
            page_id = '0';
            display_draw_menu_page(page_id);
            break;
          case 'b':
            page_id = 'a';
            display_draw_menu_page(page_id);
            break;
          case 'c':
            page_id = 'b';
            display_draw_menu_page(page_id);
            break;
          case 'd':
            page_id = 'b';
            display_draw_menu_page(page_id);
            break;
          case 'e':
            page_id = 'a';
            display_draw_menu_page(page_id);
            break;
          case 'f':
            page_id = 'a';
            display_draw_menu_page(page_id);
            break;
        }
        break;
    }
    neopixel_off();
  } else {
    if (currentMillis - display_update_previousMillis >= display_update_interval) {
      display_update_previousMillis = currentMillis;
      neopixel_green();
      display_main();
      if (page_id != '0') {
        neopixel_blue();
        page_id = '0';
        display_draw_menu_page(page_id);
      }
      neopixel_off();
    }
  }
  // power_bt_on();
}
