#include <SPI.h>      // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define doorstrike 5  // Set doorstrike Pin
#define buzzer 2      // Button pin for WipeMode
#define rxPin 3
#define txPin 4

uint8_t successRead;  // Variable integer to keep if we have Successful Read from Reader

byte readCard[4];  // Stores scanned ID read from RFID Module
char readuidhexformat[10] = { 0 };
String line;

// Create MFRC522 instance.
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
SoftwareSerial esp(rxPin, txPin);

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  line.reserve(25);

  lcd.backlight();
  lcd.init();

  //Serial.begin(115200);  // Initialize serial communications with PC
  esp.begin(9600);       // Initialize serial communications with ESP8266

  pinMode(doorstrike, OUTPUT);
  digitalWrite(doorstrike, LOW);  // Make sure door is locked
  pinMode(buzzer, OUTPUT);

  SPI.begin();         // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();  // Initialize MFRC522 Hardware

  //If you set Antenna Gain to Max it will increase reading distance
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop() {

  successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0
  if (successRead) {
    //Serial.println(readuidhexformat);
    esp.print("." + String(readuidhexformat));
  }

  if (esp.available() > 0) {
    drawOnScreen();
  }
}

//////////////////////////////////////// Default Screen ///////////////////////////////////
void drawOnScreen() {
  //  digitalWrite(buzzer, HIGH);
  //  delay(5);
  //  digitalWrite(buzzer, LOW);

  switch (esp.read()) {
    case '$':
      //Serial.println("GRANTED");
      granted(5000);
      break;
    case '!':
      //Serial.println("DENIED");
      denied();
      break;
    case '-':
      lcd.clear();
      break;
    case '>':
      switch (esp.read()) {
        case '1':
          //Serial.print(">1_");
          lcd.setCursor(0, 0);
          break;
        case '2':
          //Serial.print(">2_");
          lcd.setCursor(0, 1);
          break;
        case '3':
          //Serial.print(">3_");
          lcd.setCursor(0, 2);
          break;
        case '4':
          //Serial.print(">4_");
          lcd.setCursor(0, 3);
          break;
      }
      line = esp.readStringUntil('<');
      //Serial.println(line);
      lcd.print(line);
      break;
  }
}


/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted(uint16_t setDelay) {
  digitalWrite(doorstrike, HIGH);
  lcd.noBacklight();
  digitalWrite(buzzer, HIGH);
  delay(30);
  lcd.backlight();
  delay(470);
  digitalWrite(buzzer, LOW);
  delay(setDelay - 500);  // Hold door lock open for given seconds
  digitalWrite(doorstrike, LOW);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
}

/////////////////////////////////////////  Access Denied   ///////////////////////////////////
void denied() {
  digitalWrite(doorstrike, LOW);  // Relock door
  lcd.noBacklight();
  digitalWrite(buzzer, HIGH);
  delay(30);
  lcd.backlight();
  digitalWrite(buzzer, LOW);
  delay(100);
  lcd.noBacklight();
  digitalWrite(buzzer, HIGH);
  delay(30);
  lcd.backlight();
  digitalWrite(buzzer, LOW);
  delay(100);
  lcd.noBacklight();
  digitalWrite(buzzer, HIGH);
  delay(30);
  lcd.backlight();
  digitalWrite(buzzer, LOW);
}


///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
uint8_t getID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  char uid[50];
  for (uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  sprintf(readuidhexformat, "%02X%02X%02X%02X", readCard[0], readCard[1], readCard[2], readCard[3]);
  mfrc522.PICC_HaltA();  // Stop reading
  return 1;
}
