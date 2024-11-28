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
#include <EEPROM.h>
#include <fonts/DejaVuSans9.h>
#include <fonts/DejaVuSansBold9.h>
#include <fonts/Mono5x7.h>

#define BKG_COLOR "Gold"
#define FRG_COLOR "black"
#define CLOCK_TITLE "Dad's Bedroom Matrix Clock"

#define DISPLAYS_WIDE 1
#define DISPLAYS_HIGH 1
#define DISPLAY_BRIGHTNESS 150   // 1023 max
#define timeToSwitchOffWiFi 3600  // seconds
#define EEPROM_ADDRESS 2

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);
IPAddress local_IP(192, 168, 1, 51);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 168);  //optional
IPAddress secondaryDNS(8, 8, 8, 8);      //optional
WiFiUDP ntpUDP;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
time_t ntptime;
Timer t;

char timeForDisplay[5], dateForDisplay[8];
bool gotNTPTime, isBlink;
String weekdays[] = { "", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

String MAIN_page() {
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>" + String(CLOCK_TITLE) + "</title>\n";
  ptr += "<style>html { font-family: Helvetica; font-color: " + String(FRG_COLOR) + "; display: table; margin: 0px auto; text-align: -webkit-center;}\n";
  ptr += "body{margin-top: 50px;background-color:" + String(BKG_COLOR) + ";} h1 {font-size: 40px;color: " + String(FRG_COLOR) + ";margin-bottom: 0px;} h2 {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 15px;} h3 {font-size: 15px;color: " + String(FRG_COLOR) + ";margin-bottom: 20px;}\n";
  ptr += "p {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 14px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h3>Note: Go to 'http://ip_address/update' to perform OTA update</h3>\n";
  ptr += "<h1>WiFi Clock</h1>\n";
  ptr += "<h2>" + String(CLOCK_TITLE) + "</h2>\n";

  ptr += "<form action=/action_page>\n";
  ptr += "<table>\n";
  ptr += "<tr align=center>\n";
  ptr += "<td align=right style=\"color: " + String(FRG_COLOR) + ";\">Offset (seconds):</td>\n";
  ptr += "<td align=left><input type=text name=offset value=" + String(o) + "></td>\n";
  ptr += "</tr>\n";
  ptr += "</table>\n";
  ptr += "<input type=submit value=\"Save To EEPROM\">\n";
  ptr += "</form>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
String action_page() {
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>" + String(CLOCK_TITLE) + "</title>\n";
  ptr += "<style>html { font-family: Helvetica; font-color: " + String(FRG_COLOR) + "; display: table; margin: 0px auto; text-align: -webkit-center;}\n";
  ptr += "body{margin-top: 50px;background-color:" + String(BKG_COLOR) + ";} h1 {font-size: 40px;color: " + String(FRG_COLOR) + ";margin-bottom: 0px;} h2 {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 15px;} h3 {font-size: 15px;color: " + String(FRG_COLOR) + ";margin-bottom: 20px;}\n";
  ptr += "p {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 14px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h3>Note: Go to 'http://ip_address/update' to perform OTA update</h3>\n";
  ptr += "<h1>WiFi Clock</h1>\n";
  ptr += "<h2>" + String(CLOCK_TITLE) + "</h2>\n";

  ptr += "<form action=/reboot>\n";
  ptr += "<table>\n";
  ptr += "<tr align=center>\n";
  ptr += "<td align=right style=\"color: " + String(FRG_COLOR) + ";\">Offset is set to " + String(o) + " seconds </td>\n";
  ptr += "<td align=left><input type=submit value=\"Click here to reboot and apply!\"></td>\n";
  ptr += "</tr>\n";
  ptr += "</table>\n";
  ptr += "</form>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void getNTPTime() {
  if (!gotNTPTime) {
    if (timeClient.forceUpdate()) {
      long o;
      EEPROM.get(EEPROM_ADDRESS, o);
      setTime(timeClient.getEpochTime() + o);
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
  server.send(200, "text/html", MAIN_page());  //Send web page
}

void handleForm() {
  String o = server.arg("offset");
  EEPROM.put(EEPROM_ADDRESS, o.toInt());
  EEPROM.commit();
  server.send(200, "text/html", action_page());  //Send web page
}

void handleReboot() {
  server.send(200, "text/html", MAIN_page());  //Send web page
  delay(5000);
  ESP.restart();
}

void setup() {
  Disp.setBrightness(DISPLAY_BRIGHTNESS);  //maximum is 1024
  Disp.start();
  Disp.setPixel(0, 15, true);
  Disp.loop();

  EEPROM.begin(512);
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  if (o > 14 * 3600 || o < -12 * 3600) {
    // ofs = 0;
    EEPROM.put(EEPROM_ADDRESS, 0);
    EEPROM.commit();
  }

  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP("WAW20_AE1", "!219adeltawab");
  wifiMulti.addAP("WAW20_AE2", "!219adeltawab");
  wifiMulti.addAP("WAW20_AE4", "!219adeltawab");
  wifiMulti.run();
  WiFi.hostname("AEDMD2");
  while (wifiMulti.run() != WL_CONNECTED) {}

  ElegantOTA.begin(&server);
  server.on("/", handleRoot);
  server.on("/action_page", handleForm);
  server.on("/reboot", handleReboot);
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
