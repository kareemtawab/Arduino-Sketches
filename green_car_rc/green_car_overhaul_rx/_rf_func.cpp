#include <Arduino.h>
#include "_rf_func.h"
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK newcon_remote_433mhz();
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);

void rf_init() {
  newcon_remote_433mhz.init();
}

bool rf_loop() {
  if (newcon_remote_433mhz.recv(buf, &buflen)) {
    // for (int i = 0; i < buflen; i++) {
    //   Serial.print(char(buf[i]));
    // }
    // Serial.println();
    return true;
  } else {
    return false;
  }
}

char *getRecieved() {
  return buf;
}
