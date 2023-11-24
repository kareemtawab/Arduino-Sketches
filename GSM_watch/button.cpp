#include "button.h"
#include "config.h"
#include <Arduino.h>

int buttonID;

void buttonInit() {
  pinMode(B1_PIN, INPUT_PULLUP);
  pinMode(B2_PIN, INPUT_PULLUP);
}

void loopButton() {
  if (!digitalRead(B1_PIN)) {
    delay(100);
    if (!digitalRead(B1_PIN)) {
      buttonID = 1;
    }
  }
  if (!digitalRead(B2_PIN)) {
    delay(100);
    if (!digitalRead(B2_PIN)) {
      buttonID = 2;
    }
  }
  //  if (!digitalRead(B1_PIN)) {
  //    delay(25);
  //    if (!digitalRead(B1_PIN)) {
  //      long startPress = millis();
  //      while (!digitalRead(B1_PIN)) {}
  //      long endPress = millis();
  //      if (endPress - startPress > 2000L) {
  //        buttonID = 10;
  //      }
  //      else {
  //        buttonID = 1;
  //      }
  //    }
  //  }
  //  if (!digitalRead(B2_PIN)) {
  //    delay(25);
  //    if (!digitalRead(B2_PIN)) {
  //      long startPress = millis();
  //      while (!digitalRead(B2_PIN)) {}
  //      long endPress = millis();
  //      if (endPress - startPress > 2000L) {
  //        buttonID = 20;
  //      }
  //      else {
  //        buttonID = 2;
  //      }
  //    }
  //  }
}

bool isThereAnyButtonEvent() {
  if (buttonID) return true; else return false;
}

int getButtonID(void) {
  int b = buttonID;
  buttonID = 0;
  return b;
}
