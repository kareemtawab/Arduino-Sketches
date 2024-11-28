#define MAX_BUF_LENGTH 300
#define HEADER_BYTE_OFFSET 3

int buf[MAX_BUF_LENGTH];

unsigned long previousMillis = 0;
const long interval = 1500;  // interval at which to blink (milliseconds)

int read_buffer_length;

float v[7], v_total, c;
int soc, t[5];

void setup() {
  Serial.begin(115200);
  Serial3.begin(9600);
}

void getBMSdata() {
  const size_t packet_length = 8;
  uint8_t packet[packet_length] = { 0x81, 0x03, 0x00, 0x00, 0x00, 0x7f, 0x1b, 0xea };
  Serial3.write(packet, packet_length);

  while (Serial3.available() > 0) {
    int first_character = Serial3.read();
    if (first_character == 0x51) {
      buf[0] = 0x51;
      for (int i = 1; i < MAX_BUF_LENGTH; i++) {
        int read_character = Serial3.read();
        if (read_character != -1) buf[i] = read_character;
        else {
          read_buffer_length = i;
          break;
        }
      }
    }
  }
}

void parseBMSdata() {
  for (int i = 0; i < 7; i++) {
    v[i] = word(buf[i * 2 + HEADER_BYTE_OFFSET], buf[i * 2 + HEADER_BYTE_OFFSET + 1]) * 0.001;
  }
  v_total = word(buf[115], buf[116]) * 0.1;
  soc = word(buf[119], buf[120]) * 0.1;
  c = (word(buf[117], buf[118]) - 30000) * 0.1;
  t[0] = buf[184] - 40;
  t[1] = buf[100] - 40;
  t[2] = buf[102] - 40;
  t[3] = buf[104] - 40;
  t[4] = buf[106] - 40;
}

void printBMSdata() {
  Serial.println("Received BMS Data:");
  for (int i = 0; i < read_buffer_length; i++) {
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
    if (buf[i] < 0x10) {
      Serial.print("0");
      Serial.print(buf[i], HEX);
    } else {
      Serial.print(buf[i], HEX);
    }
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  Serial.println("Parsed BMS Data:");
  Serial.print(v[0]);
  Serial.print('\t');
  Serial.print(v[1]);
  Serial.print('\t');
  Serial.print(v[2]);
  Serial.print('\t');
  Serial.print(v[3]);
  Serial.print('\t');
  Serial.print(v[4]);
  Serial.print('\t');
  Serial.print(v[5]);
  Serial.print('\t');
  Serial.print(v[6]);
  Serial.print('\t');
  Serial.print(v_total);
  Serial.print('\t');
  Serial.print(soc);
  Serial.println('%');
  Serial.println(c);
  Serial.print(t[0]);
  Serial.print('\t');
  Serial.print(t[1]);
  Serial.print('\t');
  Serial.print(t[2]);
  Serial.print('\t');
  Serial.print(t[3]);
  Serial.print('\t');
  Serial.print(t[4]);
  Serial.println();
  Serial.println();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    getBMSdata();
    parseBMSdata();
    printBMSdata();
  }
}