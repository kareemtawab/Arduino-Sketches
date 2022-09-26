//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WebServer.h>
//#include <ElegantOTA.h>
//#include <WiFiManager.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"
//#include <TimeLib.h>
#include "arduino_secrets.h"
#include "thingProperties.h"

#define led1 LED_BUILTIN
#define led2 23

//WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
EnergyMonitor emon1;             // Create an instance
//WiFiManager wm;
//time_t systemtime;

int realPower;
float powerFActor;
int supplyVoltage;
float Irms;

unsigned long lastmillis;

void lcddraw() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(supplyVoltage, 0);
  lcd.print(" Volts");
  lcd.setCursor(0, 1);
  lcd.print(Irms, 1);
  lcd.print(" Amps");
  lcd.setCursor(12, 0);
  lcd.print(powerFActor, 2);
  lcd.print(" PF");
  lcd.setCursor(0, 2);
  lcd.print(realPower, 1);
  lcd.print(" Watts");

  lcd.setCursor(0, 3);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
}

void emonget() {
  digitalWrite(led2, HIGH);
  emon1.calcVI(100, 2500);        // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.calcVI(115, 2500);        // Calculate all. No.of half wavelengths (crossings), time-out
  digitalWrite(led2, LOW);
  realPower       = abs(emon1.realPower);      //extract Real Power into variable
  powerFActor     = abs(emon1.powerFactor);      //extract Power Factor into Variable
  supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  Irms            = emon1.Irms;             //extract Irms into Variable
}

void setup(void) {
  Serial.begin(115200);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(DBG_INFO);
  ArduinoCloud.printDebugInfo();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);

  emon1.voltage(32, 270, 1.5);  // Voltage: input pin, calibration, phase_shift
  emon1.current(35, 23);       // Current: input pin, calibration.
  lcd.backlight();
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Please wait ...");
}

void loop(void) {
  emonget();
  lcddraw();
  ArduinoCloud.update();
  //wm.process();
  //server.handleClient();
  //accpower += (((millis() - lastmillis) * realPower) / 3600000000);
  //lastmillis = millis();
}
