#include "_quotes_func.h"
#include "_vfd_func.h"
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

const char* host = "www.reddit.com";
const int httpsPort = 443;
const char fingerprint[] = "259aa2906767e41081ccb26eb7a57fd85f090239";
String quote_string;
bool quote_obtained = false;

// Instantiate the WiFiClientSecure class to use it for creating a TLS connection
WiFiClientSecure client;

void quotes_init (void) {
  client.setFingerprint(fingerprint);
  quote_string.reserve(200);
}

void quotes_get (void) {
  // Connect to reddit.com and fetch the showerthought data using the web client
  if (!client.connect(host, httpsPort)) {
    //Serial.println("connection failed!");
    return;
  }
  String url = "/r/Showerthoughts/top.json?sort=top&limit=1&t=day";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266/0.1\r\n" +
               "Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //Serial.println("headers received");
      break;
    }
  }
  // Get the response from the reddit.com server
  while (client.connected()) {
    if (client.available()) {
      quote_string = client.readString();
      break;
    }
  }
  client.stop();
  // As the JSON-formatted response text can be too long, we'll parse it manually instead of using ArduinoJson
  int quote_start = quote_string.indexOf("\"title\"");
  // The showerthought quote ends right before a ', "' substring
  int quote_end = quote_string.indexOf(", \"", quote_start + 1); // we start the search from the position where "title" is
  quote_string = quote_string.substring(quote_start + 9 + 1, quote_end - 1);
  quote_string.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
  quote_obtained = true;
}
