#include <Wire.h>
#include <OneWire.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timer.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>

// Temp data wire is plugged into pin 11 on the Arduino
#define ONE_WIRE_BUS 11

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

Timer t;

int d = 165;
float temp = 0.0f; // current temp
byte bar1[8] =
{
  B11111,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11111
};
byte bar2[8] =
{
  B11111,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B11111
};
byte bar3[8] =
{
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011
};
byte bar4[8] =
{
  B11111,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011
};
byte bar5[8] =
{
  B11111,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B00011,
  B11111
};
byte bar6[8] =
{
  B11111,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111
};
byte bar7[8] =
{
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111
};
byte bar8[8] =
{
  B11111,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011
};


void custom0(int col)
{ // uses segments to build the number 0
  lcd.setCursor(col, 0);
  lcd.write(8);
  lcd.setCursor(col, 1);
  lcd.write(7);
}

void custom1(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(3);
  lcd.setCursor(col, 1);
  lcd.write(3);
}

void custom2(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(1);
}

void custom3(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.setCursor(col, 1);
  lcd.write(2);
}

void custom4(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(7);
  lcd.setCursor(col, 1);
  lcd.write(4);
}

void custom5(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
}

void custom6(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(6);
}

void custom7(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(3);
}

void custom8(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(6);
  lcd.setCursor(col, 1);
  lcd.write(6);
}

void custom9(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(6);
  lcd.setCursor(col, 1);
  lcd.write(2);
}

void printNumber(int value, int col) {
  if (value == 0) {
    custom0(col);
  } if (value == 1) {
    custom1(col);
  } if (value == 2) {
    custom2(col);
  } if (value == 3) {
    custom3(col);
  } if (value == 4) {
    custom4(col);
  } if (value == 5) {
    custom5(col);
  } if (value == 6) {
    custom6(col);
  } if (value == 7) {
    custom7(col);
  } if (value == 8) {
    custom8(col);
  } if (value == 9) {
    custom9(col);
  }
}

int daylength;

void setup () {

  lcd.init();
  lcd.backlight();

  // assignes each segment a write number
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.createChar(6, bar6);
  lcd.createChar(7, bar7);
  lcd.createChar(8, bar8);

  Serial.begin(9600);
  sensors.begin();  // Start up the library
  sensors.setResolution(11); // set resolution of temp bits (from 9 to 12)
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  delay (400);
  dallastempget();

  setSyncProvider(RTC.get);
  Serial.print("Time is ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
  Serial.print("Ambient Temperature is ");
  Serial.println(temp - 4);

  t.every(20000, displaydateandtemp);
  //t.every(400, dallastempget);

}

void loop() {
  
  t.update();
  displaytime();

}

void print2digits(int number) {

  // Output leading zero
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);

}

void displaytime() {

  // Display time
  int hr = hourFormat12();
  int a, b;
  if (hr > 9) {
    a = (hr - (hr % 10)) / 10;
    hr = hr % 10;
  } else {
    a = 0;
  }

  b = hr;

  int minutes = minute();
  int c, d;
  if (minutes > 9) {
    c = (minutes - (minutes % 10)) / 10;
    minutes = minutes % 10;
  } else {
    c = 0;
  }

  d = minutes;

  int seconds = second();
  int e, f;
  if (seconds > 9) {
    e = (seconds - (seconds % 10)) / 10;
    seconds = seconds % 10;
  } else {
    e = 0;
  }

  f = seconds;

  lcd.setCursor(0, 0);
  printNumber(a, 0);
  printNumber(b, 1);
  printNumber(c, 3);
  printNumber(d, 4);
  printNumber(e, 6);
  printNumber(f, 7);

  lcd.setCursor(9, 1);
  if (isAM() == true)
  {
    lcd.print("am");
  }
  else
  {
    lcd.print("pm");
  }

  lcd.setCursor(2, 0);
  lcd.print(".");
  lcd.setCursor(2, 1);
  lcd.print(".");
  lcd.setCursor(5, 0);
  lcd.print(".");
  lcd.setCursor(5, 1);
  lcd.print(".");
  lcd.setCursor(12, 0);
  lcd.print("TIME");
  /*delay(400);
    lcd.setCursor(2, 0);
    lcd.print(" ");
    lcd.setCursor(2, 1);
    lcd.print(" ");
    lcd.setCursor(5, 0);
    lcd.print(" ");
    lcd.setCursor(5, 1);
    lcd.print(" ");
    delay(400);*/

}

void displaydateandtemp() {

  lcd.clear();

  // Display Day-of-Week
  //daylength = strlen(dayStr(weekday()));
  //lcd.setCursor((16 - daylength) / 2, 0);
  lcd.setCursor(12, 1);
  lcd.print(dayShortStr(weekday()));

  // Display date
  int days = day();
  int g, h;
  if (days > 9) {
    g = (days - (days % 10)) / 10;
    days = days % 10;
  } else {
    g = 0;
  }

  h = days;

  int months = month();
  int i, j;
  if (months > 9) {
    i = (months - (months % 10)) / 10;
    months = months % 10;
  } else {
    i = 0;
  }

  j = months;

  int years = year() - 2000;
  int k, l;
  if (years > 9) {
    k = (years - (years % 10)) / 10;
    years = years % 10;
  } else {
    k = 0;
  }

  l = years;

  printNumber(g, 0);
  printNumber(h, 1);
  printNumber(i, 3);
  printNumber(j, 4);
  printNumber(2, 6);
  printNumber(0, 7);
  printNumber(k, 8);
  printNumber(l, 9);

  lcd.setCursor(2, 1);
  lcd.print(".");
  lcd.setCursor(5, 1);
  lcd.print(".");
  lcd.setCursor(12, 0);
  lcd.print("DATE");
  /*
    lcd.setCursor(3, 1);
    print2digits(day());
    lcd.print(".");
    print2digits(month());
    lcd.print(".");
    lcd.print(year());
  */
  delay(3000);
  lcd.clear();

  // Display Temperature
  lcd.clear();
  
  dallastempget();
  temp = temp - 4;

  int celsius = (int)temp;
  int m, n;
  float o;
  if (celsius > 9) {
    m = (celsius - (celsius % 10)) / 10;
    celsius = celsius % 10;
  } else {
    m = 0;
  }

  n = celsius;
  o = (temp - (int)temp) * 10;

  lcd.setCursor(0, 0);
  printNumber(m, 0);
  printNumber(n, 1);
  printNumber(o, 3);

  lcd.setCursor(2, 1);
  lcd.print(".");
  lcd.setCursor(5, 0);
  lcd.print((char)223);
  lcd.print((char)1);
  lcd.setCursor(12, 0);
  lcd.print("TEMP");

  delay(3000);
  lcd.clear();

}

void dallastempget() {

  digitalWrite(13, HIGH);
  sensors.setWaitForConversion(false);
  temp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();
  digitalWrite(13, LOW);

}
