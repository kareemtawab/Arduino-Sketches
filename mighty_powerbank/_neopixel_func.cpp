#include "_neopixel_func.h"
#include <FastLED.h>

#define NUM_LEDS 1
#define WS2812_DATA_PIN 33
#define BRIGHTNESS_LEVEL 255

int gHue = 0;  // rotating "base color" used by many of the patterns

CRGB leds[NUM_LEDS];

void init_neopixel(void) {
  FastLED.addLeds<NEOPIXEL, WS2812_DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(BRIGHTNESS_LEVEL);
  leds[0].setRGB(255, 255, 255);
  FastLED.show();
  delay(50);
  leds[0].setRGB(0, 0, 0);
  FastLED.show();
}

void neopixel_rainbow_cycle(void) {
  fill_rainbow(leds, NUM_LEDS, gHue, 10);
  gHue++;  // slowly cycle the "base color" through the rainbow
}

void neopixel_red(void) {
  leds[0] = CRGB::Red;
  FastLED.show();
}

void neopixel_green(void) {
  leds[0] = CRGB::Green;
  FastLED.show();
}

void neopixel_blue(void) {
  leds[0] = CRGB::Blue;
  FastLED.show();
}

void neopixel_white(void) {
  leds[0] = CRGB::White;
  FastLED.show();
}

void neopixel_off(void) {
  leds[0] = CRGB::Black;
  FastLED.show();
}

void neopixel_update(void){
  neopixel_green();
  delay(50);
  neopixel_off();
}