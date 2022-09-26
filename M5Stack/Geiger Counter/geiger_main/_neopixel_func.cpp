#include "_neopixel_func.h"
#include "_geiger_func.h"
#include <FastLED.h>

#define NUM_LEDS 10
// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN. For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define WS2812_DATA_PIN 25
#define WS2812_CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
#define BRIGHTNESS_LEVEL1 10
#define BRIGHTNESS_LEVEL2 15
#define HUE_CHANGE_DELAY 8

int gHue = 0; // rotating "base color" used by many of the patterns
extern bool neopixel_enabled;

CRGB leds[NUM_LEDS];

void init_neopixel (void) {
  FastLED.addLeds<NEOPIXEL, WS2812_DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
}

void neopixel_rainbow_cycle (int _delay) {
  fill_rainbow(leds, NUM_LEDS, gHue, 10);
  //FastLED.delay(1000 / FRAMES_PER_SECOND);
  // insert a delay to keep the framerate modest
  EVERY_N_MILLISECONDS( _delay ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
}

void neopixel_bar (int _level) {
  switch (_level) {
    case 0:
      FastLED.setBrightness(BRIGHTNESS_LEVEL1);
      neopixel_rainbow_cycle(HUE_CHANGE_DELAY);
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
      leds[0].setRGB(255, 0, 0);
      leds[1].setRGB(255, 0, 0);
      leds[2].setRGB(255, 0, 0);
      leds[3].setRGB(255, 0, 0);
      leds[4].setRGB(255, 0, 0);
      leds[5].setRGB(255, 0, 0);
      leds[6].setRGB(255, 0, 0);
      leds[7].setRGB(255, 0, 0);
      leds[8].setRGB(255, 0, 0);
      leds[9].setRGB(255, 0, 0);
      break;
  }
}

void neopixel_update (void) {
  if (neopixel_enabled) {
    neopixel_bar(geiger_getDangerLevel());
    FastLED.show();
  }
  else {
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
}
