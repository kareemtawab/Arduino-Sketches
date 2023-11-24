#include <MD_Parola.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "JF_Font_Data.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN    15 // GPIO15 for D8

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);
WiFiUDP ntpUDP;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);
time_t ntptime;
uint8_t  nightsymbol[] = {10, 0x2, 0x27, 0x42, 0xc0, 0xc1, 0x67, 0x7e, 0x3c, 0x1, 0x83};
uint8_t  daysymbol[] = {10, 0x12, 0x49, 0x24, 0x92, 0x48, 0x23, 0x87, 0x4f, 0x1f, 0x9f};
unsigned long previousMillis = 0;
const long interval = 60000;

IPAddress local_IP(192, 168, 1, 221);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 167);   //optional
IPAddress secondaryDNS(8, 8, 8, 8); //optional

void handleRoot() {
  server.send(200, "text/plain", "LED Matrix Clock\nGo to 'http://192.168.1.221/update' to perform OTA update");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("LED Matrix Clock");

  P.begin();
  P.setIntensity(1);
  P.addChar('<', nightsymbol);
  P.addChar('>', daysymbol);
  P.setFont(jF_Custom);
  P.setInvert(false);
  P.print(".con");

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP("WAW20_AE1", "!219adeltawab");
  wifiMulti.addAP("WAW20_HK3", "!219adeltawab");
  wifiMulti.run();
  WiFi.hostname("AELEDClock");

  P.print(".conok");
  delay(2500);
  P.print(".upd");
  //delay(2500);

  timeClient.begin();
  //timeClient.forceUpdate();
  Serial.println(timeClient.getFormattedTime());
  setTime(timeClient.getEpochTime());

  server.on("/", handleRoot);

  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
}

void loop() {
  wifiMulti.run();
  server.handleClient();
  timeClient.update();
  setTime(timeClient.getEpochTime());

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

  }

  String strhour, strminute;
  char dayornight, separator;
  if (hourFormat12() < 10) {
    strhour = "0" + String(hourFormat12());
  }
  else {
    strhour = String(hourFormat12());
  }
  if (minute() < 10) {
    strminute = "0" + String(minute());
  }
  else {
    strminute = String(minute());
  }
  if (isAM()) {
    dayornight = '>';
  }
  if (isPM()) {
    dayornight = '<';
  }
  if (second() % 2) {
    separator = ':';
  }
  else {
    separator = ' ';
  }
  P.print(strhour + separator + strminute + dayornight);
}
