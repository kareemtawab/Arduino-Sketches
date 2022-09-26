#include "MapFloat.h"
#include "Timer.h"

#define LOW_CELL_PIN  A1 // 4.2v
#define HIGH_CELL_PIN A2 // 8.4v
#define BUZZER_PIN 10
#define RELAY_PIN 3

int low_cell_adc;
float low_cell_voltage;
int high_cell_adc;
float high_cell_voltage;
bool prescence_of_low_voltage_in_low_cell;
bool prescence_of_low_voltage_in_high_cell;
int count;
int secs;

Timer t;

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  t.every(5000, beat);
  t.every(100, get_voltages);
  t.every(250, get_states);
  t.every(500, print_serial);
}

void loop() {
  t.update();
}

void get_voltages(void) {
  low_cell_adc = analogRead(LOW_CELL_PIN);
  high_cell_adc = analogRead(HIGH_CELL_PIN);
  low_cell_voltage = mapFloat(low_cell_adc, 19, 862, 0.0, 4.2);
  high_cell_voltage = mapFloat(high_cell_adc, 19, 850, 0.0, 8.4) - low_cell_voltage;
}

void get_states(void) {
  if (low_cell_voltage < 3.00) {
    prescence_of_low_voltage_in_low_cell = true;
    count++;
    secs = count / 4;
  }
  else {
    prescence_of_low_voltage_in_low_cell = false;
    count = 0;
  }

  if (high_cell_voltage < 3.00) {
    prescence_of_low_voltage_in_high_cell = true;
    count++;
    secs = count / 4;
  }
  else {
    prescence_of_low_voltage_in_high_cell = false;
    count = 0;
  }
  if (secs > 5) {
    Serial.println("BATTERY EMPTY. ALARM!");
    while (1) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(250);
    }
  }
}

void print_serial(void) {
  Serial.println();
  Serial.print("LOW CELL VOLTAGE = ");
  Serial.print(low_cell_voltage);
  Serial.print(" volts, STATE = ");
  if (prescence_of_low_voltage_in_low_cell) {
    Serial.println("WARNING!");
  }
  else {
    Serial.println("OK");
  }
  Serial.print("HIGH CELL VOLTAGE = ");
  Serial.print(high_cell_voltage);
  Serial.print(" volts, STATE = ");
  if (prescence_of_low_voltage_in_high_cell) {
    Serial.println("WARNING!");
  }
  else {
    Serial.println("OK");
  }
}

void beat(void) {
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);

}
