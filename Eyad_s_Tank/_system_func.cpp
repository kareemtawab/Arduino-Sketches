#define USE_BLUETOOTH_RC_CAR_APP
//#define USE_REMOTEXY

#define LED_PIN LED_BUILTIN
#define HORN_PIN 12
#define BL_PIN 9

#include <Arduino.h>
#include "_system_func.h"
#include "_serial_func.h"
#include "_servo_func.h"
#include "_l298_func.h"
#include "_battery_func.h"

#ifdef USE_REMOTEXY
#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h>
#include <RemoteXY.h>
#define REMOTEXY_SERIAL_RX 8
#define REMOTEXY_SERIAL_TX 2
#define REMOTEXY_SERIAL_SPEED 9600
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 217 bytes
{ 255, 7, 0, 9, 0, 210, 0, 16, 79, 0, 130, 1, 66, 3, 32, 21, 2, 4, 128, 68,
  13, 29, 9, 26, 26, 1, 2, 1, 47, 30, 15, 2, 26, 226, 134, 147, 0, 1, 2, 83,
  31, 16, 31, 2, 26, 226, 134, 183, 0, 1, 2, 66, 31, 16, 31, 2, 26, 226, 134, 182,
  0, 72, 4, 2, 5, 33, 33, 2, 26, 75, 38, 0, 0, 16, 65, 0, 0, 80, 65, 0,
  0, 16, 65, 10, 50, 34, 47, 29, 15, 4, 38, 26, 79, 80, 69, 78, 69, 68, 0, 26,
  67, 76, 79, 83, 69, 68, 0, 1, 2, 1, 31, 30, 15, 2, 26, 226, 134, 145, 0, 1,
  2, 34, 31, 29, 15, 4, 31, 240, 159, 148, 138, 0, 129, 0, 69, 6, 26, 4, 26, 67,
  114, 117, 105, 115, 101, 32, 83, 112, 101, 101, 100, 0, 129, 0, 35, 5, 28, 9, 26, 69,
  121, 97, 100, 39, 115, 0, 129, 0, 40, 15, 18, 8, 26, 84, 97, 110, 107, 0, 130, 0,
  4, 27, 92, 1, 26, 67, 1, 11, 14, 15, 8, 26, 2, 5, 129, 0, 11, 21, 14, 2,
  26, 66, 97, 116, 116, 101, 114, 121, 32, 86, 111, 108, 116, 97, 103, 101, 0
};
struct {
  // input variables
  int8_t speed_slider; // =0..100 slider position
  uint8_t back_button; // =1 if button pressed, else =0
  uint8_t right_button; // =1 if button pressed, else =0
  uint8_t left_button; // =1 if button pressed, else =0
  uint8_t hatch_button; // =1 if state is ON, else =0
  uint8_t front_button; // =1 if button pressed, else =0
  uint8_t buzzer_button; // =1 if button pressed, else =0
  float accelerometer_x;
  float accelerometer_y;
  float accelerometer_z;
  // output variables
  float voltage_bar;  // from 9 to 13
  char voltage_text[10];  // string UTF8 end zero
  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0
} RemoteXY;
#pragma pack(pop)
#endif

int count;
int cruise_speed = 255 / 10 * 1;
int secs;
unsigned long previousMillis = 0;
const long interval = 1000; // interval to check for voltage
bool offline;

void system_init (void) {
#ifdef USE_REMOTEXY
  RemoteXY_Init();
#endif

#ifdef USE_BLUETOOTH_RC_CAR_APP
  btserial_init();
#endif

  pinMode(LED_PIN, OUTPUT);
  pinMode(HORN_PIN, OUTPUT);
  pinMode(BL_PIN, INPUT);
  serial_init();
  battery_init();
  l298_init();
  for (int i = 0; i < 15; i++) {
    l298_stop();
    digitalWrite(HORN_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    l298_moveR(255);
    delay(20);
    l298_stop();
    digitalWrite(HORN_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    l298_moveL(255);
    delay(20);
  }
  l298_stop();
}

void system_update (void) {
  unsigned long currentMillis = millis();

#ifdef USE_REMOTEXY
  RemoteXY_Handler();
  if (RemoteXY.front_button && !RemoteXY.back_button && !RemoteXY.right_button && !RemoteXY.left_button) {
    l298_moveF(30);
    digitalWrite(LED_PIN, HIGH);
  }
  if (RemoteXY.back_button && !RemoteXY.front_button && !RemoteXY.right_button && !RemoteXY.left_button) {
    l298_moveB(30);
    digitalWrite(LED_PIN, HIGH);
  }
  if (RemoteXY.right_button && !RemoteXY.left_button && !RemoteXY.front_button && !RemoteXY.back_button) {
    l298_moveR(30);
    digitalWrite(LED_PIN, HIGH);
  }
  if (RemoteXY.left_button && !RemoteXY.right_button && !RemoteXY.front_button && !RemoteXY.back_button) {
    l298_moveL(30);
    digitalWrite(LED_PIN, HIGH);
  }
  if (!RemoteXY.front_button && !RemoteXY.back_button && !RemoteXY.right_button && !RemoteXY.left_button) {
    l298_stop();
    digitalWrite(LED_PIN, LOW);
  }
  if (!RemoteXY.buzzer_button) digitalWrite(HORN_PIN, LOW);
  if (RemoteXY.buzzer_button) digitalWrite(HORN_PIN, HIGH);
  sprintf(RemoteXY.voltage_text, "%.2fv", get_batt_state());
  dtostrf(2.25, 0, 2, RemoteXY.voltage_text);
#endif

#ifdef USE_BLUETOOTH_RC_CAR_APP
  (digitalRead(BL_PIN)) ? offline = false : offline = true;
  if (!offline) {
    switch (serial_update()) {
      case 'S':
        digitalWrite(LED_BUILTIN, LOW);
        l298_stop();
        break;
      case 'F':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveF(cruise_speed);
        break;
      case 'B':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveB(cruise_speed);
        break;
      case 'R':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveR(cruise_speed);
        break;
      case 'L':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveL(cruise_speed);
        break;
      case 'I':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveFR(cruise_speed);
        break;
      case 'G':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveFL(cruise_speed);
        break;
      case 'J':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveBR(cruise_speed);
        break;
      case 'H':
        digitalWrite(LED_BUILTIN, HIGH);
        l298_moveBL(cruise_speed);
        break;
      case '1':
        cruise_speed = 255 / 10 * 1;
        break;
      case '2':
        cruise_speed = 255 / 10 * 2;
        break;
      case '3':
        cruise_speed = 255 / 10 * 3;
        break;
      case '4':
        cruise_speed = 255 / 10 * 4;
        break;
      case '5':
        cruise_speed = 255 / 10 * 5;
        break;
      case '6':
        cruise_speed = 255 / 10 * 6;
        break;
      case '7':
        cruise_speed = 255 / 10 * 7;
        break;
      case '8':
        cruise_speed = 255 / 10 * 8;
        break;
      case '9':
        cruise_speed = 255 / 10 * 9;
        break;
      case 'q':
        cruise_speed = 255;
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
  else {
    digitalWrite(HORN_PIN, LOW);
    l298_stop();
    digitalWrite(LED_BUILTIN, HIGH);
    delay (50);
    digitalWrite(LED_BUILTIN, LOW);
    delay (50);
  }

#endif

  if (currentMillis - previousMillis >= interval) {
    Serial.print("Battery Voltage = ");
    Serial.println(get_batt_state());
    previousMillis = currentMillis;
    if (get_batt_state() <= 9.00) {
      count++;
      secs = count;
      Serial.println("BATTERY WARNING!!!");
      digitalWrite(HORN_PIN, HIGH);
      delay(35);
      digitalWrite(HORN_PIN, LOW);
    }
    else {
      count = 0;
    }
  }

  if (secs == 5) {
    Serial.println("BATTERY EMPTY ALARM!!!");
    while (1) {
      l298_stop();
      for (int i = 0; i < 5; i++) {
        digitalWrite(HORN_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
        delay(20);
        digitalWrite(HORN_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        delay(20);
      }
      delay(250);
    }
  }
}
