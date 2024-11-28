#include "_audio_func.h"
#include "Arduino.h"

bool isClickerOn;

void init_audio(void) {
  pinMode(64, OUTPUT);  // buzz
}

void audio_update(void) {
  digitalWrite(64, HIGH);
  digitalWrite(64, LOW);
}