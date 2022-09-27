#include "_jokes_func.h"
#include "_vfd_func.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define host "http://official-joke-api.appspot.com/random_joke"
String joke_string_joke;
String joke_string_punchline;
String joke_string;
bool quote_obtained = false;

WiFiClient wifiClient;
HTTPClient http;    //Declare object of class HTTPClient
DynamicJsonBuffer jsonBuffer;

void jokes_init (void) {
  http.begin(wifiClient, host);
  joke_string_joke.reserve(30);
  joke_string_punchline.reserve(20);
  joke_string.reserve(150);
}

void jokes_get (void) {
  http.begin(wifiClient, host);     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server
  if (httpCode == 200) {
    JsonObject& root = jsonBuffer.parseObject(payload);
    joke_string_joke = root["setup"].as<String>();
    joke_string_punchline = root["punchline"].as<String>();
    joke_string = joke_string_joke + "          ~~~>           " + joke_string_punchline;
  }
  else {
    joke_string = "ERR: NULL GET RESPONSE!";
  }
  quote_obtained = true;
}
