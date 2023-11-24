#include <Arduino.h>
#include <ServoTimer2.h>
#include "_motion_func.h"

#define motion_EN 12
#define M1A A2
#define M1B A1
#define STEERING_SERVO_PIN 9
#define RIGHT_STEER_ANGLE 1150
#define CENTER_STEER_ANGLE 1350
#define LEFT_STEER_ANGLE 1600

ServoTimer2 steeringServo;

void motion_init(void) {
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(motion_EN, OUTPUT);
  steeringServo.attach(STEERING_SERVO_PIN);
  analogWrite(M1A, 0);
  analogWrite(M1B, 0);
  analogWrite(motion_EN, 0);
}

void motion_moveF(int s) {
  analogWrite(M1A, s);
  analogWrite(M1B, 0);
  analogWrite(motion_EN, 255);
}

void motion_moveB(int s) {
  analogWrite(M1A, 0);
  analogWrite(M1B, s);
  analogWrite(motion_EN, 255);
}

void motion_steerR() {
  steeringServo.write(RIGHT_STEER_ANGLE);
}

void motion_steerL() {
  steeringServo.write(LEFT_STEER_ANGLE);
}

void motion_steerC(void) {
  steeringServo.write(CENTER_STEER_ANGLE);
}
void motion_stop(void) {
  analogWrite(M1A, 0);
  analogWrite(M1B, 0);
  analogWrite(motion_EN, 0);
}
