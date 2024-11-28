#include "_serial_func.h"
#include "Arduino.h"

#define MAX_BUF_LENGTH 300
#define HEADER_BYTE_OFFSET 3
int bms_rx_msg[MAX_BUF_LENGTH];
int bms_rx_msg_len;

void init_serial(void) {
  Serial.begin(115200);
  Serial3.begin(9600);
}

void serial_update(void) {
  getBMSdata();
}

void system_print_serial(void) {
}

void getBMSdata() {
  const size_t packet_length = 8;
  uint8_t packet[packet_length] = { 0x81, 0x03, 0x00, 0x00, 0x00, 0x7f, 0x1b, 0xea };
  Serial3.write(packet, packet_length);

  while (Serial3.available() > 0) {
    int first_character = Serial3.read();
    if (first_character == 0x51) {
      bms_rx_msg[0] = 0x51;
      for (int i = 1; i < MAX_BUF_LENGTH; i++) {
        int read_character = Serial3.read();
        if (read_character != -1) bms_rx_msg[i] = read_character;
        else {
          bms_rx_msg_len = i;
          break;
        }
      }
    }
  }
}