#include "_sntp_func.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool time_obtained_sntp;

void sntp_init (void) {
  timeClient.setTimeOffset(2 * 60 * 60);
  timeClient.begin();
}

bool sntp_update (void) {
  timeClient.forceUpdate();
  if (timeClient.isTimeSet()) {
    time_obtained_sntp = true;
  }
  else {
    time_obtained_sntp = false;
  }
  if (time_obtained_sntp) {
    setTime(sntp_get_epoch());
    return true;
  }
  else {
    return false;
  }
}

long long int sntp_get_epoch (void) {
  return timeClient.getEpochTime();
}
