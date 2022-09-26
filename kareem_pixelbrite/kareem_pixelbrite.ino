#include <SPI.h>
#include <SD.h>
#include <FastLED.h>

#define NUM_LEDS 144 // LED number
#define DATA_PIN 5    // your data arduino pin
#define CHIPSET WS2812B  // your LED chip type
#define COLOR_ORDER GRB

// read the analog brightness value:
//int brightValue = analogRead(brightnessInPin);
// map it to the range of the FastLED brightness:
int mappedValue;
#define MAX_BRIGHTNESS 255      // Thats full on, watch the power!
#define MIN_BRIGHTNESS 10       // set a minimum
const int brightnessInPin = A7;  // The Analog input pin that the brightness control potentiometer is attached to.

unsigned char x = 12; // matrix x size
unsigned char y = 12; // matrix y size

File fxdata;
CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(9600);

  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); //see doc for different LED strips
  // set the brightness of the strip
  mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, 255);
  FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
  Serial.print("Brightness = ");
  Serial.print((mappedValue*100)/255);
  Serial.println("%");

  for (int y = 0 ; y < NUM_LEDS ; y++)
  {
    leds[y] = CRGB::Red; // set all leds to red during setup
    FastLED.show();
    delay(10);
  }

  for (int y = 0 ; y < NUM_LEDS ; y++)
  {
    leds[y] = CRGB::Green; // set all leds to green during setup
    FastLED.show();
    delay(10);
  }

  for (int y = 0 ; y < NUM_LEDS ; y++)
  {
    leds[y] = CRGB::Blue; // set all leds to blue during setup
    FastLED.show();
    delay(10);
  }

  for (int y = 0 ; y < NUM_LEDS ; y++)
  {
    leds[y] = CRGB::White; // set all leds to white during setup
    FastLED.show();
    delay(10);
  }

  delay(3000);

  pinMode(10, OUTPUT); // CS/SS pin as output for SD library to work.
  digitalWrite(10, HIGH); // workaround for sdcard failed error...

  if (!SD.begin(4))
  {
    Serial.println("sdcard initialization failed!");
    return;
  }
  Serial.println("sdcard initialization done.");

  // test file open
  fxdata = SD.open("display.jnr");  // read only
  if (fxdata)
  {
    Serial.println("file open ok");
    fxdata.close();
  }
}

void loop()
{
  fxdata = SD.open("display.jnr");  // read only
  if (fxdata)
  {
    Serial.println("file open ok");
  }

  while (fxdata.available())
  {
    fxdata.readBytes((char*)leds, NUM_LEDS * 3);
    ledSort(1);  //1-4 possible, set your first LED's position. Change the number: 1=TL(top left),2=TR(top right),3=BL(bottom left),4=BR(bottom right)
    FastLED.show();
    mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, 255);
    FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
    delay(35); // set the speed of the animation. 20 is appx ~ 500k bauds
  }

  // close the file in order to prevent hanging IO or similar throughout time
  fxdata.close();
}

int ledSort (int modus) { //1=TL,2=TR,3=BL,4=BR, this function will rearrange the led array ;-)
  CRGB tmp[x];
  if (modus == 3 || modus == 4) {

    for (int i = 0; i < (y / 2); i++) {
      for (int j = 0; j < x; j++) {
        tmp[j] = leds[i * x + j];
        leds[i * x] = leds[(y - i - 1) * x];
        leds[(y - i - 1) * x] = tmp[j];
      }
    }
  }

  if (modus == 1 || modus == 3) {
    for (int i = 1; i < y; i = i + 2) {
      for (int j = x; j > 0; j--) {
        tmp[(x - j)] = leds[i * x + j - 1];
      }
      for (int j = 0; j < x; j++) {
        leds[i * x + j] = tmp[j];
      }
    }

  }


  if (modus == 2 | modus == 4) {
    for (int i = 0; i < y; i = i + 2) {
      for (int j = x; j > 0; j--) {
        tmp[(x - j)] = leds[i * x + j - 1];
      }
      for (int j = 0; j < x; j++) {
        leds[i * x + j] = tmp[j];
      }
    }

  }
  return 1;
}
