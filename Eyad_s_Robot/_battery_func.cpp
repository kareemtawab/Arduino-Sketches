#include <Arduino.h>
#include "_battery_func.h"

#define BATTERY_PIN A0

void battery_init (void) {
  pinMode(BATTERY_PIN, INPUT);
}

int get_batt_state(void) {
  return analogRead(BATTERY_PIN);
}
