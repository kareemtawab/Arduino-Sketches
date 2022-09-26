#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid     = "WAW20";
const char* password = "basboosa";

const char* remote_host = "www.google.com";

// OPTION 1 - for text on line 1
byte x10[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07};
byte x11[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x1C};
byte x12[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F};
byte x13[8] = {0x07, 0x07, 0x07, 0x07, 0x07, 0x1F, 0x1F, 0x1F};
byte x14[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x1F, 0x1F};
byte x15[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
byte x16[8] = {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07};
byte x17[8] = {0x00, 0x00, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00};

int ping_time;
int p = 20;

// doNumber: routine to position number 'num' starting top left at row 'r', column 'c'
void doNumber(byte num, byte r, byte c) {
  lcd.setCursor(c, r);
  switch (num) {
    case 0: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(5)); lcd.write(byte(6));
      lcd.setCursor(c, r + 2); lcd.write(byte(4)); lcd.write(byte(3)); break;

    case 1: lcd.write(byte(0)); lcd.write(byte(1));
      lcd.setCursor(c, r + 1); lcd.print(" "); lcd.write(byte(5));
      lcd.setCursor(c, r + 2); lcd.write(byte(0)); lcd.write(byte(4)); break;

    case 2: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(2)); lcd.write(byte(3));
      lcd.setCursor(c, r + 2); lcd.write(byte(4)); lcd.write(byte(2)); break;

    case 3: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(0)); lcd.write(byte(3));
      lcd.setCursor(c, r + 2); lcd.write(byte(2)); lcd.write(byte(3)); break;

    case 4: lcd.write(byte(1)); lcd.write(byte(0));
      lcd.setCursor(c, r + 1); lcd.write(byte(4)); lcd.write(byte(3));
      lcd.setCursor(c, r + 2); lcd.print(" "); lcd.write(byte(6)); break;

    case 5: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(4)); lcd.write(byte(2));
      lcd.setCursor(c, r + 2); lcd.write(byte(2)); lcd.write(byte(3)); break;
    case 6: lcd.write(byte(1)); lcd.print(" ");
      lcd.setCursor(c, r + 1); lcd.write(byte(4)); lcd.write(byte(2));
      lcd.setCursor(c, r + 2); lcd.write(byte(4)); lcd.write(byte(3)); break;

    case 7: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.print(" "); lcd.write(byte(6));
      lcd.setCursor(c, r + 2); lcd.print(" "); lcd.write(byte(6)); break;

    case 8: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(4)); lcd.write(byte(3));
      lcd.setCursor(c, r + 2); lcd.write(byte(4)); lcd.write(byte(3)); break;

    case 9: lcd.write(byte(2)); lcd.write(byte(2));
      lcd.setCursor(c, r + 1); lcd.write(byte(4)); lcd.write(byte(3));
      lcd.setCursor(c, r + 2); lcd.print(" "); lcd.write(byte(6)); break;

    case 11: lcd.setCursor(c, r + 1); lcd.write(byte(7)); lcd.setCursor(c, r + 2); lcd.write(byte(7)); break;
  }
}

void drawnumber(int number, byte row) {
  int n1 = number / 100;
  int n2 = number % 100 / 10;
  int n3 = number % 10;

  if (n1 == 0) {
  }
  else {
    doNumber( n1, row, 0);
  }
  if (n2 == 0 && n1 == 0) {
  }
  else {
    doNumber( n2, row, 3);
  }
  doNumber( n3, row, 6);
}

void setup() {
  lcd.begin(0, 2); // sda=0, scl=2
  lcd.backlight();
  lcd.createChar(0, x10);                      // digit piece
  lcd.createChar(1, x11);                      // digit piece
  lcd.createChar(2, x12);                      // digit piece
  lcd.createChar(3, x13);                      // digit piece
  lcd.createChar(4, x14);                      // digit piece
  lcd.createChar(5, x15);                      // digit piece
  lcd.createChar(6, x16);                      // digit piece
  lcd.createChar(7, x17);                      // digit piece (colon)

  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");
    lcd.setCursor(0, 1);
    lcd.print("SSID: ");
    lcd.print(ssid);
    delay(1000);
    lcd.clear();
  }
  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connected!");
  }
}


void loop() {
  while (Ping.ping(remote_host)) {
    p--;
    if (p < 0) {
      p = -20;
    }
    lcd.setCursor(0, 0);
    lcd.print("Pinging Google");
    lcd.setCursor(9, 2);
    lcd.print("milli-");
    lcd.setCursor(9, 3);
    lcd.print("seconds");
    ping_time = Ping.averageTime();
    if ( ping_time > 999) {
      ping_time = 999;
    }
    drawnumber(ping_time, 1);
  }

  while (!Ping.ping(remote_host)) {
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("Ping failed :(");
    lcd.setCursor(0, 1);
    lcd.print("        ");
    lcd.setCursor(0, 2);
    lcd.write(byte(2));
    lcd.write(byte(2));
    lcd.print(" ");
    lcd.write(byte(2));
    lcd.write(byte(2));
    lcd.print(" ");
    lcd.write(byte(2));
    lcd.write(byte(2));
    lcd.setCursor(0, 3);
    lcd.print("        ");
  }
  lcd.clear();
}
