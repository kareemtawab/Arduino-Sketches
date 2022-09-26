#include <SevSeg.h>
#include "RTClib.h"
#include <Wire.h>

SevSeg sevseg;
RTC_DS3231 rtc;

long previousMillis = 0;
long interval = 500;
int hour12 = 0;
int temp = 0;
unsigned long date_temp_to_display = 0;
int wd[7] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000};
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
 DateTime now;
 
void setup() {
  rtc.begin();

  Serial.begin(115200);

  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  byte numDigits = 9;
  byte digitPins[] = {A3, 2, 3, 4, 5, 6, 7, 8, 9}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
  byte segmentPins[] = {A1, 11, 12, 13, A0, 10, A2}; //Segments: A,B,C,D,E,F,G,Period
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);  //sevseg2.begin(COMMON_ANODE, numDigits2, digitPins2, segmentPins2);
  sevseg.setBrightness(25); //Note: 100 brightness simply corresponds to a delay of 2000us after lighting each segment

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    
    now = rtc.now();

    //char s[64];
    //sprintf(s , "Time: %02d:%02d:%02d, Date: %s %02d/%02d/%04d", now.hour(), now.minute(), now.second(), daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year());
    //Serial.println(s);

    temp = rtc.getTemperature();
    date_temp_to_display = ((long)now.year() - 2000) * 1000000L + (long)now.month() * 10000 + (long)now.day() * 100 + (long)temp * 1;
    Serial.print("Date and temp: ");
    Serial.println(date_temp_to_display);

    if (now.hour() > 12) {
      hour12 = now.hour() - 12;
    }
    else if (now.hour() == 0) {
      hour12 = 12;
    }
    else {
      hour12 = now.hour();
    }
    Serial.print("Sending to slave: ");
    char ts[12];
    sprintf(ts , "h%02dm%02ds%02d", hour12, now.minute(), now.second());
    Serial.println(ts);
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(ts);
    Wire.endTransmission();    // stop transmitting
  }

  sevseg.setNumber(date_temp_to_display);
  sevseg.setSegmentsDigit(0, wd[now.dayOfTheWeek()]);
  sevseg.refreshDisplay(); // Must run repeatedly; don't use blocking code (ex: delay()) in the loop() function or this won't work right
}
