#include<FastLED.h>
#define NUM_LEDS 5
#define PIN 52
uint8_t rainbowhue;
uint8_t rainbowdeltahue = 40;

CRGBArray<NUM_LEDS> leds;

void setup() {
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop() {
  rainbowhue++;
  fill_rainbow(leds, NUM_LEDS, rainbowhue, rainbowdeltahue);  for (int i = 0; i < NUM_LEDS / 2; i++) {
    FastLED.delay(10);
  }
}

