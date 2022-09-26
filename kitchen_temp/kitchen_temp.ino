#include <Arduino.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h> // the beautiful library by Simon Monk, Damian Philipp and Jack Christensen. Big thanks to them.
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 6, /* data=*/ 5, /* cs=*/ 3, /* dc=*/ 4, /* reset=*/ 2);  // Nokia 5110 Display

Timer timer; // timer function for the Timer.h library

RF24 radio(7, 10);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 01;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

float t;
int BL = 9;

void setup(void) {
  u8g2.begin();
  
  sensors.begin();
  sensors.setResolution(12);
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  
  timer.every(1000, dallastempget);
  timer.every(1500, rfsend);
  
  analogWrite(BL, 240);
}

void loop(void) {
  network.update();
  
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
  RF24NetworkHeader header(master00);   // (Address where the data is going)
  network.write(header, &t, sizeof(t)); // Send the data
}
