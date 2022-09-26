#include <TimeLib.h>
#include "Timer.h"                     //http://github.com/JChristensen/Timer
Timer t;                               //instantiate the timer object

const int digitApin = 13;
const int digitBpin = 12;
const int digitCpin = 0;
const int digitDpin = 1;
const int colonpin = 4;

const int segment[] = {11, 10, 8, 9, 7, 6, 3, 2};
bool num0[]   = {1, 1, 1, 0, 1, 0, 1, 1};
bool num1[]   = {1, 0, 0, 0, 0, 0, 0, 1};
bool num2[]   = {1, 1, 0, 1, 1, 0, 1, 0};
bool num3[]   = {1, 1, 0, 1, 1, 0, 0, 1};
bool num4[]   = {1, 0, 1, 1, 0, 0, 0, 1};
bool num5[]   = {0, 1, 1, 1, 1, 0, 0, 1};
bool num6[]   = {0, 1, 1, 1, 1, 0, 1, 1};
bool num7[]   = {1, 1, 0, 0, 0, 0, 0, 1};
bool num8[]   = {1, 1, 1, 1, 1, 0, 1, 1};
bool num9[]   = {1, 1, 1, 1, 1, 0, 0, 1};
bool dash[]   = {0, 0, 0, 1, 0, 0, 0, 0};
bool lettE[]  = {0, 1, 1, 1, 1, 0, 1, 0};
bool lettr[]  = {0, 0, 0, 1, 0, 0, 1, 0};

const int multiplex = 3000; //multiplex speed through digits in us

void setup()
{
  pinMode(digitApin, OUTPUT);
  pinMode(digitBpin, OUTPUT);
  pinMode(digitCpin, OUTPUT);
  pinMode(digitDpin, OUTPUT);
  pinMode(colonpin, OUTPUT);
  digitalWrite(colonpin, HIGH);

  for (int i = 0; i <= 7; i++) {
    pinMode(segment[i], OUTPUT);
  }

  for (int i = 0; i <= 99; i++) {
    setDigitD(getDigit(i, 1));
    setDigitC(getDigit(i, 1));
    setDigitB(getDigit(i, 1));
    setDigitA(getDigit(i, 1));
  }
  alloff();
  digitalWrite(colonpin, LOW);
  delay(2000);
  t.oscillate(colonpin, 500, HIGH);
}

void loop()
{
  t.update();

  if (hour() > 99) {
    displayoverflow();
  }
  else {
    setDigitD(getDigit(minute(), 0));
    setDigitC(getDigit(minute(), 1));
    setDigitB(getDigit(hour(), 0));
    setDigitA(getDigit(hour(), 1));
  }
}

// get a digit from a number at position pos
int getDigit (int number, int pos) {
  return (pos == 0) ? number % 10 : getDigit (number / 10, --pos);
}

void alloff() {
  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, HIGH);
}

void displayoverflow() {
  for (int i = 0; i <= 7; i++) {
    digitalWrite(segment[i], lettE[i]);
  }
  digitalWrite(digitApin, LOW);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
  for (int i = 0; i <= 7; i++) {
    digitalWrite(segment[i], lettr[i]);
  }
  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, LOW);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
  for (int i = 0; i <= 7; i++) {
    digitalWrite(segment[i], lettr[i]);
  }
  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, LOW);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
  for (int i = 0; i <= 7; i++) {
    digitalWrite(segment[i], dash[i]);
  }
  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, LOW);
  delayMicroseconds(multiplex);
}

int setDigitA(int n) {
  if (n == 0) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num0[i]);
    }
  }
  if (n == 1) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num1[i]);
    }
  }
  if (n == 2) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num2[i]);
    }
  }
  if (n == 3) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num3[i]);
    }
  }
  if (n == 4) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num4[i]);
    }
  }
  if (n == 5) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num5[i]);
    }
  }
  if (n == 6) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num6[i]);
    }
  }
  if (n == 7) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num7[i]);
    }
  }
  if (n == 8) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num8[i]);
    }
  }
  if (n == 9) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num9[i]);
    }
  }

  digitalWrite(digitApin, LOW);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
}

int setDigitB(int n) {
  if (n == 0) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num0[i]);
    }
  }
  if (n == 1) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num1[i]);
    }
  }
  if (n == 2) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num2[i]);
    }
  }
  if (n == 3) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num3[i]);
    }
  }
  if (n == 4) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num4[i]);
    }
  }
  if (n == 5) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num5[i]);
    }
  }
  if (n == 6) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num6[i]);
    }
  }
  if (n == 7) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num7[i]);
    }
  }
  if (n == 8) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num8[i]);
    }
  }
  if (n == 9) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num9[i]);
    }
  }

  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, LOW);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
}

int setDigitC(int n) {
  if (n == 0) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num0[i]);
    }
  }
  if (n == 1) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num1[i]);
    }
  }
  if (n == 2) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num2[i]);
    }
  }
  if (n == 3) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num3[i]);
    }
  }
  if (n == 4) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num4[i]);
    }
  }
  if (n == 5) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num5[i]);
    }
  }
  if (n == 6) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num6[i]);
    }
  }
  if (n == 7) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num7[i]);
    }
  }
  if (n == 8) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num8[i]);
    }
  }
  if (n == 9) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num9[i]);
    }
  }

  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, LOW);
  digitalWrite(digitDpin, HIGH);
  delayMicroseconds(multiplex);
}

int setDigitD(int n) {
  if (n == 0) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num0[i]);
    }
  }
  if (n == 1) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num1[i]);
    }
  }
  if (n == 2) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num2[i]);
    }
  }
  if (n == 3) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num3[i]);
    }
  }
  if (n == 4) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num4[i]);
    }
  }
  if (n == 5) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num5[i]);
    }
  }
  if (n == 6) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num6[i]);
    }
  }
  if (n == 7) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num7[i]);
    }
  }
  if (n == 8) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num8[i]);
    }
  }
  if (n == 9) {
    for (int i = 0; i <= 7; i++) {
      digitalWrite(segment[i], num9[i]);
    }
  }

  digitalWrite(digitApin, HIGH);
  digitalWrite(digitBpin, HIGH);
  digitalWrite(digitCpin, HIGH);
  digitalWrite(digitDpin, LOW);
  delayMicroseconds(multiplex);
}
