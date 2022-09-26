#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

const char* ssid     = "WAW20_HK";
const char* password = "basboosa";

const char* remote_host = "www.google.com";

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
long tun = 0;


void setup() {
  Serial.begin(115200);
  delay(10);
  // initialize the LCD
  lcd.begin(0, 2); // sda=0, scl=2

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");
  lcd.setCursor(0, 1);
  lcd.print("SSID: ");
  lcd.print(ssid);
  delay(500);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    tun++;
    lcd.setCursor(0, 3);
    lcd.print(tun);
    delay(1000);
    Serial.print(".");
  }
}

void loop()
{
  int p = Ping.averageTime();
  lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print(p);
  lcd.print("ms");
  delay(2000);
  if (Ping.ping(remote_host)) {
    Serial.println("Success!!");
  } else {
    Serial.println("Error :(");
  }
}
