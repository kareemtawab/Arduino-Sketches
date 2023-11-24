#include <Arduino.h>
#include "_serial_func.h"
#include "_l298_func.h"
#include "_battery_func.h"
#include <SoftwareSerial.h>

SoftwareSerial btserial(8, 2);

void serial_init(void) {
  Serial.begin(115200);
  while (!Serial) {}// wait for serial port to connect. Needed for native USB port only
}

void btserial_init(void) {
  btserial.begin(9600);
}

char serial_update(void) {
  return btserial.read();
}
