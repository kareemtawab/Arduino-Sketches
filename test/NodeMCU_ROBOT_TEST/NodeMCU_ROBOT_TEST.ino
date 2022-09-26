#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#include <AccelStepper.h>

char auth[] = "18d27665f8574f649746572e50eee977";

//BlynkTimer timer;
WiFiManager wifiManager;
AccelStepper stepper1(1, 12, 13); //     D6 pinStep, D7 pinDirection
AccelStepper stepper2(1, 14, 15); //     D5 pinStep, D8 pinDirection
AccelStepper stepper3(1,  5,  4); //     D1 pinStep, D2 pinDirection
AccelStepper stepper4(1,  0,  2); //     D3 pinStep, D4 pinDirection

int x;
int y;
int a;
int b;

BLYNK_WRITE(V1) {
  x = param[0].asInt(); // assigning incoming value from pin V1 to a variable
  y = param[1].asInt(); // assigning incoming value from pin V1 to a variable

  if (x > 0 && abs(x) > abs(y)) {
    Serial.println("RIGHT");
    stepper1.setSpeed(x);
    stepper2.setSpeed(-x);
  }
  if (x < 0 && abs(x) > abs(y)) {
    Serial.println("LEFT");
    stepper1.setSpeed(x);
    stepper2.setSpeed(-x);
  }

  if (y > 0 && abs(x) < abs(y)) {
    Serial.println("FORWARD");
    stepper1.setSpeed(y);
    stepper2.setSpeed(y);
  }
  if (y < 0 && abs(x) < abs(y)) {
    Serial.println("BACKWARD");
    stepper1.setSpeed(y);
    stepper2.setSpeed(y);
  }
}

BLYNK_WRITE(V2) {
  a = param[0].asInt(); // assigning incoming value from pin V1 to a variable

  if (a > 0) {
    Serial.println("ARM-UP");
    stepper3.setSpeed(a);
  }
  else {
    Serial.println("ARM-DOWN");
    stepper3.setSpeed(a);
  }
}

BLYNK_WRITE(V3) {
  b = param[0].asInt(); // assigning incoming value from pin V1 to a variable

  if (b > 0) {
    Serial.println("BUCKET-EXTEND");
    stepper4.setSpeed(b);
  }
  else {
    Serial.println("BUCKET-RETRACT");
    stepper4.setSpeed(b);
  }
}

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.println();
  Serial.println("NodeMCU ROBOT");

  stepper1.setMaxSpeed(200);
  stepper2.setMaxSpeed(200);
  stepper3.setMaxSpeed(200);
  stepper4.setMaxSpeed(200);

  //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  wifiManager.setConnectTimeout(20);
  wifiManager.autoConnect("NodeMCU Robot");

  Blynk.config(auth);
  Blynk.connect();
}

void loop() {
  Blynk.run();
  //timer.run();
  if (x == 0 && y == 0) {
    stepper1.stop();
    stepper2.stop();
  }
  else {
    stepper1.runSpeed();
    stepper2.runSpeed();
  }
  if (a == 0 && b == 0) {
    stepper3.stop();
    stepper4.stop();
  }
  else {
    stepper3.runSpeed();
    stepper4.runSpeed();
  }
}

