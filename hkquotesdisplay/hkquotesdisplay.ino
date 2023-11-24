#include "_wifi_func.h"
#include "_vfd_func.h"
#include "_sntp_func.h"
#include "_rtc_func.h"
#include "_jokes_func.h"
#include "Timer.h"

Timer timer;

void setup() {
  wifi_init();
  vfd_init();
  sntp_init();
  sntp_update();
  rtc_init();
  jokes_init();
  jokes_get();
  timer.every(20, wifi_update);
  timer.every(125, vfd_update);
  timer.every(30000, sntp_update);
  timer.every(300000, jokes_get);
}

void loop() {
  timer.update();
}
