#include <SPI.h>
#include <RH_NRF24.h>
#include <Timer.h>

RH_NRF24 nrf24(9, 10);
Timer timer; // timer function for the Timer.h library

int a, b, c, d, e;
char msg[28];

void setup(void) {
  Serial.begin(115200);

  nrf24.init();
  nrf24.setChannel(10);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

  timer.every(350, getall);
  timer.every(1000, rfsend);
}

void loop(void) {
  timer.update();
}

void getall() {
  a = map(analogRead(A6), 0, 1023, 0, 99);
  b = map(analogRead(A0), 0, 1023, 0, 99);
  c = map(analogRead(A1), 0, 1023, 0, 99);
  d = map(analogRead(A2), 0, 1023, 0, 99);
  e = map(analogRead(A3), 0, 1023, 0, 99);
}

void rfsend() {
  sprintf(msg, ">71,%d", a);
  if (nrf24.send(msg, sizeof(msg)))Serial.println(msg);
  sprintf(msg, ">72,%d", b);
  if (nrf24.send(msg, sizeof(msg)))Serial.println(msg);
  sprintf(msg, ">73,%d", c);
  if (nrf24.send(msg, sizeof(msg)))Serial.println(msg);
  sprintf(msg, ">74,%d", d);
  if (nrf24.send(msg, sizeof(msg)))Serial.println(msg);
  sprintf(msg, ">75,%d", e);
  if (nrf24.send(msg, sizeof(msg)))Serial.println(msg);
}
