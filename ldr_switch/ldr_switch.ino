//#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
//#include <LiquidCrystal_I2C.h>          // for LCD w/ GPIO MODIFIED for the ATtiny85
//#define GPIO_ADDR     0x3F             // (PCA8574A A0-A2 @5V) typ. A0-A3 Gnd 0x20 / 0x38 for A - 0x27 is the address of the Digispark LCD modules.
//LiquidCrystal_I2C lcd(GPIO_ADDR, 16, 2); // set address & 16 chars / 2 lines

#define LDR A3              // analog pin to which LDR is connected
#define R 1
int LDRValue;           // variable to store LDR values
int threshold = 15;              // light sensitivity threshold

void setup()
{
  pinMode(R, OUTPUT);                       // configure LED pin to behave as output
  digitalWrite(R, HIGH);

  //TinyWireM.begin();                    // initialize I2C lib - comment this out to use with standard arduinos
  //lcd.init();                           // initialize the lcd
  //lcd.backlight();                      // Print a message to the LCD.
}

void loop()
{
  LDRValue = map(analogRead(LDR), 580, 1023, 0, 100);                 // read LDR value
  //lcd.clear();
  //lcd.print(LDRValue);

  if (LDRValue < threshold)                  // if the LDR reading is lower than the threshold
  {
  digitalWrite(R, LOW);
  }
  else
  {
  digitalWrite(R, HIGH);
  }
  delay(60000);
}
