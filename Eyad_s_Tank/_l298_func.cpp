#include <Arduino.h>
#include <SoftPWM.h>
#include "_l298_func.h"

#define L298_EN A1  // A1
#define M1A A2      // A2
#define M1B A3      // A3
#define M2A A4      // A4
#define M2B A5      // A5

void l298_init(void) {
  SoftPWMBegin();
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 0);
  SoftPWMSetFadeTime(L298_EN, 350, 350);
}

void l298_moveF(int s) {
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, s);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, s);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveB(int s) {
  SoftPWMSet(M1A, s);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, s);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveR(int s) {
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, s);
  SoftPWMSet(M2A, s);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveL(int s) {
  SoftPWMSet(M1A, s);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, s);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveFR(int s) {
  SoftPWMSet(M1A, s / 2);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, s);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveFL(int s) {
  SoftPWMSet(M1A, s);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, s / 2);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveBR(int s) {
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, s / 2);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, s);
  SoftPWMSet(L298_EN, 255);
}

void l298_moveBL(int s) {
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, s);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, s / 2);
  SoftPWMSet(L298_EN, 255);
}

void l298_stop(void) {
  SoftPWMSet(M1A, 0);
  SoftPWMSet(M1B, 0);
  SoftPWMSet(M2A, 0);
  SoftPWMSet(M2B, 0);
  SoftPWMSet(L298_EN, 0);
}
