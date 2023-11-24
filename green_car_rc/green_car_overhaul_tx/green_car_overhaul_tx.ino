#include <ezButton.h>
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

RH_ASK newcon_remote_433mhz;

ezButton left(2, INPUT_PULLUP);  // create ezButton object that attach to pin 6;
ezButton right(3, INPUT_PULLUP);  // create ezButton object that attach to pin 7;
ezButton forward(4, INPUT_PULLUP);  // create ezButton object that attach to pin 8;
ezButton backward(5, INPUT_PULLUP);  // create ezButton object that attach to pin 8;

char *command = "0.0.0.0";

void setup() {
  // Serial.begin(9600);
  newcon_remote_433mhz.init();
  // left.setDebounceTime(2); // set debounce time to 50 milliseconds
  // right.setDebounceTime(2); // set debounce time to 50 milliseconds
  // forward.setDebounceTime(2); // set debounce time to 50 milliseconds
  // backward.setDebounceTime(2); // set debounce time to 50 milliseconds
}

void loop() {
  left.loop(); // MUST call the loop() function first
  right.loop(); // MUST call the loop() function first
  forward.loop(); // MUST call the loop() function first
  backward.loop(); // MUST call the loop() function first

  if (left.isPressed()) command[0] = '1';
  if (left.isReleased()) command[0] = '0';
  if (right.isPressed()) command[2] = '1';
  if (right.isReleased()) command[2] = '0';
  if (forward.isPressed()) command[4] = '1';
  if (forward.isReleased()) command[4] = '0';
  if (backward.isPressed()) command[6] = '1';
  if (backward.isReleased()) command[6] = '0';
  // for (int i = 0; i < strlen(command) + 1; i++) {
  //   if (command[i] != '\0') {
  //     Serial.print(command[i]);
  //   }
  //   else {
  //     Serial.println();
  //   }
  // }
  newcon_remote_433mhz.send((uint8_t *)command, strlen(command));
  newcon_remote_433mhz.waitPacketSent();
  //delay(5);
}
