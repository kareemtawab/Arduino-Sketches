#include "_power_func.h"
#include "_serial_func.h"
#include "Arduino.h"

#define RELAY_1_PIN 3
#define RELAY_2_PIN 5
#define RELAY_3_PIN 7
#define RELAY_4_PIN 9
#define HEADER_BYTE_OFFSET 3

extern int bms_rx_msg[];
extern int bms_rx_msg_len;
float v_b[7], v_b_total, c_b;
int soc_b, t_b[5];

void init_power(void) {
  pinMode(RELAY_1_PIN, OUTPUT);  // relays
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, HIGH);
  digitalWrite(RELAY_2_PIN, HIGH);
  digitalWrite(RELAY_3_PIN, HIGH);
  digitalWrite(RELAY_4_PIN, HIGH);
}

void power_type_a_on(void) {
  digitalWrite(RELAY_2_PIN, LOW);
}

void power_type_a_off(void) {
  digitalWrite(RELAY_2_PIN, HIGH);
}

void power_type_c_on(void) {
  digitalWrite(RELAY_3_PIN, LOW);
}

void power_type_c_off(void) {
  digitalWrite(RELAY_3_PIN, HIGH);
}

void power_bt_on(void) {
  digitalWrite(RELAY_1_PIN, LOW);
}

void power_bt_off(void) {
  digitalWrite(RELAY_1_PIN, HIGH);
}

void power_inverter_on(void) {
  digitalWrite(RELAY_4_PIN, LOW);
}

void power_inverter_off(void) {
  digitalWrite(RELAY_4_PIN, HIGH);
}

void power_activate_30(void) {
}

void power_activate_60(void) {
}

void power_update(void){
    parseBMSdata();
    // printBMSdata();
}

void parseBMSdata() {
  for (int i = 0; i < sizeof(v_b); i++) {
    v_b[i] = word(bms_rx_msg[i * 2 + HEADER_BYTE_OFFSET], bms_rx_msg[i * 2 + HEADER_BYTE_OFFSET + 1]) * 0.001;
  }
  v_b_total = word(bms_rx_msg[115], bms_rx_msg[116]) * 0.1;
  soc_b = word(bms_rx_msg[119], bms_rx_msg[120]) * 0.1;
  c_b = (word(bms_rx_msg[117], bms_rx_msg[118]) - 30000) * 0.1;
  t_b[0] = bms_rx_msg[184] - 40;
  t_b[1] = bms_rx_msg[100] - 40;
  t_b[2] = bms_rx_msg[102] - 40;
  t_b[3] = bms_rx_msg[104] - 40;
  t_b[4] = bms_rx_msg[106] - 40;
}

void printBMSdata() {
  Serial.println("Received BMS Data:");
  for (int i = 0; i < bms_rx_msg_len; i++) {
    int start, end;
    if (i == 0) start = 0;
    if (i % 16 == 0 && i != 0) {
      end = i - 1;
      Serial.print(' ');
      Serial.print('(');
      if (start < 10) {
        Serial.print('0');
        Serial.print('0');
        Serial.print(start);
      } else if (start >= 10 && start < 100) {
        Serial.print('0');
        Serial.print(start);
      } else {
        Serial.print(start);
      }
      Serial.print('-');
      if (end < 10) {
        Serial.print('0');
        Serial.print('0');
        Serial.print(end);
      } else if (end >= 10 && end < 100) {
        Serial.print('0');
        Serial.print(end);
      } else {
        Serial.print(end);
      }
      Serial.println(')');
      start = end + 1;
    }
    if (bms_rx_msg[i] < 0x10) {
      Serial.print("0");
      Serial.print(bms_rx_msg[i], HEX);
    } else {
      Serial.print(bms_rx_msg[i], HEX);
    }
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  Serial.println("Parsed BMS Data:");
  Serial.print(v_b[0]);
  Serial.print('\t');
  Serial.print(v_b[1]);
  Serial.print('\t');
  Serial.print(v_b[2]);
  Serial.print('\t');
  Serial.print(v_b[3]);
  Serial.print('\t');
  Serial.print(v_b[4]);
  Serial.print('\t');
  Serial.print(v_b[5]);
  Serial.print('\t');
  Serial.print(v_b[6]);
  Serial.print('\t');
  Serial.print(v_b_total);
  Serial.print('\t');
  Serial.print(soc_b);
  Serial.println('%');
  Serial.println(c_b);
  Serial.print(t_b[0]);
  Serial.print('\t');
  Serial.print(t_b[1]);
  Serial.print('\t');
  Serial.print(t_b[2]);
  Serial.print('\t');
  Serial.print(t_b[3]);
  Serial.print('\t');
  Serial.print(t_b[4]);
  Serial.println();
  Serial.println();
}