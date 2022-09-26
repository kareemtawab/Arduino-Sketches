#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(8, 9);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(void) {
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(75);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipe);
  //delay(500);
}

void loop(void) {
  msg[0] = 111;
  bool tx = radio.write(msg, 1);
  if (tx){
        Serial.print(msg[0]);
    Serial.println(": TX OK");
  }
  else{
        Serial.println("TX NOT OK");
  }
  delay(20);
}
