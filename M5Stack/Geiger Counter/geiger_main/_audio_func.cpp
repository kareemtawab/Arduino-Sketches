#include "_audio_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>
#include "audio_data.c"
#include <driver/i2s.h>

#define CONFIG_I2S_BCK_PIN 12
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_OUT_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34
#define Speak_I2S_NUMBER I2S_NUM_0
#define BUZZER_PIN 19

extern bool geiger_audio_enabled;
extern bool gm_pulse_detected;

void init_audio(void) {
  //pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER_PIN, 0);


  // SPEAKER CONFIG
  auto spk_cfg = M5.Speaker.config();
  // Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
  spk_cfg.sample_rate = 96000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
  spk_cfg.pin_data_out = CONFIG_I2S_DATA_OUT_PIN;
  spk_cfg.pin_bck = CONFIG_I2S_BCK_PIN;
  spk_cfg.pin_ws = CONFIG_I2S_LRCK_PIN;   // LRCK
  /// use single gpio buzzer, ( need only pin_data_out )
  spk_cfg.buzzer = false;
  /// use DAC speaker, ( need only pin_data_out ) ( only GPIO_NUM_25 or GPIO_NUM_26 )
  spk_cfg.use_dac = false;
  // spk_cfg.dac_zero_level = 64; // for Unit buzzer with DAC.
  /// Volume Multiplier
  spk_cfg.magnification = 16;
  M5.Speaker.config(spk_cfg);
  M5.Speaker.begin();

  /// The setVolume function can be set the master volume in the range of 0-255.
  M5.Speaker.setVolume(255);
  /// The setAllChannelVolume function can be set the all virtual channel volume in the range of 0-255.
  M5.Speaker.setAllChannelVolume(255);
  /// The setChannelVolume function can be set the specified virtual channel volume in the range of 0-255.
  M5.Speaker.setChannelVolume(0, 255);
  /// play 2000Hz tone sound, 40 msec.
  M5.Speaker.tone(2000, 50);
}

void audio_update(void) {
  //digitalWrite(BUZZER_PIN, LOW);
  ledcWrite(0, 0);
  if (gm_pulse_detected && geiger_audio_enabled) {
    //digitalWrite(BUZZER_PIN, HIGH);
    ledcWrite(0, 128);
    gm_pulse_detected = false;
  }
}

void audio_dingdong(void) {
  if (geiger_audio_enabled) {

  }
}

void audio_warning(void) {
  if (geiger_audio_enabled) {

  }
}
