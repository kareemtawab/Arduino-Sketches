#include <SPI.h>
#include <RH_NRF24.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RH_NRF24 nrf24(9, 10);
Timer timer; // timer function for the Timer.h library

float t;
char msg[10];

void setup()
{
  Serial.begin(9600);
  nrf24.init();
  nrf24.setChannel(10);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);
  sensors.begin();
  sensors.setResolution(12);
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);

  timer.every(1000, dallastempget);
  timer.every(1000, rfsend);
}


void loop()
{
  timer.update();
}

void dallastempget() {
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
}

void rfsend() {
  char temp[100];
  dtostrf(t , 2, 2, temp);
  sprintf(msg, ">2,%s", temp);
  Serial.println(msg);
  nrf24.send(msg, sizeof(msg));
}
