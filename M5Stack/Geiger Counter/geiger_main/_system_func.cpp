#include "_system_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>
#include <Ticker.h>
#include <WiFi.h>
#include "driver/adc.h"
#include "_display_func.h"
#include "_audio_func.h"
#include "_neopixel_func.h"
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_touch_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_serial_func.h"
#include "_eeprom_func.h"

// SPEAKER CONFIG
#define CONFIG_I2S_BCK_PIN 12
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34
#define Speak_I2S_NUMBER I2S_NUM_0
#define MODE_MIC 0
#define MODE_SPK 1
#define DATA_SIZE 1024
#define BUZZER_PIN 35

#define rtc_interval      5
#define display_interval  350
#define touch_interval    10
#define gps_interval      1000
#define geiger_interval   10
#define serial_interval   1000
#define neopixel_interval 5
#define eeprom_interval   500
#define audio_interval    5

Ticker rtc_timer;
Ticker display_timer;
Ticker touch_timer;
Ticker gps_timer;
Ticker geiger_timer;
Ticker serial_timer;
Ticker neopixel_timer;
Ticker eeprom_timer;
Ticker audio_timer;

void init_system (void) {
  adc_power_off();
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off

  auto main_cfg = M5.config();
  main_cfg.serial_baudrate = 115200;   // default=115200. if "Serial" is not needed, set it to 0.
  main_cfg.clear_display = true;  // default=true. clear the screen when begin.
  main_cfg.output_power  = true;  // default=true. use external port 5V output.
  main_cfg.internal_imu  = false;  // default=true. use internal IMU.
  main_cfg.internal_rtc  = true;  // default=true. use internal RTC.
  main_cfg.internal_spk  = true;  // default=true. use internal speaker.
  main_cfg.internal_mic  = false;  // default=true. use internal microphone.
  main_cfg.external_imu  = false;  // default=false. use Unit Accel & Gyro.
  main_cfg.external_rtc  = false;  // default=false. use Unit RTC.
  main_cfg.external_spk  = false; // default=false. use SPK_HAT / ATOMIC_SPK
  main_cfg.led_brightness = 64;   // default= 0. system LED brightness (0=off / 255=max) (※ not NeoPixel)
  M5.begin(main_cfg);

  init_rtc();
  //rtc_set_time_date(0, 32, 30, 13, 7, 2022);
  init_display();
  init_touch();
  init_audio();
  init_gps();
  init_geiger();
  init_neopixel();
  init_eeprom();

  vibrate(100);

  serial_timer.attach_ms(serial_interval, serial_print_all);
  rtc_timer.attach_ms(rtc_interval, rtc_update);
  display_timer.attach_ms(display_interval, display_update);
  touch_timer.attach_ms(touch_interval, touch_update);
  gps_timer.attach_ms(gps_interval, gps_update);
  geiger_timer.attach_ms(geiger_interval, geiger_update);
  neopixel_timer.attach_ms(neopixel_interval, neopixel_update);
  eeprom_timer.attach_ms(eeprom_interval, eeprom_update);
  audio_timer.attach_ms(audio_interval, audio_update);
}

void run_system (void) {
  // run other commands beside main routine
}
