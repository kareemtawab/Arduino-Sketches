#include <Arduino.h>
#include "_battery_func.h"

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  float result;
  result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result;
}

#define BATTERY_PIN A0

void battery_init (void) {
  pinMode(BATTERY_PIN, INPUT);
}

float get_batt_state(void) {
  return mapf(float(analogRead(BATTERY_PIN)), 769.00f, 1023.00f, 11.82f, 15.73f);
}
