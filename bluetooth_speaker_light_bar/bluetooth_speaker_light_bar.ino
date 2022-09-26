#include <WS2812B.h>
#define LED_PIN   PA7  //Pin for the pixel strand. Does not have to be analog.
#define LED_TOTAL 40  //Change this to the number of LEDs in your strand.
WS2812B strip = WS2812B(LED_TOTAL);

uint8_t volume = 0;    //Holds the volume level read.
float maxVol = 5;     //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
int previousMillis = 0;        // will store the last time the LED was updated
int interval = 25;            // interval at which to blink (in milliseconds)
byte r, g, b, gr;
byte level0[LED_TOTAL] = {020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 050, 075, 120, 180, 255, 255, 180, 120, 075, 050, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020};
byte level1[LED_TOTAL] = {020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 050, 075, 120, 180, 255, 255, 255, 255, 255, 255, 180, 120, 075, 050, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020};
byte level2[LED_TOTAL] = {020, 020, 020, 020, 020, 020, 020, 020, 020, 020, 050, 075, 120, 180, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 180, 120, 075, 050, 020, 020, 020, 020, 020, 020, 020, 020, 020, 020};
byte level3[LED_TOTAL] = {020, 020, 020, 020, 020, 020, 020, 050, 075, 120, 180, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 180, 120, 075, 050, 020, 020, 020, 020, 020, 020, 020};
byte level4[LED_TOTAL] = {020, 020, 020, 020, 050, 075, 120, 180, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 180, 120, 075, 050, 020, 020, 020, 020};
byte level5[LED_TOTAL] = {050, 075, 120, 180, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 180, 120, 075, 050};

void setup() {    //Like it's named, this gets ran before any other function.
  pinMode(PC13, OUTPUT);
  pinMode(PB10, INPUT);
  pinMode(PB11, INPUT);
  pinMode(PB12, INPUT);
  pinMode(PB13, INPUT);
  pinMode(PB14, INPUT);
  strip.begin(); //Initialize the LED strand object.
  strip.show();  //Show a blank strand, just to get the LED's ready for use.
}


void loop() {  //This is where the magic happens. This loop produces each frame of the visual.
  getvolume();
  if (millis() - previousMillis > interval) {
    Wheel(gr++);
    previousMillis = millis();
  }
  switch (volume) {
    case 0:
      strip.setBrightness(140);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level0[i] / 255, g * level0[i] / 255, b * level0[i] / 255 ) );
      }
      break;
    case 1:
      strip.setBrightness(160);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level1[i] / 255, g * level1[i] / 255, b * level1[i] / 255 ) );
      }
      break;
    case 2:
      strip.setBrightness(180);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level2[i] / 255, g * level2[i] / 255, b * level2[i] / 255 ) );
      }
      break;
    case 3:
      strip.setBrightness(200);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level3[i] / 255, g * level3[i] / 255, b * level3[i] / 255 ) );
      }
      break;
    case 4:
      strip.setBrightness(225);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level4[i] / 255, g * level4[i] / 255, b * level4[i] / 255 ) );
      }
      break;
    case 5:
      strip.setBrightness(255);
      for (uint16_t i = 0; i < LED_TOTAL; i++) {
        strip.setPixelColor(i, strip.Color(r * level5[i] / 255, g * level5[i] / 255, b * level5[i] / 255 ) );
      }
      break;
  }
  strip.show();
  delay(10);
}



void getvolume() {
  if (digitalRead(PB10) == 1 && digitalRead(PB11) == 1 && digitalRead(PB12) == 1 && digitalRead(PB13) == 1 && digitalRead(PB14) == 1) {
    volume = 0;
    digitalWrite(PC13, HIGH);
  }

  if (digitalRead(PB10) == 0 && digitalRead(PB11) == 1 && digitalRead(PB12) == 1 && digitalRead(PB13) == 1 && digitalRead(PB14) == 1) {
    volume = 1;
    digitalWrite(PC13, HIGH);
  }

  if (digitalRead(PB10) == 0 && digitalRead(PB11) == 0 && digitalRead(PB12) == 1 && digitalRead(PB13) == 1 && digitalRead(PB14) == 1) {
    volume = 2;
    digitalWrite(PC13, HIGH);
  }

  if (digitalRead(PB10) == 0 && digitalRead(PB11) == 0 && digitalRead(PB12) == 0 && digitalRead(PB13) == 1 && digitalRead(PB14) == 1) {
    volume = 3;
    digitalWrite(PC13, HIGH);
  }

  if (digitalRead(PB10) == 0 && digitalRead(PB11) == 0 && digitalRead(PB12) == 0 && digitalRead(PB13) == 0 && digitalRead(PB14) == 1) {
    volume = 4;
    digitalWrite(PC13, HIGH);
  }

  if (digitalRead(PB10) == 0 && digitalRead(PB11) == 0 && digitalRead(PB12) == 0 && digitalRead(PB13) == 0 && digitalRead(PB14) == 0) {
    volume = 5;
    digitalWrite(PC13, LOW);
  }
  Serial.println(volume);
}

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    //return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    r = WheelPos * 3;
    g = 255 - WheelPos * 3;
    b = 0;
  }
  else
  {
    if (WheelPos < 170)
    {
      WheelPos -= 85;
      //return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
      r = 255 - WheelPos * 3;
      g = 0;
      b = WheelPos * 3;
    }
    else
    {
      WheelPos -= 170;
      //return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      r = 0;
      g = WheelPos * 3;
      b = 255 - WheelPos * 3;
    }
  }
}
