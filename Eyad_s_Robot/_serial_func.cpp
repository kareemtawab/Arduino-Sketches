#include <Arduino.h>
#include "_serial_func.h"
#include "_l298_func.h"
#include "_battery_func.h"
#include <CustomSoftwareSerial.h>

CustomSoftwareSerial* btserial;               // Declare serial

void serial_init(void) {
  Serial.begin(115200);
  btserial = new CustomSoftwareSerial(8, 2); // rx, tx
  btserial->begin(9600, CSERIAL_8N1);        // Baud rate: 9600, configuration: CSERIAL_8N1
}

char serial_update(void) {
  if (btserial->available()) {
    return btserial->read();
  }
}
