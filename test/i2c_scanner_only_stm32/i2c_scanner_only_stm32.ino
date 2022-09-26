#include <Wire.h>

HardwareTimer timer(1);

void setup() {
  timer.setMode(TIMER_CH1, TIMER_PWM);
  timer.pause();
  timer.setPrescaleFactor(1);
  timer.setOverflow(3); // ~24Mhz
  timer.refresh();
  timer.resume();
  pinMode(PA8, PWM);  // setup the pin as PWM
  Serial.begin();
  while (!Serial)
  {
    delay(100);
  }
  Wire.begin();
  Serial.println("\nI2C Scanner");
}

void loop() {
    pwmWrite(PA8, 2);
byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");

  delay(5000); // wait 5 seconds for next scan
}
