#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

float v;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.init();
  lcd.backlight();
}

void loop() {
  double Vcc;
  Vcc = readVcc() / 1000.0;
  unsigned int  ADCforV = analogRead(A0);
  v = ADCforV * Vcc / 1024;
  lcd.setCursor(0, 0);
  lcd.print(Vcc);
  lcd.setCursor(5, 0);
  lcd.print(v);
  delay(200);
}

long readVcc() {

  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(5); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;

}

