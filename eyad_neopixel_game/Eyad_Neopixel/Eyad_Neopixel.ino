#include "FastLED.h"
#define NUM_LEDS 5
CRGB leds[NUM_LEDS];
#define PIN 11

const int buttonPin = 10;
int buttonState = 0;
int count = 0;

//Pin connected to ST_CP of 74HC595
int latchPin = 5;
//Pin connected to SH_CP of 74HC595
int clockPin = 6;
////Pin connected to DS of 74HC595
int dataPin = 4;
////Pin for unit digit enable
int unitdigitPin = 8;
////Pin for tens digit enable
int tensdigitPin = 7;

int LED_SEG_TAB[] = {0xfc, // 0
                     0x60, // 1
                     0xda, // 2
                     0xf2, // 3
                     0x66, // 4
                     0xb6, // 5
                     0xbe, // 6
                     0xe0, // 7
                     0xfe, // 8
                     0xf6, // 9
                     0x01, // .
                     0xee, // a
                     0x3e, // b
                     0x1a, // c
                     0x7a, // d
                     0x9e, // e
                     0x8e, // f
                     0x00  //
                    };

const byte ratio [] = {0, 255};

byte ten_e1, ten_e0;

unsigned long t1 = 0;
unsigned long t2 = 0;

uint8_t rainbowhue;
uint8_t rainbowdeltahue = 40;
uint8_t rainbowdelay = 20;
uint8_t sleepdelay = 5000;

void setup()
{
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  Serial.begin(115200);
  //set pins to input for switch
  pinMode(buttonPin, INPUT);
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(unitdigitPin, OUTPUT);
  pinMode(tensdigitPin, OUTPUT);
  setAll(CRGB::White);
  FastLED.show();
}

void loop() {

  t1++;

  buttonState = digitalRead(buttonPin);

  if (t1 >= sleepdelay && buttonState == 1) {
    
    //Serial.println(t2 - t1);

    rainbowhue++;
    fill_rainbow(leds, NUM_LEDS, rainbowhue, rainbowdeltahue);
    FastLED.show();
    FastLED.delay(rainbowdelay);

    digitalWrite(unitdigitPin, HIGH);
    digitalWrite(tensdigitPin, HIGH);
    shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[10]);
    digitalWrite(latchPin, HIGH);
    delay(1);
    digitalWrite(latchPin, LOW);
    digitalWrite(unitdigitPin, LOW);
    digitalWrite(tensdigitPin, LOW);

  }

  else {
    ten_e1 = count / 10;     //Calculate tens
    ten_e0 = count % 10;     //Calculate units

    digitalWrite(latchPin, LOW);
    if (count < 10) {
      shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[17]);
    }
    else {
      shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[ten_e1]);
    }
    digitalWrite(unitdigitPin, HIGH);
    digitalWrite(tensdigitPin, LOW);
    digitalWrite(latchPin, HIGH);
    delay(13);

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[ten_e0]);
    digitalWrite(unitdigitPin, LOW);
    digitalWrite(tensdigitPin, HIGH);
    digitalWrite(latchPin, HIGH);
    delay(13);

    t2 = millis();
  }

  if (buttonState == 0) {

    t1 = 0;

    digitalWrite(13, LOW);
    digitalWrite(unitdigitPin, HIGH);
    digitalWrite(tensdigitPin, HIGH);

    count++;
    if (count > 99) {
      count = 0;
    }

    if (count == 99) {
      setAll (CRGB::White);
      FastLED.show();
    }

redo_random:
    int pn = random(0, NUM_LEDS);
    int pr = ratio[random(0, sizeof(ratio))];
    int pg = ratio[random(0, sizeof(ratio))];
    int pb = ratio[random(0, sizeof(ratio))];
    if (pr == 0 && pg == 0 && pb == 0) {
      Serial.println("ERROR! BLACK COLOUR! REDOING RANDOM COLOUR");
      goto redo_random;
    }

    setAll(CRGB::Black);
    FastLED.show();
    setpixel(pn, pr, pg, pb);
    FastLED.show();

    delay(350);
  }

  else {
    digitalWrite(13, HIGH);
  }

}

void setpixel(int pn, int pr, int pg, int pb) {
  leds[pn].r = pr;
  leds[pn].g = pg;
  leds[pn].b = pb;
  Serial.print("LED ");
  Serial.print(pn + 1);
  Serial.print(",");
  Serial.print("\t");
  Serial.print("R ");
  Serial.print(pr);
  Serial.print(",");
  Serial.print("\t");
  Serial.print("G ");
  Serial.print(pg);
  Serial.print(",");
  Serial.print("\t");
  Serial.print("B ");
  Serial.println(pb);
}

void setAll(int pc) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = pc;
  }
}
