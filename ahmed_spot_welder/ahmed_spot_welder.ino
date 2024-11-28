#include <Arduino.h>
#include <TM1637Display.h>
#include <FastLED.h>

#define DISP_CLK 4
#define DISP_DIO 5
#define PIXEL_NUM_LEDS 1
#define PIXEL_DATA_PIN 6
#define POT_PIN A0
#define TRIG_PIN 2
#define SSR_PIN 3
#define ONBOARD_LED_PIN 13

TM1637Display display(DISP_CLK, DISP_DIO);
CRGB leds[PIXEL_NUM_LEDS];
int pulse_time;
byte hsv;

void setup() {
  pinMode(TRIG_PIN, INPUT_PULLUP);
  pinMode(SSR_PIN, OUTPUT);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, PIXEL_DATA_PIN>(leds, PIXEL_NUM_LEDS);  // GRB ordering is assumed
}

void loop() {
  display.setBrightness(0xff);
  pulse_time = map(analogRead(POT_PIN), 0, 1023, 40, 1495);
  pulse_time = (pulse_time / 5 + (pulse_time % 5 > 2)) * 5;
  display.showNumberDec(pulse_time);
  fill_rainbow(&(leds[0]), 1 /*led count*/, hsv /*starting hue*/);
  hsv = random(0, 255);
  FastLED.show();
  delay(20);
  while (digitalRead(TRIG_PIN) == false) {
    leds[0] = CRGB::Black;
    FastLED.show();
    digitalWrite(SSR_PIN, HIGH);
    digitalWrite(ONBOARD_LED_PIN, HIGH);
    display.setBrightness(0xff, false);
    display.showNumberDec(pulse_time);
    delay(pulse_time);
    digitalWrite(SSR_PIN, LOW);
    digitalWrite(ONBOARD_LED_PIN, LOW);
    display.showNumberDec(pulse_time);
    while (digitalRead(TRIG_PIN) == LOW) {}  // waits forever until button is depressed/released
    display.setBrightness(0xff, true);
  }
}
