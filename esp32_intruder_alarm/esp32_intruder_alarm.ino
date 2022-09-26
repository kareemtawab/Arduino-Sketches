#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>

#define reed_pin 4
#define led LED_BUILTIN
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"

WebServer server(80);
BlynkTimer timer;

char auth[] = "pKkCgNPbav_Ezfbngg9G6rfF2ClzTrBX";
bool detected;

void blinker() {
  if (!digitalRead(reed_pin)) {
    digitalWrite(led, HIGH);
    detected = false;
  }
  else {
    digitalWrite(led, !digitalRead(led));
    detected = true;
  }
}

void statusled() {
  if (detected) {
    Blynk.setProperty(V0, "color", BLYNK_RED);
  }
  else {
    Blynk.setProperty(V0, "color", BLYNK_GREEN);
  }
}

void setup(void) {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.begin(115200);
  WiFiManager wm;
  wm.setConnectTimeout(60);
  wm.setConfigPortalTimeout(60);
  if (!wm.autoConnect("ESP32 Intruder Alarm #1")) {
    ESP.restart();
  }
  server.on("/", []() {
    server.send(200, "text/plain", "Hi! I am ESP32.  Visit <IP address>/update to uplad sketch.");
  });
  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Blynk.config(auth);
  Blynk.connect();
  pinMode(led, OUTPUT);
  pinMode(reed_pin, INPUT);
  timer.setInterval(250L, blinker);
  timer.setInterval(1000L, statusled);
}

void loop(void) {
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  server.handleClient();
}
