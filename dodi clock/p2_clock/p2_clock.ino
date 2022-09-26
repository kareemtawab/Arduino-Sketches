#include <SevSeg.h>
#include <Wire.h>

#define BLINKER_LED_PIN A1

SevSeg sevseg;

long previousMillis = 0;
long interval = 500;
char time_from_master[12];
long time_to_7seg;
int h, m, s;

void setup() {
  Serial.begin(115200);

  Wire.begin(9);
  Wire.onReceive(receiveEvent);

  pinMode(BLINKER_LED_PIN, OUTPUT);

  byte hardwareConfig = P_TRANSISTORS; // See README.md for options
  byte numDigits = 6;
  byte digitPins[] = {7, 6, 5, 4, 3, 2}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
  byte segmentPins[] = {12, 11, 8, 9, 10, 13, A0}; //Segments: A,B,C,D,E,F,G
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = true; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);  //sevseg2.begin(COMMON_ANODE, numDigits2, digitPins2, segmentPins2);
  sevseg.setBrightness(75); //Note: 100 brightness simply corresponds to a delay of 2000us after lighting each segment
}

void receiveEvent(int howMany) {
  Wire.readBytes(time_from_master, howMany);
  //time_from_master = Wire.read();    // read one character from the I2C
  Serial.print("Recieved from master: ");
  Serial.println(time_from_master);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    digitalWrite(BLINKER_LED_PIN, !(digitalRead(BLINKER_LED_PIN)));

    sscanf(time_from_master, "h%02dm%02ds%02d", &h, &m, &s);
    time_to_7seg = (long)h * 10000 +
                   (long)m * 100 +
                   (long)s;
    Serial.print("Sending to display: ");
    Serial.println(time_to_7seg);
  }
  sevseg.setNumber(time_to_7seg);
  sevseg.refreshDisplay(); // Must run repeatedly; don't use blocking code (ex: delay()) in the loop() function or this won't work right
}
