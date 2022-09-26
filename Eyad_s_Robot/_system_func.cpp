#include <Arduino.h>
#include "_system_func.h"
#include "_serial_func.h"
#include "_servo_func.h"
#include "_l298_func.h"
#include "_battery_func.h"

#define HORN_PIN 12

bool prescence_of_low_voltage;
int count;
int secs;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000;           // interval at which to blink (milliseconds)

void system_init (void) {
  pinMode(HORN_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  serial_init();
  servo_init();
  l298_init();
}

void system_update (void) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (get_batt_state() < 56) {
      prescence_of_low_voltage = true;
      count++;
      secs = count;
    }
    else {
      prescence_of_low_voltage = false;
      count = 0;
    }
    if (secs > 5) {
      Serial.println("BATTERY EMPTY. ALARM!");
      while (1) {
        digitalWrite(HORN_PIN, !digitalRead(HORN_PIN));
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(250);
      }
    }
  }

  Serial.print("Battery Analog Read = ");
  Serial.println(get_batt_state());

  char r = serial_update();
  switch (r) {
    case 'S':
      l298_stop();
      break;
    case 'F':
      l298_moveF();
      break;
    case 'B':
      l298_moveB();
      break;
    case 'R':
      l298_moveR();
      break;
    case 'L':
      l298_moveL();
      break;
    case 'I':
      l298_moveFR();
      break;
    case 'G':
      l298_moveFL();
      break;
    case 'J':
      l298_moveBR();
      break;
    case 'H':
      l298_moveBL();
      break;
    case '1':
      break;
    case '2':
      break;
    case '3':
      break;
    case '4':
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    case '8':
      break;
    case '9':
      break;
    case 'q':
      break;
    case 'V':
      digitalWrite(HORN_PIN, HIGH);
      break;
    case 'v':
      digitalWrite(HORN_PIN, LOW);
      break;
    case 'X':
      servo_update(100);
      break;
    case 'x':
      servo_update(0);
      break;
  }
}
