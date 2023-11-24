#include "battery.h"
#include "config.h"
#include <Arduino.h>
#include <TimerEvent.h>

TimerEvent batteryReadTimer;

uint8_t battStatus, battLevel;
bool isBattLow = false;
long result;

void battInit() {
  pinMode(TP4056CHRG_PIN, INPUT_PULLUP);
  pinMode(TP4056STDBY_PIN, INPUT_PULLUP);
  batteryReadTimer.set(250, loopBatt);
}

void battRun(void) {
  //loopBatt();
  batteryReadTimer.update();
}

void loopBatt() {
  if (!digitalRead(TP4056CHRG_PIN)) {
    battStatus = 1;
    battLevel = battLevel + 10;
    if (battLevel > 100) battLevel = 0;
  }
  else if (!digitalRead(TP4056STDBY_PIN)) {
    battStatus = 2;
    battLevel = 100;
  }
  else {
    battStatus = 0;
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC));
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    battLevel = map(result, 3500, 4200, 0, 100);
    //if (battLevel <= 0) battLevel = 0;
    if (battLevel >= 100) battLevel = 100;
    if (battLevel <= 5) {
      isBattLow = true;
    }
    else {
      isBattLow = false;
    }
  }
}
