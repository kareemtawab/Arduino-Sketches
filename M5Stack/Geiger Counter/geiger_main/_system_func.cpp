#include "_system_func.h"
#include "_touch_func.h"
#include "_display_func.h"
#include "_audio_func.h"
#include "_neopixel_func.h"
#include "_rtc_func.h"
#include "_batt_led_vib_func.h"
#include "_gps_func.h"
#include "_geiger_func.h"
#include "_serial_func.h"
#include "_eeprom_func.h"
#include "_file_transfer_func.h"
#include <driver/adc.h>
#include <esp_task_wdt.h>
#include <Wire.h>

#define SDA 21
#define SCL 22

#define wdt_timeout 8

#define rtc_interval 1000
#define display_interval 100
#define touch_interval 50
#define gps_interval 500
#define geiger_interval 250
#define serial_interval 1000
#define neopixel_interval 100
#define eeprom_interval 100
#define audio_interval 2
#define file_transfer_interval 1000
#define batt_led_vib_interval 500

void geiger_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(geiger_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    geiger_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void gps_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(gps_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    gps_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void eeprom_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(eeprom_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    eeprom_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void neopixel_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(neopixel_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    neopixel_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void touch_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(touch_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    touch_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void rtc_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(rtc_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    rtc_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void serial_print_all_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(serial_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    serial_print_all();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void batt_led_vib_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(batt_led_vib_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    batt_led_vib_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void audio_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(audio_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    audio_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void display_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(display_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    display_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void file_transfer_update_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(file_transfer_interval);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    file_transfer_update();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void init_system(void) {
  adc_power_off();
  btStop();
  esp_bt_controller_disable();

  esp_task_wdt_init(wdt_timeout, true);
  esp_task_wdt_add(NULL);
  esp_task_wdt_reset();

  Wire.begin(SDA, SCL);
  delay(10);

  batt_led_vib_init();
  init_serial();
  init_rtc();
  // rtc_set_time_date(0, 32, 30, 13, 7, 2022);
  init_eeprom();
  init_geiger();
  init_audio();
  init_touch();
  init_display();
  init_gps();
  init_neopixel();
  init_file_transfer();

  vibrate(100);

  // xTaskCreatePinnedToCore(serial_print_all_task, "serial_print_all_task", 9216, NULL, 20, NULL, 1);
  xTaskCreatePinnedToCore(batt_led_vib_update_task, "batt_led_vib_update_task", 4096, NULL, 100, NULL, 1);
  xTaskCreatePinnedToCore(rtc_update_task, "rtc_update_task", 2048, NULL, 20, NULL, 1);
  xTaskCreatePinnedToCore(eeprom_update_task, "eeprom_update_task", 4096, NULL, 60, NULL, 1);
  xTaskCreatePinnedToCore(geiger_update_task, "geiger_update_task", 4096, NULL, 100, NULL, 1);
  xTaskCreatePinnedToCore(audio_update_task, "audio_update_task", 4096, NULL, 100, NULL, 1);
  xTaskCreatePinnedToCore(touch_update_task, "touch_update_task", 4096, NULL, 90, NULL, 1);
  xTaskCreatePinnedToCore(display_update_task, "display_update_task", 81920, NULL, 100, NULL, 1);
  xTaskCreatePinnedToCore(gps_update_task, "gps_update_task", 4096, NULL, 20, NULL, 1);
  xTaskCreatePinnedToCore(neopixel_update_task, "neopixel_update_task", 1024, NULL, 40, NULL, 1);
  xTaskCreatePinnedToCore(file_transfer_update_task, "file_transfer_update_task", 16384, NULL, 95, NULL, 1);
}

void run_system(void) {
  esp_task_wdt_reset();
  Serial.print("free heap size: ");
  Serial.print(esp_get_free_heap_size() / 1024);
  Serial.println(" Kbytes");
  delay(1000);
}
