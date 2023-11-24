#include "_sntp_func.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdint.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool time_obtained_sntp;

void sntp_init (void) {
  timeClient.setTimeOffset(2 * 60 * 60);
  timeClient.begin();
}

void sntp_update (void) {
  timeClient.forceUpdate();
  if (timeClient.isTimeSet()) {
    time_obtained_sntp = true;
  }
  else {
    time_obtained_sntp = false;
  }
}

unsigned int sntp_get_epoch (void) {
  return timeClient.getEpochTime();
}
