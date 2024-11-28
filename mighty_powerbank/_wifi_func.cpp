#include "_wifi_func.h"
#include "Arduino.h"
#include <SoftwareSerial.h>

#define WIFI_MOSFET_GATE_PIN 47

SoftwareSerial mySerial(66, 68);  // RX, TX

void init_wifi(void) {
  mySerial.begin(9600);
  pinMode(WIFI_MOSFET_GATE_PIN, OUTPUT);  // esp
  digitalWrite(WIFI_MOSFET_GATE_PIN, LOW);
}

void wifi_on(void) {
  digitalWrite(WIFI_MOSFET_GATE_PIN, HIGH);
}

void wifi_off(void) {
  digitalWrite(WIFI_MOSFET_GATE_PIN, LOW);
}

void wifi_update(void) {
  // if (mySerial.available()) {
  //   Serial.write(mySerial.read());
  // }
  // if (Serial.available()) {
  //   mySerial.write(Serial.read());
  // }
}