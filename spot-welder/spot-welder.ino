#include <max7219.h>

#define POT_PIN 18
#define SW_PIN 5
#define SPK_PIN 6
#define SSR_PIN 9
#define LED_BRIG 200

MAX7219 max7219;
// MAX_CLK 11
// MAX_CS 12
// MAX_DIN 10

int count, pulse_length;
bool button_was_pressed, pulse_executed;
char display_data[20];

void setup() {
  max7219.Begin();
  max7219.MAX7219_SetBrightness(LED_BRIG);
  pinMode(SW_PIN, INPUT);
  pinMode(SPK_PIN, OUTPUT);
  pinMode(SSR_PIN, OUTPUT);
  digitalWrite(SPK_PIN, HIGH);
  delay(25);
  digitalWrite(SPK_PIN, LOW);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    pulse_length += map(analogRead(POT_PIN), 1023, 0, 50, 950);
    delay(2);
  }
  pulse_length = pulse_length / 11;                               // calculates average
  pulse_length = (pulse_length / 5 + (pulse_length % 5 > 2)) * 5; // rounds to the nearest 5ms
  sprintf(display_data, "%03d%03d", count, pulse_length);
  max7219.DisplayText(display_data, 1);                           // prints to display

  while (digitalRead(SW_PIN) == LOW) {
    sprintf(display_data, "%s", " . . . . . .");                  // blanks display while button pressed
    max7219.DisplayText(display_data, 1);                         // prints to display
    digitalWrite(SPK_PIN, HIGH);
    delay(25);
    digitalWrite(SPK_PIN, LOW);
    delay(300);
    digitalWrite(SPK_PIN, HIGH);
    digitalWrite(SSR_PIN, HIGH);
    delay(pulse_length);
    digitalWrite(SSR_PIN, LOW);
    digitalWrite(SPK_PIN, LOW);
    count > 999 ? count = 0 : count++;                            // doesn't display for more than 999 events
    while (digitalRead(SW_PIN) == LOW) {}                         // waits forever until button is depressed/released
    delay(100);                                                   // allows for button bounce to be depleted
  }
  delay(50);                                                      // for stability
}
