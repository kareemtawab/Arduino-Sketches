#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "Timer.h"

#define P_SERIAL D8   //pin 14 on the 75HC595 serial/data
#define P_LATCH  D7   //pin 12 on the 75HC595 latch
#define P_CLOCK  D6   //pin 11 on the 75HC595 clock
#define ZERO_PADDING 8
#define REGISTERS 3
#define PINS REGISTERS * 8
#define numberCountToDraw 11
#define rowsToDrawForEveryNumber 7
#define timeToSwitchOffWiFi 3600 // seconds
IPAddress local_IP(192, 168, 1, 135);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 168);  //optional
IPAddress secondaryDNS(8, 8, 8, 8);      //optional

// Will point to our light patterns. A byte is an 8-bit BINARY variable to match the 8 lights
byte * registerPatterns;
int finalNumberForDisplay;
bool gotNTPTime, isNight, isBlink, blankDisplay;
const byte pixelAnodeBitMap[18] = {7, 6, 5, 4, 6, 3, 1, 0, 15, 14, 0, 13, 12, 11, 10, 9, 11, 8};
const byte pixelCathodeBitMap[rowsToDrawForEveryNumber] =
{
  22,
  21,
  20,
  19,
  18,
  17,
  16
};

const byte numberBitMap[numberCountToDraw][rowsToDrawForEveryNumber] = {
  { 0b111,
    0b101,
    0b101,
    0b101,
    0b101,
    0b101,
    0b111,
  }, // num 0

  { 0b001,
    0b001,
    0b001,
    0b001,
    0b001,
    0b001,
    0b001,
  }, // num 1

  { 0b111,
    0b001,
    0b001,
    0b111,
    0b100,
    0b100,
    0b111,
  }, // num 2

  { 0b111,
    0b001,
    0b001,
    0b111,
    0b001,
    0b001,
    0b111,
  }, // num 3

  { 0b101,
    0b101,
    0b101,
    0b111,
    0b001,
    0b001,
    0b001,
  }, // num 4

  { 0b111,
    0b100,
    0b100,
    0b111,
    0b001,
    0b001,
    0b111,
  }, // num 5

  { 0b111,
    0b100,
    0b100,
    0b111,
    0b101,
    0b101,
    0b111,
  }, // num 6

  { 0b111,
    0b101,
    0b001,
    0b001,
    0b001,
    0b001,
    0b001,
  }, // num 7

  { 0b111,
    0b101,
    0b101,
    0b111,
    0b101,
    0b101,
    0b111,
  }, // num 8

  { 0b111,
    0b101,
    0b101,
    0b111,
    0b001,
    0b001,
    0b111,
  }, // num 9

  { 0b000,
    0b000,
    0b000,
    0b000,
    0b000,
    0b000,
    0b000,
  } // blank
};

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);
WiFiUDP ntpUDP;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);
time_t ntptime;
Timer t;

void handleRoot() {
  server.send(200, "text/plain", "LED Matrix Clock\nGo to 'http://ip_address/update' to perform OTA update");
}

void clearRegisters() {
  //yield();
  // For each register, set it's pattern to 0 (which, rember is 00000000 in bytes, meaning all lights off)
  for (size_t i = 0; i < REGISTERS; i++) {
    registerPatterns[i] = 0;
  }
}

// Takes the overall pin number and figures out which register it's on and what pin it would be on that register
void writePinToByte(int pin, bool state) {
  // Which register are we looking at? 0=first, 1=second, etc. "/" will round down to an int
  int r = pin / 8;

  // Which pin on that register, finding the remainder of the pin division will give us 0-7
  int q = pin % 8;

  // Go through each of our stored patterns and write them back to the registers in order
  for (size_t i = 0; i < REGISTERS; i++) {

    // Point to the current pattern for this register
    byte * pattern = &registerPatterns[i];

    // If this is the register that the pin we are editing is on...
    if (i == r) {
      // Take the "pattern" that holds the 8 bits, change the binary digit at position "q" (0-7) to "state" (LOW = 0, HIGH = 1)
      bitWrite(*pattern, q, state);
    }
  }
}

void writeToRegisters() {
  //yield();
  // Lock the registers to ready them to be written
  //digitalWrite(P_LATCH, LOW);
  GPOC = (1 << P_LATCH); // direct faster gpio control

  // Go through each of our stored patterns and write them back to the registers in order
  for (size_t i = 0; i < REGISTERS; i++) {

    // Point to the current pattern for this register
    byte * pattern = &registerPatterns[i];

    // Write the pattern back to the register chips byte by byte
    shiftOut(P_SERIAL, P_CLOCK, MSBFIRST, *pattern);

    // Activate to show the byte pattern as a binary number
    //SPrintZeroPadBin(*pattern);
    //if (i != REGISTERS - 1) {
    //  Serial.print('-');
    //}
  }
  //Serial.println();
  // Unlock the registers
  //digitalWrite(P_LATCH, HIGH);
  GPOS = (1 << P_LATCH); // direct faster gpio control
}

void drawNumberOnDisplay(long number) {
  if (!blankDisplay) {
    drawSingleDigitNumberAtDigitLocation(0, (number / 100000));
    drawSingleDigitNumberAtDigitLocation(1, (number / 10000) % 10);
    drawSingleDigitNumberAtDigitLocation(2, (number / 1000) % 10);
    drawSingleDigitNumberAtDigitLocation(3, (number / 100) % 10);
    drawSingleDigitNumberAtDigitLocation(4, (number / 10) % 10);
    drawSingleDigitNumberAtDigitLocation(5, number % 10);
  }
  else {
    clearRegisters();
    writeToRegisters();
  }

  if (isBlink) {
    drawSingleDigitNumberAtDigitLocation(13, 0); // right colon on
    if (isNight) {
      drawSingleDigitNumberAtDigitLocation(11, 0); // PM mark on with left colon
    }
    else {
      drawSingleDigitNumberAtDigitLocation(12, 0); // left colon on independantely
    }
  }
  else {
    if (isNight) {
      drawSingleDigitNumberAtDigitLocation(10, 0); // PM mark on independantely
    }
  }
}

void drawSingleDigitNumberAtDigitLocation(int digit, int number) {
  if (digit >= 0 && digit <= 5) {
    for (int c = 0; c <= 2; c++) {
      clearRegisters();
      for (int r = 0; r <= 6; r++) {
        if (numberBitMap[number][r] & (1 << (2 - c))) {
          drawLED (digit, c, r);
          writeToRegisters();
        }
      }
    }
  }
  else {
    clearRegisters();
    for (int r = 0; r <= 6; r++) {
      drawLED (digit, 0, r);
      writeToRegisters();
    }
  }
}

void drawTest() {
  for (int d = 0; d <= 5; d++) {
    for (int c = 0; c <= 2; c++) {
      for (int r = 0; r <= 6; r++) {
        //clearRegisters();
        drawLED(d, c , r);
        writeToRegisters();
        delay(20);
      }
    }
  }
}

void drawLED (int digit, int col, int row) {
  switch (digit) {
    case 0:
    case 2:
    case 4:
      switch (col) {
        case 0:
          if (digit == 0) {
            writePinToByte(pixelAnodeBitMap[0], true);
          }
          if (digit == 2) {
            writePinToByte(pixelAnodeBitMap[6], true);
          }
          if (digit == 4) {
            writePinToByte(pixelAnodeBitMap[12], true);
          }
          switch (row) {
            case 0:
              writePinToByte(pixelCathodeBitMap[0], true);
              break;
            case 1:
              writePinToByte(pixelCathodeBitMap[1], true);
              break;
            case 2:
              writePinToByte(pixelCathodeBitMap[2], true);
              break;
            case 3:
              writePinToByte(pixelCathodeBitMap[3], true);
              break;
            case 4:
              writePinToByte(pixelCathodeBitMap[4], true);
              break;
            case 5:
              writePinToByte(pixelCathodeBitMap[5], true);
              break;
            case 6:
              writePinToByte(pixelCathodeBitMap[6], true);
              break;
          }
          break;
        case 1:
          if (digit == 0) {
            writePinToByte(pixelAnodeBitMap[1], true);
          }
          if (digit == 2) {
            writePinToByte(pixelAnodeBitMap[7], true);
          }
          if (digit == 4) {
            writePinToByte(pixelAnodeBitMap[13], true);
          }
          if (row != 1 && row != 2 && row != 4 && row != 5) {
            switch (row) {
              case 0:
                writePinToByte(pixelCathodeBitMap[1], true);
                break;
              case 3:
                writePinToByte(pixelCathodeBitMap[3], true);
                break;
              case 6:
                writePinToByte(pixelCathodeBitMap[6], true);
                break;
            }
          }
          break;
        case 2:
          if (digit == 0) {
            writePinToByte(pixelAnodeBitMap[2], true);
          }
          if (digit == 2) {
            writePinToByte(pixelAnodeBitMap[8], true);
          }
          if (digit == 4) {
            writePinToByte(pixelAnodeBitMap[14], true);
          }
          switch (row) {
            case 0:
              writePinToByte(pixelCathodeBitMap[1], true);
              break;
            case 1:
              writePinToByte(pixelCathodeBitMap[0], true);
              break;
            case 2:
              writePinToByte(pixelCathodeBitMap[2], true);
              break;
            case 3:
              writePinToByte(pixelCathodeBitMap[3], true);
              break;
            case 4:
              writePinToByte(pixelCathodeBitMap[4], true);
              break;
            case 5:
              writePinToByte(pixelCathodeBitMap[5], true);
              break;
            case 6:
              writePinToByte(pixelCathodeBitMap[6], true);
              break;
          }
          break;
      }
      break;
    case 1:
    case 3:
    case 5:
      switch (col) {
        case 0:
          if (digit == 1) {
            writePinToByte(pixelAnodeBitMap[3], true);
          }
          if (digit == 3) {
            writePinToByte(pixelAnodeBitMap[9], true);
          }
          if (digit == 5) {
            writePinToByte(pixelAnodeBitMap[15], true);
          }
          switch (row) {
            case 0:
              writePinToByte(pixelCathodeBitMap[1], true);
              break;
            case 1:
              writePinToByte(pixelCathodeBitMap[0], true);
              break;
            case 2:
              writePinToByte(pixelCathodeBitMap[2], true);
              break;
            case 3:
              writePinToByte(pixelCathodeBitMap[3], true);
              break;
            case 4:
              writePinToByte(pixelCathodeBitMap[4], true);
              break;
            case 5:
              writePinToByte(pixelCathodeBitMap[5], true);
              break;
            case 6:
              writePinToByte(pixelCathodeBitMap[6], true);
              break;
          }
          break;
        case 1:
          if (digit == 1) {
            writePinToByte(pixelAnodeBitMap[4], true);
          }
          if (digit == 3) {
            writePinToByte(pixelAnodeBitMap[10], true);
          }
          if (digit == 5) {
            writePinToByte(pixelAnodeBitMap[16], true);
          }
          if (row != 1 && row != 2 && row != 4 && row != 5) {
            switch (row) {
              case 0:
                writePinToByte(pixelCathodeBitMap[0], true);
                break;
              case 3:
                writePinToByte(pixelCathodeBitMap[2], true);
                break;
              case 6:
                writePinToByte(pixelCathodeBitMap[4], true);
                break;
            }
          }
          break;
        case 2:
          if (digit == 1) {
            writePinToByte(pixelAnodeBitMap[5], true);
          }
          if (digit == 3) {
            writePinToByte(pixelAnodeBitMap[11], true);
          }
          if (digit == 5) {
            writePinToByte(pixelAnodeBitMap[17], true);
          }
          switch (row) {
            case 0:
              writePinToByte(pixelCathodeBitMap[0], true);
              break;
            case 1:
              writePinToByte(pixelCathodeBitMap[1], true);
              break;
            case 2:
              writePinToByte(pixelCathodeBitMap[2], true);
              break;
            case 3:
              writePinToByte(pixelCathodeBitMap[3], true);
              break;
            case 4:
              writePinToByte(pixelCathodeBitMap[4], true);
              break;
            case 5:
              writePinToByte(pixelCathodeBitMap[5], true);
              break;
            case 6:
              writePinToByte(pixelCathodeBitMap[6], true);
              break;
          }
          break;
      }
      break;
    case 10: // PM mark on independantly
      writePinToByte(2, true);
      switch (row) {
        case 0:
          writePinToByte(pixelCathodeBitMap[0], true);
          break;
        case 1:
          writePinToByte(pixelCathodeBitMap[1], false);
          break;
        case 2:
          writePinToByte(pixelCathodeBitMap[2], false);
          break;
        case 3:
          writePinToByte(pixelCathodeBitMap[3], false);
          break;
        case 4:
          writePinToByte(pixelCathodeBitMap[4], false);
          break;
        case 5:
          writePinToByte(pixelCathodeBitMap[5], false);
          break;
        case 6:
          writePinToByte(pixelCathodeBitMap[6], false);
          break;
      }
      break;
    case 11: // PM mark with left colon
      writePinToByte(2, true);
      switch (row) {
        case 0:
          writePinToByte(pixelCathodeBitMap[0], true);
          break;
        case 1:
          writePinToByte(pixelCathodeBitMap[1], false);
          break;
        case 2:
          writePinToByte(pixelCathodeBitMap[2], true);
          break;
        case 3:
          writePinToByte(pixelCathodeBitMap[3], false);
          break;
        case 4:
          writePinToByte(pixelCathodeBitMap[4], true);
          break;
        case 5:
          writePinToByte(pixelCathodeBitMap[5], false);
          break;
        case 6:
          writePinToByte(pixelCathodeBitMap[6], false);
          break;
      }
      break;
    case 12: // left colon on independantly
      writePinToByte(2, true);
      switch (row) {
        case 0:
          writePinToByte(pixelCathodeBitMap[0], false);
          break;
        case 1:
          writePinToByte(pixelCathodeBitMap[1], false);
          break;
        case 2:
          writePinToByte(pixelCathodeBitMap[2], true);
          break;
        case 3:
          writePinToByte(pixelCathodeBitMap[3], false);
          break;
        case 4:
          writePinToByte(pixelCathodeBitMap[4], true);
          break;
        case 5:
          writePinToByte(pixelCathodeBitMap[5], false);
          break;
        case 6:
          writePinToByte(pixelCathodeBitMap[6], false);
          break;
      }
      break;
    case 13: // right colon on
      writePinToByte(2, true);
      switch (row) {
        case 0:
          writePinToByte(pixelCathodeBitMap[0], false);
          break;
        case 1:
          writePinToByte(pixelCathodeBitMap[2], false);
          break;
        case 2:
          writePinToByte(pixelCathodeBitMap[1], true);
          break;
        case 3:
          writePinToByte(pixelCathodeBitMap[4], false);
          break;
        case 4:
          writePinToByte(pixelCathodeBitMap[3], true);
          break;
        case 5:
          writePinToByte(pixelCathodeBitMap[5], false);
          break;
        case 6:
          writePinToByte(pixelCathodeBitMap[6], false);
          break;
      }
      break;
  }
}

void SPrintZeroPadBin(uint16_t number) {
  char binstr[] = "00000000";
  uint8_t i = 0;
  uint16_t n = number;

  while (n > 0 && i < ZERO_PADDING) {
    binstr[ZERO_PADDING - 1 - i] = n % 2 + '0';
    ++i;
    n /= 2;
  }
  Serial.print(binstr);
}

void getNTPTime() {
  if (!gotNTPTime) {
    Serial.print("Getting time from NTP ... ");
    if (timeClient.forceUpdate()) {
      Serial.println("OK");
      Serial.print("Setting time ... ");
      setTime(timeClient.getEpochTime());
      Serial.print("OK");
      Serial.print(", Time: ");
      Serial.println(timeClient.getFormattedTime());
      gotNTPTime = true;
    }
    else {
      Serial.println("ERROR");
    }
  }
}

void updateTimeNumberForDisplay() {
  isBlink = !isBlink;
  //(hour() >= 0 && hour() < 12) ? blankDisplay = true : blankDisplay = false;
  (isPM()) ? isNight = true : isNight = false;
  if (gotNTPTime) {
    finalNumberForDisplay = hourFormat12() * 10000 + minute() * 100 + second();
  }
}

void setup() {
  pinMode(P_SERIAL, OUTPUT);
  pinMode(P_LATCH, OUTPUT);
  pinMode(P_CLOCK, OUTPUT);
  // Create an array of bytes to match the number of registeres (1 pattern for each)
  registerPatterns = new byte[REGISTERS];
  writeToRegisters();
  drawLED(2, 0, 5);
  writeToRegisters();

  delay (500);
  Serial.begin(115200);
  while (!Serial) {}   // Wait for serial to connect
  Serial.println("LED Matrix Clock");

  Serial.print("Connecting to WiFi ... ");
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP("HK SG Note 9", "07081989");
  wifiMulti.addAP("WAW20_HK2", "!219adeltawab");
  WiFi.hostname("HKMatrixClock");
  while (wifiMulti.run() != WL_CONNECTED) {}
  Serial.print("OK, IP: ");
  Serial.println(WiFi.localIP());

  drawLED(2, 2, 5);
  writeToRegisters();
  Serial.print("Starting web-server ... ");
  server.on("/", handleRoot);
  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("OK");

  delay(2000);
  drawLED(3, 0, 5);
  writeToRegisters();
  timeClient.begin();
  getNTPTime();

  t.every(1000, getNTPTime);
  t.every(1000, updateTimeNumberForDisplay);
  drawLED(3, 2, 5);
  writeToRegisters();
  delay(2000);
}

void loop() {
  t.update();
  if (gotNTPTime) {
    drawNumberOnDisplay(finalNumberForDisplay);
  }
  if (millis() / 1000 > timeToSwitchOffWiFi) {
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  }
  else {
    wifiMulti.run();
    server.handleClient();
  }
}
