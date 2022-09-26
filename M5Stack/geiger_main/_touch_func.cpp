#include "_touch_func.h"
#include <M5Core2.h>

int touch_get (void) {
  if (M5.BtnA.wasPressed()) {
    return 1;
  }
  else if (M5.BtnB.wasPressed()) {
    return 2;
  }
  else if (M5.BtnC.wasPressed()) {
    return 3;
  }
  else {
    return 0;
  }
}
