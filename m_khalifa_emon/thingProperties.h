#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

#define THING_ID "5f076fb7-2632-4984-8349-0208ef6123fa"
#define BOARD_ID "399ebea7-0e91-43da-b625-0dc832299cb5"

extern int realPower;
extern float powerFActor;
extern int supplyVoltage;
extern float Irms;

void initProperties() {
  ArduinoCloud.setBoardId(BOARD_ID);
  ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(supplyVoltage, READ, ON_CHANGE, NULL, 1);
  ArduinoCloud.addProperty(Irms, READ, ON_CHANGE, NULL, 0.1);
  ArduinoCloud.addProperty(powerFActor, READ, ON_CHANGE, NULL, 0.01);
  ArduinoCloud.addProperty(realPower, READ, ON_CHANGE, NULL, 1);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_SSID, SECRET_PASS);
