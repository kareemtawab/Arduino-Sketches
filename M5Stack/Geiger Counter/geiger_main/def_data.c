// ---------------------------------------
// audio
#define CONFIG_I2S_BCK_PIN 12
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_OUT_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34  // defined although not needed for now
#define CONFIG_I2S_PORT_NUMBER I2S_NUM_0
#define BUZZER_PIN 27
// ---------------------------------------
// battery
#define BATTERY_VOLTAGE_MIN_LIMIT 2600
#define BATTERY_VOLTAGE_MAX_LIMIT 4200
// ---------------------------------------
// display
#define display_interval 100
#define colorvalue_default_bg 0x0000 // colors are in are encoded in RGB565 http://www.rinkydinkelectronics.com/calc_rgb565.php
#define colorvalue_default_fg 0xF607 // colors are in are encoded in RGB565 http://www.rinkydinkelectronics.com/calc_rgb565.php

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
// ---------------------------------------
// EEPROM
#define EEPROM_SIZE 15
#define CPM_PER_USV_PER_H_ADDRESS 2
#define ALL_SETTINGS_ADDRESS 8
// ---------------------------------------
// Geiger
#define RADIATION_LEVEL_0 0.40
#define RADIATION_LEVEL_1 0.69
#define RADIATION_LEVEL_2 1.23
#define RADIATION_LEVEL_3 1.89
#define RADIATION_LEVEL_4 2.52
#define RADIATION_LEVEL_5 3.00

#define GEIGER_TUBE_PIN 36  //26
#define GM_LOG_PERIOD 500   //Logging time in milliseconds
#define Y_PLOTS_UPDATE_PERIOD 1000
#define VIBRATE_UPDATE_PERIOD 1000
// ---------------------------------------
// GPS
#define X_OFFSET_CORRECTION -8
#define Y_OFFSET_CORRECTION 13
#define GPS_SERIAL_RX_PIN 13
#define GPS_SERIAL_TX_PIN 16
#define TIMEZONE 2
// ---------------------------------------
// neopixel
#define NUM_LEDS 10
#define WS2812_DATA_PIN 25
#define BRIGHTNESS_LEVEL1 220
#define BRIGHTNESS_LEVEL2 255
// ---------------------------------------
#define wdt_timeout 8

#define rtc_interval 1000
#define display_interval 100
#define touch_interval 50
#define gps_interval 150
#define geiger_interval 250
#define serial_interval 1000
#define neopixel_interval 50
#define eeprom_interval 100
#define audio_interval 2
#define file_transfer_interval 1000
#define batt_led_vib_interval 500
// ---------------------------------------
// touch
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