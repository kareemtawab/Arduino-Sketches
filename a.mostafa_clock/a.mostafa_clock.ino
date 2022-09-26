#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
DateTime now;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
int hoursIn12hr;
int t;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("> RTC initialization failed! System can not continue!");
    lcd.setCursor(0, 0);
    lcd.print("RTC init. error");
    while (1);
  }
  else {
    Serial.println("> RTC initialized!");
  }
  if (rtc.lostPower()) {
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("> RTC lost time/date! System can not continue!");
    lcd.setCursor(0, 0);
    lcd.print("RTC batt. error");
    while (1);
  }
}

void loop()
{
  now = rtc.now();
  t = rtc.getTemperature();
  lcdupdatetime();
}

void lcdupdatetime() {
  if (now.hour() > 0 && now.hour() <= 12) {
    hoursIn12hr = now.hour();
  }
  if (now.hour() > 12 && now.hour() <= 23) {
    hoursIn12hr = now.hour() - 12;
  }
  if (now.hour() == 00) {
    hoursIn12hr = 12;
  }
  char str1[17];
  if (now.hour() >= 0 && now.hour() < 12) {
    sprintf(str1, "%02d:%02d:%02dAM  %02d", hoursIn12hr, now.minute(), now.second(), t);
  }
  else {
    sprintf(str1, "%02d:%02d:%02dPM  %02d", hoursIn12hr, now.minute(), now.second(), t);
  }
  char str2[17];
  sprintf(str2, "%s   %02d/%02d/%4d", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year());

  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcd.print(char(B11011111)); // degrees symbol
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(str2);
}
