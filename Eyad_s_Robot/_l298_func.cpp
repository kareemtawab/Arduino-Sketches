#include <Arduino.h>
#include <SoftPWM.h>
#include "_l298_func.h"

#define L298_EN A1  // A1
#define M1A A2      // A2
#define M1B A3      // A3
#define M2A A4      // A4
#define M2B A5      // A5

void l298_init(void) {
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  SoftPWMBegin();
  SoftPWMSet(A1, 0);
  SoftPWMSet(A2, 0);
  SoftPWMSet(A3, 0);
  SoftPWMSet(A4, 0);
  SoftPWMSet(A5, 0);
}

void l298_moveF(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}

void l298_moveB(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

void l298_moveR(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}

void l298_moveL(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

void l298_moveFR(void) {
  digitalWrite(L298_EN, HIGH);
  SoftPWMSet(M1A, 128);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}

void l298_moveFL(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  SoftPWMSet(M2A, 128);
  digitalWrite(M2B, LOW);
}

void l298_moveBR(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, LOW);
  SoftPWMSet(M1B, 128);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

void l298_moveBL(void) {
  digitalWrite(L298_EN, HIGH);
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW);
  SoftPWMSet(M2B, 128);
}

void l298_stop(void) {
  digitalWrite(L298_EN, LOW);
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
}
