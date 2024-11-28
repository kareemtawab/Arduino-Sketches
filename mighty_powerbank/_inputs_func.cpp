#include "_inputs_func.h"
#include "Arduino.h"
#include "OneButton.h"

#define JOY_UP_PIN 35
#define JOY_DOWN_PIN 37
#define JOY_LEFT_PIN 41
#define JOY_RIGHT_PIN 39
#define IDLE_TIMEOUT 10000

OneButton j_up(JOY_UP_PIN, true);
OneButton j_down(JOY_DOWN_PIN, true);
OneButton j_left(JOY_LEFT_PIN, true);
OneButton j_right(JOY_RIGHT_PIN, true);

float command_id;
bool joystick_event_old, joystick_event;
int joy_direction;
unsigned long idle_time_start, idle_millis_counter;
bool idle_event;

void init_inputs() {
  j_up.attachClick(up_click);
  // j_up.attachLongPressStart(up_long_start);

  j_down.attachClick(down_click);
  // j_down.attachLongPressStart(down_long_start);

  j_left.attachClick(left_click);
  // j_left.attachLongPressStart(left_long_start);

  j_right.attachClick(right_click);
  // j_right.attachLongPressStart(right_long_start);
}

void up_click() {
  joystick_event = true;
  joy_direction = 2;
}

void up_long_start() {
  Serial.println("up_long_start");
}

void down_click() {
  joystick_event = true;
  joy_direction = 4;
}

void down_long_start() {
  Serial.println("down_long_start");
}

void left_click() {
  joystick_event = true;
  joy_direction = 1;
}

void left_long_start() {
  Serial.println("left_long_start");
}

void right_click() {
  joystick_event = true;
  joy_direction = 3;
}

void right_long_start() {
  Serial.println("right_long_start");
}

void inputs_update() {
  j_up.tick();
  j_down.tick();
  j_left.tick();
  j_right.tick();
}

bool inputs_get_joystick_event() {
  if (joystick_event) {
    joystick_event = false;
    return true;
  } else {
    return false;
  }
}

int inputs_get_joystick_direction() {
  int joy_direction_temp = joy_direction;
  joy_direction = 0;
  return joy_direction_temp;
}