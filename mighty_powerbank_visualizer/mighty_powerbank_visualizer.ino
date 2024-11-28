#include <arduinoFFT.h>
#include <FastLED.h>

#define SAMPLES 64      // Must be a power of 2
#define LED_PIN 7       // Data pin to LEDS
#define INDICATOR_PIN 13
#define BRIGHTNESS 150  // LED information
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BUTTON_PIN 2
#define xres 32  // Total number of  columns in the display
#define yres 8   // Total number of  rows in the display
#define NUM_LEDS xres* yres

double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long idleStart;
int Intensity[xres] = {};  // initialize Frequency Intensity to zero
int Displacement = 1;
bool drawAnimation, highLevel;
byte leds_pattern[8][32] = { { 255, 254, 253, 252, 251, 250, 249, 248, 191, 190, 189, 188, 187, 186, 185, 184, 127, 126, 125, 124, 123, 122, 121, 120, 63, 62, 61, 60, 59, 58, 57, 56 },
                             { 247, 246, 245, 244, 243, 242, 241, 240, 183, 182, 181, 180, 179, 178, 177, 176, 119, 118, 117, 116, 115, 114, 113, 112, 55, 54, 53, 52, 51, 50, 49, 48 },
                             { 239, 238, 237, 236, 235, 234, 233, 232, 175, 174, 173, 172, 171, 170, 169, 168, 111, 110, 109, 108, 107, 106, 105, 104, 47, 46, 45, 44, 43, 42, 41, 40 },
                             { 231, 230, 229, 228, 227, 226, 225, 224, 167, 166, 165, 164, 163, 162, 161, 160, 103, 102, 101, 100, 99, 98, 97, 96, 39, 38, 37, 36, 35, 34, 33, 32 },
                             { 223, 222, 221, 220, 219, 218, 217, 216, 159, 158, 157, 156, 155, 154, 153, 152, 95, 94, 93, 92, 91, 90, 89, 88, 31, 30, 29, 28, 27, 26, 25, 24 },
                             { 215, 214, 213, 212, 211, 210, 209, 208, 151, 150, 149, 148, 147, 146, 145, 144, 87, 86, 85, 84, 83, 82, 81, 80, 23, 22, 21, 20, 19, 18, 17, 16 },
                             { 207, 206, 205, 204, 203, 202, 201, 200, 143, 142, 141, 140, 139, 138, 137, 136, 79, 78, 77, 76, 75, 74, 73, 72, 15, 14, 13, 12, 11, 10, 9, 8 },
                             { 199, 198, 197, 196, 195, 194, 193, 192, 135, 134, 133, 132, 131, 130, 129, 128, 71, 70, 69, 68, 67, 66, 65, 64, 7, 6, 5, 4, 3, 2, 1, 0 } };

CRGB leds[NUM_LEDS];            // Create LED Object
arduinoFFT FFT = arduinoFFT();  // Create FFT object

void setup() {
  // Serial.begin(115200);
  pinMode(INDICATOR_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);  //Initialize LED strips
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  //   Begin FFT operations
  ADCSRA = 0b11100101;  // set ADC to free running mode   and set pre-scalar to 32 (0xe5)
  ADMUX = 0b00000000;   // use pin A0 and   external voltage reference
}

void loop() {
  getSamples();
  checkData();
  displayUpdate();
}

void getSamples() {
  // for (int i = 0; i < SAMPLES; i++) {
  //   vReal[i] = analogRead(MIC_IN);
  //   // Serial.print(vReal[i]);
  //   // Serial.print('\t');
  //   vImag[i] = 0;
  // }
  // // Serial.println();

  // ++ Sampling
  for (int i = 0; i < SAMPLES; i++) {
    while (!(ADCSRA & 0x10))
      ;                     //   wait for ADC to complete current conversion ie ADIF bit set
    ADCSRA = 0b11110101;    // clear ADIF bit so that ADC can do next operation (0xf5)
    int value = ADC - 512;  // Read from ADC and subtract DC offset caused value
    vReal[i] = value / 8;   // Copy to bins after compressing
    vImag[i] = 0;
  }

  //FFT
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  //Update Intensity Array
  for (int i = 2; i < (xres * Displacement) + 2; i += Displacement) {
    vReal[i] = constrain(vReal[i], 0, 10);          // set max value for input data
    vReal[i] = map((int)vReal[i], 0, 10, 0, yres - 1);  // map data to fit our display

    Intensity[(i / Displacement) - 2]--;               // Decrease displayed value
    if (vReal[i] > Intensity[(i / Displacement) - 2])  // Match displayed value to measured value
      Intensity[(i / Displacement) - 2] = vReal[i];
  }

  // for (int i = 0; i < xres; i++) {
  //   Serial.print(Intensity[i]);
  //   Serial.print('\t');
  // }
  // Serial.println();
}

void checkData() {
  int count = 0;
  for (int i = 0; i < xres; i++) {
    if (Intensity[i] == 0) {
      count++;
      highLevel = false;
    } else if (Intensity[i] == 7) highLevel = true;
  }
  if (count == xres) {
    idleStart++;
    // Serial.println(idleStart);
    drawAnimation = true;

  } else {
    drawAnimation = false;
    idleStart = 0;
  }
}

void displayUpdate() {
  if (highLevel) digitalWrite(INDICATOR_PIN, HIGH);
  else digitalWrite(INDICATOR_PIN, LOW);
  if (!drawAnimation) {
    int color = 0;
    for (int x = 0; x < xres; x++) {
      for (int y = 0; y < yres; y++) {
        if (y <= Intensity[x]) {  // Light everything within the intensity range
          leds[leds_pattern[yres - 1 - y][x]] = CHSV(color, 255, BRIGHTNESS);
        } else {  // Everything outside the range goes dark
          leds[leds_pattern[yres - 1 - y][x]] = CHSV(color, 255, 0);
        }
      }
      color += 255 / xres;  // Increment the Hue to get the Rainbow
    }
    FastLED.show();
  }
  if (drawAnimation && idleStart > 200) {
    FastLED.clear(false);
    int p = random(0, NUM_LEDS);
    leds[p] = CHSV(random(255), 255, 255);
    FastLED.show();
  }
}