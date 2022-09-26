#include <Arduino.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h>
#include <SPI.h>
#include <RH_NRF24.h>

RH_NRF24 nrf24(7, 10);

#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 6, /* data=*/ 5, /* cs=*/ 3, /* dc=*/ 4, /* reset=*/ 2);  // Nokia 5110 Display

Timer timer; // timer function for the Timer.h library

float t;
int BL = 9;
char msg[10];

void setup(void) {
  Serial.begin(9600);
  nrf24.init();
  nrf24.setChannel(10);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

  u8g2.begin();

  sensors.begin();
  sensors.setResolution(12);
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);

  timer.every(1000, dallastempget);
  timer.every(1000, rfsend);

  analogWrite(BL, 240);
}

void loop(void) {
  timer.update();

  u8g2.clearBuffer();
  u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_logisoso34_tn);
  u8g2.setCursor(3, 34);
  u8g2.print(t, 1);
  u8g2.setFont(u8g2_font_guildenstern_nbp_tr);
  u8g2.drawStr(0, 47, "Temperature( C)");
  u8g2.drawFrame(70, 38, 3, 3); // for the degree symbol
  u8g2.sendBuffer();
}

void dallastempget() {
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
}

void rfsend() {
  char temp[100];
  dtostrf(t , 2, 2, temp);
  sprintf(msg, ">5,%s", temp);
  Serial.println(msg);
  nrf24.send(msg, sizeof(msg));
}
