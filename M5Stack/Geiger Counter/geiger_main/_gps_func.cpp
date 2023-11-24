#include "_gps_func.h"
#include "_rtc_func.h"
#include <TinyGPS++.h>
#include <math.h>

#define X_OFFSET_CORRECTION -8
#define Y_OFFSET_CORRECTION 13
#define GPS_SERIAL_RX_PIN 13
#define GPS_SERIAL_TX_PIN 16
/*
  Cairo, Egypt
   latitude    = 30.033333; // (φ)
   longitude   = 31.233334;   // (λ)

  Venice, Italy
   latitude    = 45.438759; // (φ)
   longitude   = 12.327145;   // (λ)

  New York, USA
   latitude    = 40.730610; // (φ)
   longitude   = -73.935242;   // (λ)

  Sydney, Australia
   latitude    = -33.870453; // (φ)
   longitude   = 151.208755;   // (λ)
*/

int mapWidth = 320;
int mapHeight = 158;

TinyGPSPlus gps;

void init_gps(void) {
  Serial2.begin(9600, SERIAL_8N1, GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN);  //RX, TX
}

void gps_update(void) {
  while (Serial2.available() > 0)
    gps.encode(Serial2.read());
  if (gps.time.isValid()) {
    rtc_set_time_date(gps_get_hours(), gps_get_minutes(), gps_get_seconds(), gps_get_days(), gps_get_months(), gps_get_years());
  }
}

float gps_get_lat(void) {
  if (gps.location.isValid()) {
    return gps.location.lat();
  } else {
    return 0;
  }
}

float gps_get_long(void) {
  if (gps.location.isValid()) {
    return gps.location.lng();
  } else {
    return 0;
  }
}

int gps_get_draw_x(void) {
  if (gps.location.isValid()) {
    // get x value
    int draw_x;
    //draw_x = ((float)gps_get_long() + 180) * (mapWidth / 360);
    draw_x = ((float)gps_get_long() + 180.0) * ((float)mapWidth / 360.0) + X_OFFSET_CORRECTION;
    return draw_x;
  } else {
    return 330;
  }
}

int gps_get_draw_y(void) {
  if (gps.location.isValid()) {
    // convert from degrees to radians
    float latRad;
    latRad = (float)gps_get_lat() * PI / 180.0;
    //latRad = latitude * PI / 180;
    // get y value
    float mercN;
    int draw_y;
    mercN = log(tan((PI / 4.0) + (latRad / 2.0)));
    draw_y = ((float)mapHeight / 2.0) - ((float)mapWidth * (float)mercN / (2.0 * PI)) + Y_OFFSET_CORRECTION;
    return draw_y;
  } else {
    return 250;
  }
}

int gps_get_alt(void) {
  if (gps.altitude.isValid()) {
    return gps.altitude.meters();
  } else {
    return 0;
  }
}

int gps_get_sat(void) {
  if (gps.satellites.isValid()) {
    return gps.satellites.value();
  } else {
    return 0;
  }
}

int gps_get_hours(void) {
  if (gps.time.isValid()) {
    return gps.time.hour();
  } else {
    return 0;
  }
}

int gps_get_minutes(void) {
  if (gps.time.isValid()) {
    return gps.time.minute();
  } else {
    return 0;
  }
}

int gps_get_seconds(void) {
  if (gps.time.isValid()) {
    return gps.time.second();
  } else {
    return 0;
  }
}

int gps_get_days(void) {
  if (gps.time.isValid()) {
    return gps.date.day();
  } else {
    return 0;
  }
}

int gps_get_months(void) {
  if (gps.time.isValid()) {
    return gps.date.month();
  } else {
    return 0;
  }
}

int gps_get_years(void) {
  if (gps.time.isValid()) {
    return gps.date.year();
  } else {
    return 0;
  }
}