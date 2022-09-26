#include "_gps_func.h"
#include <M5Core2.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;

void init_gps (void) {
  Serial2.begin(9600, SERIAL_8N1, 13, 16);
}

void gps_update (void) {
  while (Serial2.available() > 0)
    gps.encode(Serial2.read());
}

float gps_get_lat (void) {
  if (gps.location.isValid()) {
    return gps.location.lat();
  }
  else {
    return -1;
  }
}

float gps_get_long (void) {
  if (gps.location.isValid()) {
    return gps.location.lng();
  }
  else {
    return -1;
  }
}

int gps_get_alt (void) {
  if (gps.altitude.isValid()) {
    return gps.altitude.meters();
  }
  else {
    return -1;
  }
}

int gps_get_sat (void) {
  if (gps.satellites.isValid()) {
    return gps.satellites.value();
  }
  else {
    return -1;
  }
}

int gps_get_hours (void) {
  if (gps.time.isValid()) {
    return gps.time.hour();
  }
  else {
    return -1;
  }
}

int gps_get_minutes (void) {
  if (gps.time.isValid()) {
    return gps.time.minute();
  }
  else {
    return -1;
  }
}

int gps_get_seconds (void) {
  if (gps.time.isValid()) {
    return gps.time.second();
  }
  else {
    return -1;
  }
}

void gps_print_serial (void) {
  char Strbuff[64];
  sprintf(Strbuff, "GPS - Long.: %f, Lat.: %f, Alt.: %f, Sat.: %d", gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
}
