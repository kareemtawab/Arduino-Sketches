#include "driver/i2s.h"  // Library of I2S routines, comes with ESP32 standard install
#include "WavData.h"     // The Wav file stored in memory, should be in folder with this file
#include <esp_task_wdt.h>

#define CONFIG_I2S_BCK_PIN 12
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_OUT_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34  // defined although not needed for now
#define CONFIG_I2S_PORT_NUMBER I2S_NUM_0

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

TaskHandle_t audio_test_task_th;
//------------------------------------------------------------------------------------------------------------------------
void millis_task(void* pvParameters) {
  while (1) {
    esp_task_wdt_reset();
    Serial.println(millis() / 1000);
    vTaskDelay(1000);
  }
  vTaskDelete(NULL);
}

void audio_test_task(void* pvParameters) {
  while (!audioStopped) {
    esp_task_wdt_reset();
    // if (!audioStopped)
    playAudio();
  }
  Serial.println("d");
  vTaskDelete(audio_test_task_th);
}

void playAudio() {
  // i2s_start(i2s_num);
  size_t BytesWritten;  // Returned by the I2S write routine, we are not interested in it
  // As the WAV data for this example is in form of two 16 bit signed values we can send each four bytes direct to I2S
  for (int i = 0; i < sizeof(alarm_audio); i += 2) {  // DataIdx += 2 increase the data index to next 16 bit value (2 bytes)
    i2s_write(i2s_num, alarm_audio + i, 4, &BytesWritten, portMAX_DELAY);
  }
  // vTaskDelay(2000);
  // i2s_stop(i2s_num);
  audioStopped = true;
}

void setup() {
  Serial.begin(115200);
  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);  // ESP32 will allocated resources to run I2S
  i2s_set_pin(i2s_num, &pin_config);                  // Tell it the pins you will be using
  xTaskCreatePinnedToCore(audio_test_task, "audio_test_task", 16384, NULL, 100, &audio_test_task_th, 1);
  xTaskCreatePinnedToCore(millis_task, "millis_task", 16384, NULL, 100, NULL, 1);
}

void loop() {
}