#include <Arduino.h>
#include "_servo_func.h"
#include <Servo.h>
#define S1_PIN 9
#define S2_PIN 10
#define S3_PIN 11

Servo S1;

void servo_init(void) {
  S1.attach(S1_PIN);
}

void servo_update(int z) {
  S1.write(z);
}
