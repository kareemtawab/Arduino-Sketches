#include "_batt_led_vib_func.h"
#include "_file_transfer_func.h"
#include <I2C_AXP192.h>

#define BATTERY_VOLTAGE_MIN_LIMIT 2600
#define BATTERY_VOLTAGE_MAX_LIMIT 4200

int vib_delay;
long vib_start;
float vin_voltage, voltage, internal_temp_c, internal_temp_f;
int percent, battery_current;
bool vibration_needed;

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

void batt_led_vib_init(void) {
  axp192.begin(initDef);
}

void batt_led_vib_update(void) {
  vin_voltage = axp192.getAcinVoltage() / 1000;
  voltage = axp192.getBatteryVoltage() / 1000;
  internal_temp_c = axp192.getInternalTemperature();
  internal_temp_f = internal_temp_c * 1.8f + 32.0f;
  percent = constrain(map(axp192.getBatteryVoltage(), BATTERY_VOLTAGE_MIN_LIMIT, BATTERY_VOLTAGE_MAX_LIMIT, 0, 100), 0, 100);
  battery_current = int(axp192.getBatteryChargeCurrent()) - int(axp192.getBatteryDischargeCurrent());
  if (vibration_needed) {
    axp192.setLDO3(1800);
    vTaskDelay(vib_delay);
    axp192.setLDO3(0);
    vibration_needed = false;
  }
  if (battery_current > 0 && vin_voltage > 2) {
    green_led(true);
    vTaskDelay(250);
    green_led(false);
  }
  if (battery_current < 0 && vin_voltage < 2) {
    green_led(false);
  }
  if (battery_current == 0 && vin_voltage > 2) {
    green_led(true);
  }
}

float batt_get_vin_voltage(void) {
  if (vin_voltage == 0) return -1;
  else return vin_voltage;
}

float batt_get_voltage(void) {
  if (voltage == 0) return -1;
  else return voltage;
}

int batt_get_percent(void) {
  // return random(80, 99);
  if (percent == 0) return -1;
  else return percent;
}

float get_internal_temp_c(void) {
  if (internal_temp_c < 0) return -1;
  else return internal_temp_c;
}

float get_internal_temp_f(void) {
  if (internal_temp_f < 0) return -1;
  else return internal_temp_f;
}

void green_led(bool s) {
  axp192.setGPIO1(s);
}

void vibrate(int _delay) {
  vib_delay = _delay;
  vibration_needed = true;
}

void power_off(void) {
  axp192.powerOff();
}

void restart_now(void) {
  ESP.restart();
}

int get_battery_current(void) {
  return battery_current;
}
