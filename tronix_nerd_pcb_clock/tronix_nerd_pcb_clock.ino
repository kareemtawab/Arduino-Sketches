#define shiftRegisterLatchPin 9
#define shiftRegisterClockPin 10
#define shiftRegisterDataPin 8
#define espSerialRXPin 7
#define espSerialTXPin 6
#define colonLEDPin 11
#define buzzerPin 12
#define topButtonPin 2
#define middleButtonPin 4
#define bottomButtonPin 5
#define ds18b20Pin 3
#define INTERVAL_TICKER_SLOWER 3000
#define INTERVAL_TICKER_SLOW 1500
#define INTERVAL_TICKER_1000 1000
#define INTERVAL_TICKER_500 500
#define INTERVAL_TICKER_FAST 250
#define INTERVAL_TICKER_FASTER 125
#define MODE_UNSET 0
#define MODE_SET_HOUR 1
#define MODE_SET_MINUTE 2
#define MODE_SET_YEAR 3
#define MODE_SET_MONTH 4
#define MODE_SET_DAY 5
#define NO_BLINK 0
#define BLINK_SLOW 1
#define BLINK_FAST 2

#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

RTC_DS1307 rtc;
DateTime now, set;
OneWire oneWire(ds18b20Pin);
DallasTemperature sensors(&oneWire);
SoftwareSerial espSerial = SoftwareSerial(espSerialRXPin, espSerialTXPin);

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int mode, disp_val, temperature, hour_old;
bool ticker_slower, ticker_slow, ticker_1000, ticker_500, ticker_fast, ticker_faster;
unsigned long previousMillis_slower, previousMillis_slow, previousMillis_1000, previousMillis_500, previousMillis_fast, previousMillis_faster, time_interrupt_triggered, time_button_triggered, time_serial_printed, time_setting_change_triggered, buzzer_trigger;
int set_hours, set_minutes, set_year, set_month, set_day;
bool up_button_pressed, dn_button_pressed, rtc_ready_to_set, setting_to_change, hour_chime_to_play;

void setup() {
  pinMode(espSerialRXPin, INPUT);
  pinMode(espSerialTXPin, OUTPUT);
  pinMode(colonLEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(topButtonPin, INPUT_PULLUP);
  pinMode(middleButtonPin, INPUT_PULLUP);
  pinMode(bottomButtonPin, INPUT_PULLUP);
  pinMode(shiftRegisterLatchPin, OUTPUT);
  pinMode(shiftRegisterDataPin, OUTPUT);
  pinMode(shiftRegisterClockPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(topButtonPin), button_interrupt, FALLING);

  Serial.begin(9600);
  espSerial.begin(9600);

  rtc_init();

  sensors.setWaitForConversion(false);  // makes it async
  sensors.requestTemperatures();
  sensors.begin();

  introDisplay();
  // delay(1000);
}

void loop() {
  resume_blinkers();
  updateButtons();
  updateSerial();
  updateAll();
  buzzerUpdate();
  updateTemperature();
}

void blankDisplay() {
  sendToRegisters(65535, false, false, NO_BLINK, NO_BLINK);
}

void introDisplay() {
  blankDisplay();
  for (int i = 80; i > 0; i = i / 1.15) {
    sendToRegisters(65528, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(65423, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(36863, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(63743, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(36863, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(65423, false, false, NO_BLINK, NO_BLINK);
    delay(i);
    sendToRegisters(65528, false, false, NO_BLINK, NO_BLINK);
    delay(i);
  }
  blankDisplay();
}

void updateAll() {
  if (millis() - time_setting_change_triggered > INTERVAL_TICKER_FASTER) {
    setting_to_change = true;
    time_setting_change_triggered = millis();
  }
  switch (mode) {
    case MODE_UNSET:
      now = rtc.now();
      if (hour_old != now.hour()) {
        hour_old = now.hour();
        buzzerBeep();
      }
      if (!ticker_slower) {
        sendToRegisters(calc_bitarray(now.twelveHour() * 100 + now.minute()), false, false, NO_BLINK, NO_BLINK);
        if (ticker_1000) analogWrite(colonLEDPin, 2);
        else analogWrite(colonLEDPin, 255);
        if (rtc_ready_to_set) {
          rtc.adjust(DateTime(set.year(), set_month, set.day(), set.hour(), set.minute(), 0));
          rtc_ready_to_set = false;
        }
      } else {
        sendToRegisters(calc_bitarray(temperature), true, false, NO_BLINK, NO_BLINK);
        analogWrite(colonLEDPin, 0);
      }
      set = rtc.now();
      set_month = set.month();
      break;
    case MODE_SET_HOUR:
      if (up_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, 1, 0, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.hour() * 100), false, true, NO_BLINK, NO_BLINK);
        analogWrite(colonLEDPin, 255);
      }
      if (dn_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, -1, 0, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.hour() * 100), false, true, NO_BLINK, NO_BLINK);
        analogWrite(colonLEDPin, 255);
      }
      if (!up_button_pressed && !dn_button_pressed) {
        sendToRegisters(calc_bitarray(set.hour() * 100), true, true, BLINK_FAST, NO_BLINK);
        if (ticker_fast) analogWrite(colonLEDPin, 0);
        else analogWrite(colonLEDPin, 255);
      }
      break;
    case MODE_SET_MINUTE:
      if (up_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, 0, 1, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.minute()), true, false, NO_BLINK, NO_BLINK);
        analogWrite(colonLEDPin, 255);
      }
      if (dn_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, 0, -1, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.minute()), true, false, NO_BLINK, NO_BLINK);
        analogWrite(colonLEDPin, 255);
      }
      if (!up_button_pressed && !dn_button_pressed) {
        sendToRegisters(calc_bitarray(set.minute()), true, true, NO_BLINK, BLINK_FAST);
        if (ticker_fast) analogWrite(colonLEDPin, 0);
        else analogWrite(colonLEDPin, 255);
      }
      break;
    case MODE_SET_YEAR:
      analogWrite(colonLEDPin, 0);
      if (up_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(365, 5, 49, 1);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.year()), false, false, NO_BLINK, NO_BLINK);
      }
      if (dn_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(-365, -5, -49, -1);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.year()), false, false, NO_BLINK, NO_BLINK);
      }
      if (!up_button_pressed && !dn_button_pressed) {
        sendToRegisters(calc_bitarray(set.year()), true, true, BLINK_FAST, BLINK_FAST);
      }
      break;
    case MODE_SET_MONTH:
      analogWrite(colonLEDPin, 0);
      if (up_button_pressed) {
        if (setting_to_change) {
          set_month++;
          if (set_month > 12) set_month = 1;
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set_month), true, false, NO_BLINK, NO_BLINK);
      }
      if (dn_button_pressed) {
        if (setting_to_change) {
          set_month--;
          if (set_month < 1) set_month = 12;
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set_month), true, false, NO_BLINK, NO_BLINK);
      }
      if (!up_button_pressed && !dn_button_pressed) {
        sendToRegisters(calc_bitarray(set_month), true, true, NO_BLINK, BLINK_FAST);
      }
      set = DateTime(set.year(), set_month, set.day(), set.hour(), set.minute(), set.second());
      break;
    case MODE_SET_DAY:
      analogWrite(colonLEDPin, 0);
      if (up_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, 24, 0, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.day() * 100), false, true, NO_BLINK, NO_BLINK);
      }
      if (dn_button_pressed) {
        if (setting_to_change) {
          set = set + TimeSpan(0, -24, 0, 0);
          setting_to_change = false;
        }
        sendToRegisters(calc_bitarray(set.day() * 100), false, true, NO_BLINK, NO_BLINK);
      }
      if (!up_button_pressed && !dn_button_pressed) {
        sendToRegisters(calc_bitarray(set.day() * 100), true, true, BLINK_FAST, NO_BLINK);
      }
      rtc_ready_to_set = true;
      break;
  }
}

void buzzerBeep() {
  digitalWrite(buzzerPin, HIGH);
  buzzer_trigger = millis();
}

void buzzerUpdate() {
  if (millis() - buzzer_trigger > 25) {
    digitalWrite(buzzerPin, LOW);
  }
}

void updateTemperature() {
  sensors.requestTemperatures();  // Send the command to get temperatures
  temperature = sensors.getTempCByIndex(0);
}

void updateButtons() {
  if (!digitalRead(middleButtonPin)) {
    if (millis() - time_button_triggered > INTERVAL_TICKER_FAST) {
      up_button_pressed = true;
      time_button_triggered = millis();
    }
  } else up_button_pressed = false;
  if (!digitalRead(bottomButtonPin)) {
    if (millis() - time_button_triggered > INTERVAL_TICKER_FAST) {
      dn_button_pressed = true;
      time_button_triggered = millis();
    }
  } else dn_button_pressed = false;
}

void updateSerial() {
  if (espSerial.available() > 0) {
    if (espSerial.read() == '>') {
      if (espSerial.read() == '$') {
        int h = espSerial.parseInt();
        int mi = espSerial.parseInt();
        int s = espSerial.parseInt();
        int d = espSerial.parseInt();
        int m = espSerial.parseInt();
        int y = espSerial.parseInt();
        if (h < 24 && h > -1 && mi < 60 && mi > -1 && s < 60 && s > -1 && d < 32 && d > 0 && m < 13 && m > 0 && y < 2100 && y > 2020)
          rtc.adjust(DateTime(y, m, d, h, mi, s));
      }
    }
  } else if (Serial.available() > 0) {
    if (Serial.read() == '>') {
      if (Serial.read() == '$') {
        int h = Serial.parseInt();
        int mi = Serial.parseInt();
        int s = Serial.parseInt();
        int d = Serial.parseInt();
        int m = Serial.parseInt();
        int y = Serial.parseInt();
        if (h < 24 && h > -1 && mi < 60 && mi > -1 && s < 60 && s > -1 && d < 32 && d > 0 && m < 13 && m > 0 && y < 2100 && y > 2020)
          rtc.adjust(DateTime(y, m, d, h, mi, s));
      }
    }
  } else {
    if (millis() - time_serial_printed > INTERVAL_TICKER_1000) {
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(") ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.print('\t');
      Serial.print(temperature, DEC);
      Serial.print(" degC");
      Serial.println();
      time_serial_printed = millis();
    }
  }
}


void sendToRegisters(uint16_t m, bool blank_left, bool blank_right, int blink_left, int blink_right) {
  if (blank_left) {
    if (blink_left == NO_BLINK)
      for (int o = 0; o < 8; o++) bitSet(m, o);
    if (blink_left == BLINK_SLOW)
      if (ticker_slow) {
        for (int o = 0; o < 8; o++) bitSet(m, o);
      }
    if (blink_left == BLINK_FAST)
      if (ticker_fast) {
        for (int o = 0; o < 8; o++) bitSet(m, o);
      }
  }
  if (blank_right) {
    if (blink_right == NO_BLINK)
      for (int o = 8; o < 16; o++) bitSet(m, o);
    if (blink_right == BLINK_SLOW)
      if (ticker_slow) {
        for (int o = 8; o < 16; o++) bitSet(m, o);
      }
    if (blink_right == BLINK_FAST)
      if (ticker_fast) {
        for (int o = 8; o < 16; o++) bitSet(m, o);
      }
  }
  if (!bitRead(m, 0) && !bitRead(m, 1) && !bitRead(m, 2) && !bitRead(m, 3)) {
    bitSet(m, 0);
    bitSet(m, 1);
    bitSet(m, 2);
    bitSet(m, 3);
    digitalWrite(shiftRegisterLatchPin, LOW);
    shiftOut(shiftRegisterDataPin, shiftRegisterClockPin, MSBFIRST, highByte(m));
    shiftOut(shiftRegisterDataPin, shiftRegisterClockPin, MSBFIRST, lowByte(m));
    digitalWrite(shiftRegisterLatchPin, HIGH);
  } else {
    digitalWrite(shiftRegisterLatchPin, LOW);
    shiftOut(shiftRegisterDataPin, shiftRegisterClockPin, MSBFIRST, highByte(m));
    shiftOut(shiftRegisterDataPin, shiftRegisterClockPin, MSBFIRST, lowByte(m));
    digitalWrite(shiftRegisterLatchPin, HIGH);
  }
}

void scrollDisplay() {
  digitalWrite(shiftRegisterLatchPin, LOW);
  digitalWrite(shiftRegisterDataPin, HIGH);
  for (int i = 0; i < 4; i++) {
    digitalWrite(shiftRegisterClockPin, HIGH);
    digitalWrite(shiftRegisterClockPin, LOW);
    delay(1);
  }
  digitalWrite(shiftRegisterDataPin, LOW);
  digitalWrite(shiftRegisterLatchPin, HIGH);
}

uint16_t calc_bitarray(int val) {
  byte hours_tens = val / 1000 % 10;
  byte hours_units = val / 100 % 10;
  byte minutes_units = val / 10 % 10;
  byte minutes_tens = val % 10;
  uint16_t bitarray = (hours_tens) + (hours_units << 4) + (minutes_tens << 8) + (minutes_units << 12);
  return bitarray;
}

void resume_blinkers() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_slower >= INTERVAL_TICKER_SLOWER) {
    previousMillis_slower = currentMillis;
    ticker_slower = !ticker_slower;
  }
  if (currentMillis - previousMillis_slow >= INTERVAL_TICKER_SLOW) {
    previousMillis_slow = currentMillis;
    ticker_slow = !ticker_slow;
  }
  if (currentMillis - previousMillis_1000 >= INTERVAL_TICKER_1000) {
    previousMillis_1000 = currentMillis;
    ticker_1000 = !ticker_1000;
  }
  if (currentMillis - previousMillis_1000 >= INTERVAL_TICKER_500) {
    previousMillis_500 = currentMillis;
    ticker_500 = !ticker_500;
  }
  if (currentMillis - previousMillis_fast >= INTERVAL_TICKER_FAST) {
    previousMillis_fast = currentMillis;
    ticker_fast = !ticker_fast;
  }
  if (currentMillis - previousMillis_faster >= INTERVAL_TICKER_FASTER) {
    previousMillis_faster = currentMillis;
    ticker_faster = !ticker_faster;
  }
}

void rtc_init() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) {
      analogWrite(colonLEDPin, 0);
      delay(100);
      analogWrite(colonLEDPin, 255);
    }
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running");
  }
}

void button_interrupt() {
  if (millis() - time_interrupt_triggered > INTERVAL_TICKER_FAST) {
    buzzerBeep();
    noInterrupts();
    switch (mode) {
      case MODE_UNSET:
        set = now;
        mode = MODE_SET_MINUTE;
        break;
      case MODE_SET_HOUR:
        mode = MODE_SET_YEAR;
        break;
      case MODE_SET_MINUTE:
        mode = MODE_SET_HOUR;
        break;
      case MODE_SET_YEAR:
        mode = MODE_SET_MONTH;
        break;
      case MODE_SET_MONTH:
        mode = MODE_SET_DAY;
        break;
      case MODE_SET_DAY:
        mode = MODE_UNSET;
        break;
    }
    time_interrupt_triggered = millis();
    interrupts();
  }
}