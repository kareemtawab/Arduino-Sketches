#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ElegantOTA.h>  // https://github.com/ayushsharma82/ElegantOTA/tree/master تحميل المكتبة من
#include <TimeLib.h>     // https://github.com/PaulStoffregen/Time/tree/master تحميل المكتبة من
#include "Timer.h"       // https://github.com/JChristensen/Timer/tree/master تحميل المكتبة من

#include <MD_Parola.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include "JF_Font_Data.h"

#include <EEPROM.h>

#define BKG_COLOR "MediumSeaGreen"
#define FRG_COLOR "white"
#define CLOCK_TITLE "Next-To-Router Matrix Clock"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 15                 // GPIO15 for D8
#define timeToSwitchOffWiFi 3600  // تحديد عدد الثواني لايقاف الواي فاي بعد الحصول على الوقت من النت
#define EEPROM_ADDRESS 2

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
IPAddress local_IP(192, 168, 1, 54);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 1);
IPAddress secondaryDNS(8, 8, 8, 8);
char *ssid1 = "WAW20_HK1";       // اسم اول شبكة
char *ssid2 = "WAW20";           // اسم ثاني شبكة
char *ssid3 = "WAW20_AE4";       // اسم ثالث شبكة
char *pass1 = "!219adeltawab";   // كلمة سر اول شبكة
char *pass2 = "!219adeltawab";   // كلمة سر ثاني شبكة
char *pass3 = "!219adeltawab";   // كلمة سر ثالث شبكة
char *host_name = "AELEDClock";  // اسم المتحكم على الشبكة

bool gotNTPTime;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
time_t ntptime;
Timer t;

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t nightsymbol[] = { 10, 0x2, 0x27, 0x42, 0xc0, 0xc1, 0x67, 0x7e, 0x3c, 0x1, 0x83 };
uint8_t daysymbol[] = { 10, 0x12, 0x49, 0x24, 0x92, 0x48, 0x23, 0x87, 0x4f, 0x1f, 0x9f };

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

void updateDisplay() {
  String strhour, strminute;
  char dayornight, separator;
  if (hourFormat12() < 10) {
    strhour = "0" + String(hourFormat12());
  } else {
    strhour = String(hourFormat12());
  }
  if (minute() < 10) {
    strminute = "0" + String(minute());
  } else {
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
  } else {
    separator = ' ';
  }
  P.print(strhour + separator + strminute + dayornight);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("LED Matrix Clock");

  EEPROM.begin(512);
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  if (o > 14 * 3600 || o < -12 * 3600) {
    // ofs = 0;
    EEPROM.put(EEPROM_ADDRESS, 0);
    EEPROM.commit();
  }

  P.begin();
  P.setIntensity(1);
  P.addChar('<', nightsymbol);
  P.addChar('>', daysymbol);
  P.setFont(jF_Custom);
  P.setInvert(false);
  P.print(".con");

  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);
  wifiMulti.run();
  WiFi.hostname(host_name);

  P.print(".conok");
  delay(2500);
  P.print(".upd");

  ElegantOTA.begin(&server);
  server.on("/", handleRoot);
  server.on("/action_page", handleForm);
  server.on("/reboot", handleReboot);
  server.begin();

  timeClient.begin();
  getNTPTime();

  t.every(5000, getNTPTime);
  t.every(50, updateDisplay);
}

void loop() {
  // yield();
  t.update();

  if (millis() / 1000 > timeToSwitchOffWiFi && gotNTPTime) {
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  } else {
    wifiMulti.run();
    server.handleClient();
  }
}
