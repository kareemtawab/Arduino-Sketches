#include <Arduino.h>
#include <Wire.h>
#include <FT6X36.h>
#include <I2C_AXP192.h>

FT6X36 ts(&Wire, 39);
I2C_AXP192 axp192(I2C_AXP192_DEFAULT_ADDRESS, Wire);
I2C_AXP192_InitDef initDef = {
  .EXTEN = true,
  .BACKUP = true,
  .DCDC1 = 3300, // cpu power rail
  .DCDC2 = 0, // not connected
  .DCDC3 = 3300, // lcd backlight rail
  .LDO2 = 3300, // peripheral and sd card rail
  .LDO3 = 0, // vibration motor rail
  .GPIO0 = 2800, // bus power enable
  .GPIO1 = 0, // green led
  .GPIO2 = -1,
  .GPIO3 = -1,
  .GPIO4 = -1,
};

void onTouch(TPoint p, TEvent e) {
    Serial.print("X: ");
    Serial.print(p.x);
    Serial.print(", Y: ");
    Serial.println(p.y);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22, 200000);
  axp192.begin(initDef);

  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning for I2C devices ...");
  for (address = 0x01; address < 0x7f; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if (error != 2) {
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }

  delay(100);
  ts.begin();
  ts.registerTouchHandler(onTouch);
}

void loop() {
  ts.loop();
  delay(25);
}
