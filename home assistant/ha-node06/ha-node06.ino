//#include <Arduino.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include "SparkFunBME280.h"
#include <BH1750_WE.h>
#include <Timer.h>

#define BH1750_ADDRESS 0x23
BME280 bme280;
BH1750_WE myBH1750(BH1750_ADDRESS); 
RH_NRF24 nrf24(9, 10);
Timer timer; // timer function for the Timer.h library

float t;
//float a;
float p;
float h;
long l;
char msg[28];

void setup(void) {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);

  nrf24.init();
  nrf24.setChannel(10);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

  Wire.begin();
  bme280.beginI2C();
  myBH1750.init(); // sets default values: mode = CHM, measuring time factor = 1.0
  myBH1750.setMeasuringTimeFactor(0.45);

  timer.every(350, getall);
  timer.every(1000, rfsend);
}

void loop(void) {
  timer.update();
}

void getall() {
  digitalWrite(A0, LOW);
  t = bme280.readTempC();
  //a = bme280.readFloatAltitudeMeters();
  p = bme280.readFloatPressure() / 100; //to convert Pa to hPa
  h = bme280.readFloatHumidity();
  l = myBH1750.getLux();
}

void rfsend() {
  char temp[10];
  //char alti[10];
  char pres[10];
  char humi[10];
  dtostrf(t , 2, 1, temp);
  //dtostrf(a , 3, 1, alti);
  dtostrf(p , 3, 1, pres);
  dtostrf(h , 2, 1, humi);
  //sprintf(msg, ">6,%s,%s,%s,%s,%ld", temp, alti, pres, humi, l);
  sprintf(msg, ">6,%s,%s,%s,%ld", temp, pres, humi, l);
  Serial.println(msg);
  nrf24.send(msg, sizeof(msg));
  digitalWrite(A0, HIGH);
}
