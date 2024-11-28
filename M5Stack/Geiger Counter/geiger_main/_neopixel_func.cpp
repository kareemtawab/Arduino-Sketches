#include "def_data.c"
#include "_neopixel_func.h"
#include "_geiger_func.h"
#include "_eeprom_func.h"
#include <FastLED.h>

int gHue = 0;  // rotating "base color" used by many of the patterns
bool alternator;

CRGB leds[NUM_LEDS];

void init_neopixel(void) {
  FastLED.addLeds<NEOPIXEL, WS2812_DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  leds[0].setRGB(0, 0, 0);
  leds[1].setRGB(0, 0, 0);
  leds[2].setRGB(0, 0, 0);
  leds[3].setRGB(0, 0, 0);
  leds[4].setRGB(0, 0, 0);
  leds[5].setRGB(0, 0, 0);
  leds[6].setRGB(0, 0, 0);
  leds[7].setRGB(0, 0, 0);
  leds[8].setRGB(0, 0, 0);
  leds[9].setRGB(0, 0, 0);
  FastLED.show();
}

void neopixel_rainbow_cycle(void) {
  fill_rainbow(leds, NUM_LEDS, gHue, 10);
  gHue++;  // slowly cycle the "base color" through the rainbow
}

void neopixel_bar(int _level) {
  switch (_level) {
    case 0:
      FastLED.setBrightness(BRIGHTNESS_LEVEL1);
      neopixel_rainbow_cycle();
      break;
    case 1:
      FastLED.setBrightness(BRIGHTNESS_LEVEL2);
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(0, 0, 0);
      leds[2].setRGB(0, 0, 0);
      leds[3].setRGB(0, 0, 0);
      leds[4].setRGB(255, 0, 0);
      leds[5].setRGB(255, 0, 0);
      leds[6].setRGB(0, 0, 0);
      leds[7].setRGB(0, 0, 0);
      leds[8].setRGB(0, 0, 0);
      leds[9].setRGB(0, 0, 0);
      break;
    case 2:
      FastLED.setBrightness(BRIGHTNESS_LEVEL2);
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(0, 0, 0);
      leds[2].setRGB(0, 0, 0);
      leds[3].setRGB(255, 0, 0);
      leds[4].setRGB(255, 0, 0);
      leds[5].setRGB(255, 0, 0);
      leds[6].setRGB(255, 0, 0);
      leds[7].setRGB(0, 0, 0);
      leds[8].setRGB(0, 0, 0);
      leds[9].setRGB(0, 0, 0);
      break;
    case 3:
      FastLED.setBrightness(BRIGHTNESS_LEVEL2);
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(0, 0, 0);
      leds[2].setRGB(255, 0, 0);
      leds[3].setRGB(255, 0, 0);
      leds[4].setRGB(255, 0, 0);
      leds[5].setRGB(255, 0, 0);
      leds[6].setRGB(255, 0, 0);
      leds[7].setRGB(255, 0, 0);
      leds[8].setRGB(0, 0, 0);
      leds[9].setRGB(0, 0, 0);
      break;
    case 4:
      FastLED.setBrightness(BRIGHTNESS_LEVEL2);
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(255, 0, 0);
      leds[2].setRGB(255, 0, 0);
      leds[3].setRGB(255, 0, 0);
      leds[4].setRGB(255, 0, 0);
      leds[5].setRGB(255, 0, 0);
      leds[6].setRGB(255, 0, 0);
      leds[7].setRGB(255, 0, 0);
      leds[8].setRGB(255, 0, 0);
      leds[9].setRGB(0, 0, 0);
      break;
    case 5:
      FastLED.setBrightness(BRIGHTNESS_LEVEL2);
      if (alternator) {
        leds[0].setRGB(0, 0, 255);
        leds[1].setRGB(0, 0, 255);
        leds[2].setRGB(0, 0, 255);
        leds[3].setRGB(0, 0, 255);
        leds[4].setRGB(0, 0, 255);
        leds[5].setRGB(255, 0, 0);
        leds[6].setRGB(255, 0, 0);
        leds[7].setRGB(255, 0, 0);
        leds[8].setRGB(255, 0, 0);
        leds[9].setRGB(255, 0, 0);
      } else {
        leds[0].setRGB(255, 0, 0);
        leds[1].setRGB(255, 0, 0);
        leds[2].setRGB(255, 0, 0);
        leds[3].setRGB(255, 0, 0);
        leds[4].setRGB(255, 0, 0);
        leds[5].setRGB(0, 0, 255);
        leds[6].setRGB(0, 0, 255);
        leds[7].setRGB(0, 0, 255);
        leds[8].setRGB(0, 0, 255);
        leds[9].setRGB(0, 0, 255);
      }
      alternator = !alternator;
      break;
  }
}

void neopixel_update(void) {
  if (eeprom_get_lights()) {
    neopixel_bar(geiger_getDangerLevel());
  } else {
    leds[0].setRGB(0, 0, 0);
    leds[1].setRGB(0, 0, 0);
    leds[2].setRGB(0, 0, 0);
    leds[3].setRGB(0, 0, 0);
    leds[4].setRGB(0, 0, 0);
    leds[5].setRGB(0, 0, 0);
    leds[6].setRGB(0, 0, 0);
    leds[7].setRGB(0, 0, 0);
    leds[8].setRGB(0, 0, 0);
    leds[9].setRGB(0, 0, 0);
  }
  FastLED.show();
}
