#include <Arduino.h>
#include "_system_func.h"
#include "_rf_func.h"
#include "_motion_func.h"

int cruise_speed = 255 / 10 * 1;
int d1, d2, d3, d4;

void system_init() {
  Serial.begin(9600);
  motion_init();
  motion_moveF(255);
  delay(100);
  motion_stop();
  rf_init();
}

void system_run() {
  if (rf_loop()) {
    sscanf(getRecieved(), "%d.%d.%d.%d", &d1, &d2, &d3, &d4);
    if (d3) {
      motion_moveF(255);
    }
    if (d4) {
      motion_moveB(255);
    }
    if (d1) {
      motion_steerL();
    }
    if (d2) {
      motion_steerR();
    }

    if (!d1 && !d2) {
      motion_steerC();
    }
    if (!d3 && !d4) {
      motion_stop();
    }
  }
   else {

  }
}