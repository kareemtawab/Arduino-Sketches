#include "_wifi_func.h"
#include "_vfd_func.h"
#include "_sntp_func.h"
#include "_rtc_func.h"
#include "_quotes_func.h"
#include "Timer.h"

Timer timer;

void setup() {
  wifi_init();
  vfd_init();
  sntp_init();
  quotes_init();
  sntp_update();
  quotes_get();
  timer.every(20, wifi_update);
  timer.every(150, vfd_update);
  timer.every(300000, sntp_update);
  timer.every(300000, quotes_get);
}

void loop() {
  timer.update();
}
