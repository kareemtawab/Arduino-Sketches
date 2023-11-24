#include "_jokes_func.h"
#include "_vfd_func.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define host "http://official-joke-api.appspot.com/random_joke"
String jerky_bottom_string_joke;
String jerky_bottom_string_punchline;
String jerky_bottom_string;
bool joke_obtained = false;

WiFiClient wifiClient;
HTTPClient http;    //Declare object of class HTTPClient
DynamicJsonBuffer jsonBuffer;

void jokes_init (void) {
  http.begin(wifiClient, host);
  jerky_bottom_string_joke.reserve(30);
  jerky_bottom_string_punchline.reserve(30);
  jerky_bottom_string.reserve(200);
}

void jokes_get (void) {
  http.begin(wifiClient, host);     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server
  if (httpCode == 200) {
    JsonObject& root = jsonBuffer.parseObject(payload);
    jerky_bottom_string_joke = root["setup"].as<String>();
    jerky_bottom_string_punchline = root["punchline"].as<String>();
    jerky_bottom_string = jerky_bottom_string_joke + "          --->           " + jerky_bottom_string_punchline;
  }
  else {
    jerky_bottom_string = "ERR: NULL GET RESPONSE!";
  }
  joke_obtained = true;
}
