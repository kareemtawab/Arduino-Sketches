#include <MD_Parola.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
//#include "JF_Font_Data.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   D5
#define DATA_PIN  D7
#define CS_PIN    D8
#define SPEED_TIME  25
#define PAUSE_TIME  2500

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
ESP8266WebServer server(80);

IPAddress _ip = IPAddress(192, 168, 1, 164);
IPAddress _gw = IPAddress(192, 168, 1, 1);
IPAddress _sn = IPAddress(255, 255, 255, 0);

int updateinterval = 1 * 60 * 1000;
unsigned long time_now = 0;
uint8_t  curText;
const char  *msg[] =
{
  "Cases",
  "---",
  "Deaths",
  "---",
  "Recover",
  "---",
};

const char* host = "api.thingspeak.com";  //We read the data from this host
const int httpPortRead = 80;

const char* url1 = "/apps/thinghttp/send_request?api_key=C474WNZJ8YC7U46A";     //Change this URL Cases
const char* url2 = "/apps/thinghttp/send_request?api_key=E2KLF2PLDYUBJ18N";   //Deaths
const char* url3 = "/apps/thinghttp/send_request?api_key=E1C9MHS48EZTOEZ1";   //Recovered
int To_remove;      //There are some irrelevant data on the string and here's how I keep the index
//of those characters
String Cases, Deaths, Recovered, Data_Raw, Data_Raw_1, Data_Raw_2, Data_Raw_3; //Here I keep the numbers that I got

WiFiClient client;      //Create a WiFi client and http client
HTTPClient http;

textEffect_t  effect[] =
{
  PA_PRINT,
  PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_MESH,
  PA_SCROLL_UP_RIGHT,
  PA_BLINDS,
  PA_CLOSING,
  PA_RANDOM,
  PA_GROW_DOWN,
  PA_SCAN_VERT,
  PA_SCROLL_DOWN_LEFT,
  PA_WIPE_CURSOR,
  PA_DISSOLVE,
  PA_OPENING,
  PA_CLOSING_CURSOR,
  PA_SCROLL_DOWN_RIGHT,
  PA_SCROLL_RIGHT,
  PA_SLICE,
  PA_SCROLL_DOWN,
};

void updateroutine() {
  if ( http.begin(host, httpPortRead, url1))     //Connect to the host and the url
  {
    int httpCode = http.GET();                //Check feedback if there's a response
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        Data_Raw = http.getString();   //Here we store the raw data string

        Data_Raw_1 = Data_Raw;
        To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"
        Data_Raw_1.remove(0, To_remove + 1);                     //I remove it and everything that's before
        To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"
        Data_Raw_1.remove(To_remove, Data_Raw_1.length());         //I remove it and everything that's after
        //Example: This is the raw data received <td style="font-weight: bold; text-align:right">63,927</td>
        //First we look for ">" and we remove everything before it including it
        //We stay with this 63,927</td>
        //We look for "<" symbol and we remove it + everything after
        //We keep only this 63,927 as string
        Cases = Data_Raw_1;
        Serial.print("Cases: ");  //I choosed to display it on the serial monitor to help you debug
        Serial.println(Cases);
        Data_Raw_2 = Data_Raw;
        To_remove = Data_Raw_2.indexOf("<span>");
        Data_Raw_2.remove(0, To_remove + 6);
        Data_Raw_3 = Data_Raw_2;
        To_remove = Data_Raw_2.indexOf("</span>");
        Data_Raw_2.remove(To_remove, Data_Raw_2.length());

        Deaths = Data_Raw_2;
        Serial.print("Deaths: ");
        Serial.println(Deaths);
        To_remove = Data_Raw_3.indexOf("<span>");
        Data_Raw_3.remove(0, To_remove + 6);
        To_remove = Data_Raw_3.indexOf("<");
        Data_Raw_3.remove(To_remove, Data_Raw_3.length());

        Recovered = Data_Raw_3;
        Serial.print("Recovered: ");
        Serial.println(Recovered);
      }
      msg[1] = const_cast<char*>(Cases.c_str());
      msg[3] = const_cast<char*>(Deaths.c_str());
      msg[5] = const_cast<char*>(Recovered.c_str());
    }
    else //If we can't get data
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else //If we can't connect to the HTTP
  {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}


void setup() {
  Serial.begin(115200);
  P.begin();
  //P.setFont(jF_Custom);
  P.setInvert(false);
  P.print(".con");
  P.displayText(msg[0], PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_SCROLL_UP, PA_SCROLL_UP);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setConfigPortalTimeout(120); // auto close configportal after n seconds
  //wm.setSTAStaticIPConig(_ip, _gw, _sn);
  if (!wm.autoConnect("NodeMCU LED Matrix")) {
    P.print(".res");
    delay(3500);
    ESP.reset();
  }
  Serial.println(WiFi.macAddress());
  server.on("/", []() {
    server.send(200, "text/plain", "NodeMCU LED Matrix\nGo to 'http://<IPAddress>/update' to perform OTA update");
  });
  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  //P.print(".upd");
  updateroutine();
}

void loop() {
  //P.setIntensity(4);
  server.handleClient();
  if (millis() >= time_now + updateinterval) {
    time_now += updateinterval;
    //P.print(".upd");
    updateroutine();
  }
  else {
    //Display stats
    if (P.displayAnimate()) {
      // Set the display for the next string.
      curText = (curText + 1) % ARRAY_SIZE(msg);
      P.setTextBuffer(msg[curText]);
      // Tell Parola we have a new animation
      P.displayReset();
    }
  }
}
