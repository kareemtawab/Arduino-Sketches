#include "_wifi_func.h"
#include "_vfd_func.h"
#include "_jokes_func.h"
#include "_rtc_func.h"
#include "_sntp_func.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;

IPAddress local_IP(192, 168, 1, 158);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 167);   //optional
IPAddress secondaryDNS(8, 8, 8, 8); //optional

extern String jerky_bottom_string;
extern char top[21];

void wifi_init (void) {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP("WAW20", "!219adeltawab");
  wifiMulti.addAP("WAW20_HK1", "!219adeltawab");
  wifiMulti.addAP("WAW20_HK2", "!219adeltawab");
  wifiMulti.addAP("WAW20_HK3", "!219adeltawab");
  //wifiMulti.addAP("HK SG Note 9", "07081989");
  wifiMulti.run();

  WiFi.hostname("HKJokesDisplay");
  MDNS.begin("HKJokesDisplay");

  server.on("/", []() {
    server.send(200, "text/plain", "Top VFD line: {" + String(top) + "}\n"
                + "Bottom VFD line: {" + String(jerky_bottom_string) + "}\n"
                + "SNTP Epoch = " + sntp_get_epoch() + "\n"
                + "RTC Epoch = " + now() + "\n");
  });
  ElegantOTA.begin(&server);
  server.begin();
}

void wifi_update (void) {
  wifiMulti.run();
  server.handleClient();
}
