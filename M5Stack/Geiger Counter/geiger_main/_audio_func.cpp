#include "def_data.c"
#include "_audio_func.h"
#include "_eeprom_func.h"
#include "_geiger_func.h"
#include "audio_data.c"
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include <Arduino.h>

bool audioStopped;
static const i2s_port_t i2s_num = CONFIG_I2S_PORT_NUMBER;  // i2s port number
static const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = 16000,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
  .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,  // high interrupt priority
  .dma_buf_count = 8,                        // 8 buffers
  .dma_buf_len = 1024,                       // 1K per buffer, so 8K of buffer space
  .use_apll = 0,
  .tx_desc_auto_clear = true,
  .fixed_mclk = -1
};
static const i2s_pin_config_t pin_config = {
  .bck_io_num = CONFIG_I2S_BCK_PIN,         // The bit clock connectiom, goes to pin 27 of ESP32
  .ws_io_num = CONFIG_I2S_LRCK_PIN,         // Word select, also known as word select or left right clock
  .data_out_num = CONFIG_I2S_DATA_OUT_PIN,  // Data out from the ESP32, connect to DIN on 38357A
  .data_in_num = CONFIG_I2S_DATA_IN_PIN     // we are not interested in I2S data into the ESP32
};
extern bool gm_pulse_detected;
bool isClickerOn;

void init_audio(void) {
  pinMode(BUZZER_PIN, OUTPUT);
  GPIO.out_w1ts = ((uint32_t)1 << BUZZER_PIN);
  i2s_driver_install(i2s_num, &i2s_config, 0, NULL); 
  i2s_set_pin(i2s_num, &pin_config);
  xTaskCreatePinnedToCore(play_intro_task, "play_intro_task", 4096, NULL, 100, NULL, 1);
  xTaskCreatePinnedToCore(play_alarm_task, "play_alarm_task", 4096, NULL, 100, NULL, 1);
  GPIO.out_w1tc = ((uint32_t)1 << BUZZER_PIN);
}

void audio_update(void) {
  if (geiger_getCPM() != -1 && isClickerOn) {
    GPIO.out_w1tc = ((uint32_t)1 << BUZZER_PIN);
    isClickerOn = false;
  }
  if (gm_pulse_detected && eeprom_get_audio() && !isClickerOn) {
    GPIO.out_w1ts = ((uint32_t)1 << BUZZER_PIN);
    isClickerOn = true;
    gm_pulse_detected = false;
  }
}

void play_intro_task(void *pvParameters) {
  while (!audioStopped && eeprom_get_audio()) {
    play_intro();
  }
  vTaskDelete(NULL);
}

void play_alarm_task(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    if (eeprom_get_audio() && geiger_getDangerLevel() == 5 && audioStopped) {
      play_alarm();
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
}

void play_intro() {
  // i2s_start(i2s_num);
  audioStopped = false;
  size_t BytesWritten;  // Returned by the I2S write routine, we are not interested in it
  // As the WAV data for this example is in form of two 16 bit signed values we can send each four bytes direct to I2S
  for (int i = 0; i < sizeof(intro_audio); i += 2) {  // DataIdx += 2 increase the data index to next 16 bit value (2 bytes)
    i2s_write(i2s_num, intro_audio + i, 4, &BytesWritten, portMAX_DELAY);
  }
  audioStopped = true;
  // i2s_stop(i2s_num);
}

void play_alarm() {
  // i2s_start(i2s_num);
  audioStopped = false;
  size_t BytesWritten;  // Returned by the I2S write routine, we are not interested in it
  // As the WAV data for this example is in form of two 16 bit signed values we can send each four bytes direct to I2S
  for (int i = 0; i < sizeof(alarm_audio); i += 2) {  // DataIdx += 2 increase the data index to next 16 bit value (2 bytes)
    i2s_write(i2s_num, alarm_audio + i, 4, &BytesWritten, portMAX_DELAY);
  }
  audioStopped = true;
  // i2s_stop(i2s_num);
}