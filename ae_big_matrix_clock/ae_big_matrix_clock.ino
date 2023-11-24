#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "Timer.h"
#include <DMDESP.h>
#include <fonts/DejaVuSans9.h>
#include <fonts/DejaVuSansBold9.h>
#include <fonts/Mono5x7.h>

#define DISPLAYS_WIDE 1
#define DISPLAYS_HIGH 1
#define DISPLAY_BRIGHTNESS 150   // 1023 max
#define timeToSwitchOffWiFi 300  // seconds

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);
IPAddress local_IP(192, 168, 1, 53);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 168);  //optional
IPAddress secondaryDNS(8, 8, 8, 8);      //optional
WiFiUDP ntpUDP;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);
time_t ntptime;
Timer t;

char timeForDisplay[5], dateForDisplay[8];
bool gotNTPTime, isBlink;
String weekdays[] = { "",  "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
bool isDSTenabled = true;

void getNTPTime() {
  if (!gotNTPTime) {
    if (timeClient.forceUpdate()) {
      if (isDSTenabled) setTime(timeClient.getEpochTime() + 3600);
      if (!isDSTenabled) setTime(timeClient.getEpochTime());
      gotNTPTime = true;
    }
  }
}

void updateDataForDisplay() {
  isBlink = !isBlink;
  //(hour() >= 0 && hour() < 12) ? "" : "";
  if (gotNTPTime) {
    isPM() ? Disp.setPixel(31, 0, true) : Disp.setPixel(31, 0, false);
    if (!isBlink) {
      sprintf(timeForDisplay, "%02d:%02d", hourFormat12(), minute());
      Disp.setFont(Mono5x7);
      Disp.drawText(1, 0, timeForDisplay);
      sprintf(dateForDisplay, "%s", weekdays[weekday()]);
      Disp.setFont(DejaVuSans9);
      Disp.drawText(0, 8, "       ");
      Disp.drawText(7, 8, dateForDisplay);
    } else {
      sprintf(timeForDisplay, "%02d %02d", hourFormat12(), minute());
      Disp.setFont(Mono5x7);
      Disp.drawText(1, 0, timeForDisplay);
      sprintf(dateForDisplay, "%02d-%02d", day(), month());
      Disp.setFont(DejaVuSans9);
      Disp.drawText(0, 8, "       ");
      Disp.drawText(5, 8, dateForDisplay);
    }
  }
}

void handleRoot() {
  server.send(200, "text/html", "seconds since start: " + String(millis() / 1000));  //Send web page
}

void setup() {
  Disp.setBrightness(DISPLAY_BRIGHTNESS);  //maximum is 1024
  Disp.start();
  Disp.setPixel(0, 15, true);
  Disp.loop();

  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP("WAW20_HK2", "!219adeltawab");
  wifiMulti.addAP("WAW20_AE3", "!219adeltawab");
  wifiMulti.addAP("WAW20_AE4", "!219adeltawab");
  wifiMulti.run();
  WiFi.hostname("AEDMD2");
  while (wifiMulti.run() != WL_CONNECTED) {}

  ElegantOTA.begin(&server);
  server.on("/", handleRoot);
  server.begin();

  timeClient.begin();
  getNTPTime();

  t.every(1000, getNTPTime);
  t.every(1000, updateDataForDisplay);
}

void loop() {
  yield();
  t.update();
  Disp.loop();

  if (millis() / 1000 > timeToSwitchOffWiFi) {
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  } else {
    wifiMulti.run();
    server.handleClient();
  }
}
