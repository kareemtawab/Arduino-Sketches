#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <RH_NRF24.h>

#define SEALEVELPRESSURE_HPA (1013.25)

RH_NRF24 nrf24(2, 15); // CE, CSN
Adafruit_BME280 bme; // I2C

void setup() {
  Serial.begin(9600);
  Serial.println();
  nrf24.init();
  nrf24.setChannel(1);
  nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);
  bme.begin(0x76, &Wire);
}


void loop() {
  /*
    Serial.print("T");
    Serial.print(bme.readTemperature());

    Serial.print(" P");
    Serial.print(bme.readPressure() / 100.0F);

    Serial.print(" A");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));

    Serial.print(" H");
    Serial.print(bme.readHumidity());
    Serial.println();
  */
  if (nrf24.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.println((char*)buf);
    }
  }
}
