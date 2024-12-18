#include "I2C_AXP192.h"

I2C_AXP192 axp192(I2C_AXP192_DEFAULT_ADDRESS, Wire1);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire1.begin(21, 22);

  I2C_AXP192_InitDef initDef = {
    .EXTEN  = true,
    .BACKUP = true,
    .DCDC1  = 3300,
    .DCDC2  = 0,
    .DCDC3  = 0,
    .LDO2   = 3000,
    .LDO3   = 500,
    .GPIO0  = 2800,
    .GPIO1  = -1,
    .GPIO2  = -1,
    .GPIO3  = -1,
    .GPIO4  = -1,
  };
  axp192.begin(initDef);

}

void loop() {
  Serial.printf("getBatteryVoltage()          : %7.2f\n", axp192.getBatteryVoltage());
  Serial.printf("getBatteryDischargeCurrent() : %7.2f\n", axp192.getBatteryDischargeCurrent());
  Serial.printf("getBatteryChargeCurrent()    : %7.2f\n", axp192.getBatteryChargeCurrent());
  Serial.printf("getAcinVolatge()             : %7.2f\n", axp192.getAcinVolatge());
  Serial.printf("getAcinCurrent()             : %7.2f\n", axp192.getAcinCurrent());
  Serial.printf("getVbusVoltage()             : %7.2f\n", axp192.getVbusVoltage());
  Serial.printf("getVbusCurrent()             : %7.2f\n", axp192.getVbusCurrent());
  Serial.printf("getInternalTemperature()     : %7.2f\n", axp192.getInternalTemperature());
  Serial.printf("getApsVoltage()              : %7.2f\n", axp192.getApsVoltage());
  Serial.printf("getPekPress()                : %4d\n"  , axp192.getPekPress());
  Serial.printf("getBatteryPower()            : %4d\n"  , axp192.getBatteryPower());
  Serial.printf("is battery charging          : %s\n", axp192.getBatteryChargeCurrent() ? "YES" : "NO");

  Serial.println();

  axp192.setGPIO1(true);
  delay(500);
  axp192.setGPIO1(false);
  delay(500);
}
