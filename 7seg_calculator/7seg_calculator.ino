#include <TM1650.h>
#include <IRremote.h> //include the library
#include "Timer.h"

#define LED_PIN 13
#define IR_PIN 11

/*
  #define NUM_0 0xF708FD02
  #define NUM_1 0xF30CFD02
  #define NUM_2 0xF20DFD02
  #define NUM_3 0xF10EFD02
  #define NUM_4 0xEF10FD02
  #define NUM_5 0xEE11FD02
  #define NUM_6 0xED12FD02
  #define NUM_7 0xEB14FD02
  #define NUM_8 0xEA15FD02
  #define NUM_9 0xE916FD02
  #define ADD   0xF906FD02
  #define SUB   0xFA05FD02
  #define MUL   0xFD02FD02
  #define DIV   0xFE01FD02
  #define EQ    0xFB04FD02
  #define CLR   0xFF00FD02
*/

#define NUM_0 0xE619FF00
#define NUM_1 0xBA45FF00
#define NUM_2 0xB946FF00
#define NUM_3 0xB847FF00
#define NUM_4 0xBB44FF00
#define NUM_5 0xBF40FF00
#define NUM_6 0xBC43FF00
#define NUM_7 0xF807FF00
#define NUM_8 0xEA15FF00
#define NUM_9 0xF609FF00
#define ADD   0xAD52FF00
#define SUB   0xE718FF00
#define MUL   0xF708FF00
#define DIV   0xA55AFF00
#define EQ    0xE31CFF00
#define CLR   0xF20DFF00
#define SEC   0xE916FF00

#define NUM1_INPUT 1
#define OP_INPUT 0
#define NUM2_INPUT 2
#define RESULT_OUTPUT 3

int state = NUM1_INPUT; // state of calculator input of first number or second number or input of operation sign or preview result
unsigned int num1_0, num1_1, num1_2, num1_3; // digits of num1 from right to left
unsigned int num2_0, num2_1, num2_2, num2_3; // digits of num2 from right to left
unsigned long num1, num2; // the real first and second numbers
long res; // the result
unsigned int num1_pointer; // pointer to digits in num1
unsigned int num2_pointer; // pointer to digits in num2
unsigned int remainder;
unsigned int counter;
unsigned int n1, n2; // fragments of first and second numbers to be drawn on display
char button_code;
char op;
bool button_pressed;
bool clear_calc = false;
bool done = false;
bool nan = false;
bool valid_remainder = false;
bool seconds_mode = false;

//uint32_t Previous;
IRrecv ir(IR_PIN); //create a new instance of receiver
decode_results results;

Timer t;

TM1650 disp(A4, A5);  // SDA = A4, SCL = A5

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(4800);
  Serial.println("EYAD's Seven Segment Calculator 2021");
  ir.enableIRIn(); //start the receiver.
  disp.setupDisplay(true, 7); // set intensity 0-7, 7=highest
  disp.setDisplayToString("EYAd", B0001, 0); // set decimal point in the far right digit
  delay (2500);
  disp.setDisplayToString("CALC", B0001, 0); // set decimal point in the far right digit
  delay (1500);
  disp.setDisplayToString("2021", B0001, 0); // set decimal point in the far right digit
  delay (1500);
  t.every(50, button_press);
  t.every(100, calculator_main);
  t.every(250, display_draw);
  t.every(250, led_on_NUM2_INPUT);
  Serial.println("READY");
}

void loop() {
  t.update();
}

void led_on_NUM2_INPUT() {
  if (state == NUM2_INPUT) {
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
}

void display_draw() {
  int i;
  char msg[5];
  if (!seconds_mode) {
    switch (state) {
      case NUM1_INPUT:
        i = n1;
        sprintf(msg, "%4d", i);
        disp.setDisplayToString(msg);
        break;
      case OP_INPUT:
        i = n1;
        sprintf(msg, "%4d", i);
        disp.setDisplayToString(msg);
        break;
      case NUM2_INPUT:
        i = n2;
        sprintf(msg, "%4d", i);
        disp.setDisplayToString(msg);
        break;
      case RESULT_OUTPUT:
        if (nan) {
          disp.setDisplayToString("-E- ", B0001, 0); // set decimal point in the far right digit to indicate error
        }
        else if (valid_remainder) {
          if (res >= 0 && res <= 9 && remainder <= 9 && remainder >= 1) {
            i = res;
            sprintf(msg, "%2dr%d", i, remainder);
            disp.setDisplayToString(msg, B0001, 0); // set decimal point in the far right digit to indicate result
          }
          if (res >= 10 && res <= 99 && remainder <= 9 && remainder >= 1) {
            i = res;
            sprintf(msg, "%2dr%d", i, remainder);
            disp.setDisplayToString(msg, B0001, 0); // set decimal point in the far right digit to indicate result
          }
          if (res >= 0 && res <= 9 && remainder >= 10 && remainder <= 99) {
            i = res;
            sprintf(msg, "%dr%2d", i, remainder);
            disp.setDisplayToString(msg, B0001, 0); // set decimal point in the far right digit to indicate result
          }
          if (res >= 100) {
            disp.setDisplayToString("-o- ", B0001, 0); // set decimal point in the far right digit to indicate error
          }
        }
        else if (res > 9999 || res < -999) {
          disp.setDisplayToString("-o- ", B0001, 0); // set decimal point in the far right digit to indicate error
        }
        else {
          i = res;
          sprintf(msg, "%4d", i);
          disp.setDisplayToString(msg, B0001, 0); // set decimal point in the far right digit to indicate result
        }
        break;
    }
  }
  else {
    counter++;
    i = counter / 4;
    if ( i > 9999) {
      counter = 0;
    }
    else {
      sprintf(msg, "%4d", i);
      disp.setDisplayToString(msg, B0001, 0); // set decimal point in the far right digit
    }
  }
}

void button_press() {
  char i;
  if (ir.decode()) { //if we have received an IR signal
    if (ir.decodedIRData.decodedRawData != 0) {
      button_pressed = true;
      switch (ir.decodedIRData.decodedRawData) {
        case NUM_0 :
          i = '0';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_1 :
          i = '1';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_2 :
          i = '2';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_3 :
          i = '3';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_4 :
          i = '4';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_5 :
          i = '5';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_6 :
          i = '6';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_7 :
          i = '7';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_8 :
          i = '8';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case NUM_9 :
          i = '9';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case ADD :
          i = '+';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case SUB :
          i = '-';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case MUL :
          i = '*';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case DIV :
          i = '/';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case EQ :
          i = '=';
          Serial.print("BUTTON PRESS: '");
          Serial.print(i);
          Serial.print("', CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = i;
          break;
        case CLR :
          Serial.print("BUTTON PRESS: CLR, CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = 0;
          clear_calc = true;
          break;
        case SEC :
          Serial.print("BUTTON PRESS: SEC, CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          seconds_mode = true;
          break;
        default :
          Serial.print("BUTTON PRESS: UNKNOWN, CODE: 0x");
          Serial.println(ir.decodedIRData.decodedRawData, HEX);
          button_code = -1;
          break;
      }
    }
  }
  ir.resume();
}

void calculator_main() {
  if (button_pressed) {
    if (clear_calc) {
      num1_pointer = 0;
      num2_pointer = 0;
      num1_0 = 0;
      num1_1 = 0;
      num1_2 = 0;
      num1_3 = 0;
      num2_0 = 0;
      num2_1 = 0;
      num2_2 = 0;
      num2_3 = 0;
      num1 = 0;
      num2 = 0;
      n1 = 0;
      n2 = 0;
      res = 0;
      remainder = 0;
      state = NUM1_INPUT;
      clear_calc = false;
      done = false;
      nan = false;
      valid_remainder = false;
      seconds_mode = false;
      counter = 0;
      Serial.println("Calculator was reset.");
      Serial.println("READY!");
    }
    else {
      if (!done && !seconds_mode) {
        switch (button_code) {
          case -1 :
            // unkown keycode
            break;
          case '0' :
            if (state == NUM1_INPUT) put_in_num1(0);
            if (state == NUM2_INPUT) put_in_num2(0);
            break;
          case '1' :
            if (state == NUM1_INPUT) put_in_num1(1);
            if (state == NUM2_INPUT) put_in_num2(1);
            break;
          case '2' :
            if (state == NUM1_INPUT) put_in_num1(2);
            if (state == NUM2_INPUT) put_in_num2(2);
            break;
          case '3' :
            if (state == NUM1_INPUT) put_in_num1(3);
            if (state == NUM2_INPUT) put_in_num2(3);
            break;
          case '4' :
            if (state == NUM1_INPUT) put_in_num1(4);
            if (state == NUM2_INPUT) put_in_num2(4);
            break;
          case '5' :
            if (state == NUM1_INPUT) put_in_num1(5);
            if (state == NUM2_INPUT) put_in_num2(5);
            break;
          case '6' :
            if (state == NUM1_INPUT) put_in_num1(6);
            if (state == NUM2_INPUT) put_in_num2(6);
            break;
          case '7' :
            if (state == NUM1_INPUT) put_in_num1(7);
            if (state == NUM2_INPUT) put_in_num2(7);
            break;
          case '8' :
            if (state == NUM1_INPUT) put_in_num1(8);
            if (state == NUM2_INPUT) put_in_num2(8);
            break;
          case '9' :
            if (state == NUM1_INPUT) put_in_num1(9);
            if (state == NUM2_INPUT) put_in_num2(9);
            break;
          case '+' :
            if (state == NUM1_INPUT || state == OP_INPUT) {
              switch (num1_pointer) {
                case 1:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 1);
                  break;
                case 2:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 2);
                  break;
                case 3:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 3);
                  break;
                case 4:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 4);
                  break;
              }
              op = '+';
              state = NUM2_INPUT;
            }
            break;
          case '-' :
            if (state == NUM1_INPUT || state == OP_INPUT) {
              switch (num1_pointer) {
                case 1:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 1);
                  break;
                case 2:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 2);
                  break;
                case 3:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 3);
                  break;
                case 4:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 4);
                  break;
              }
              op = '-';
              state = NUM2_INPUT;
            }
            break;
          case '*' :
            if (state == NUM1_INPUT || state == OP_INPUT) {
              switch (num1_pointer) {
                case 1:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 1);
                  break;
                case 2:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 2);
                  break;
                case 3:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 3);
                  break;
                case 4:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 4);
                  break;
              }
              op = '*';
              state = NUM2_INPUT;
            }
            break;
          case '/' :
            if (state == NUM1_INPUT || state == OP_INPUT) {
              switch (num1_pointer) {
                case 1:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 1);
                  break;
                case 2:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 2);
                  break;
                case 3:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 3);
                  break;
                case 4:
                  num1 = concatenate_num(num1_3, num1_2, num1_1, num1_0, 4);
                  break;
              }
              op = '/';
              state = NUM2_INPUT;
            }
            break;
          case '=' :
            switch (num2_pointer) {
              case 1:
                num2 = concatenate_num(num2_3, num2_2, num2_1, num2_0, 1);
                break;
              case 2:
                num2 = concatenate_num(num2_3, num2_2, num2_1, num2_0, 2);
                break;
              case 3:
                num2 = concatenate_num(num2_3, num2_2, num2_1, num2_0, 3);
                break;
              case 4:
                num2 = concatenate_num(num2_3, num2_2, num2_1, num2_0, 4);
                break;
            }
            if (state == NUM2_INPUT) {
              state = RESULT_OUTPUT;
              switch (op) {
                case '+' :
                  res = num1 + num2;
                  break;
                case '-' :
                  res = num1 - num2;
                  break;
                case '*' :
                  res = num1 * num2;
                  break;
                case '/' :
                  if (num2 == 0) {
                    nan = true;
                  }
                  else {
                    res = num1 / num2;
                    remainder = num1 % num2;
                    if (remainder == 0) {
                      valid_remainder = false;
                    }
                    else {
                      valid_remainder = true;
                    }
                  }
                  break;
              }
              done = true;
              Serial.print(num1);
              Serial.print(op);
              Serial.print(num2);
              Serial.print('=');
              if (nan) {
                Serial.println("NaN");
              }
              else if (valid_remainder) {
                Serial.print(res);
                Serial.print("r");
                Serial.println(remainder);
              }
              else {
                Serial.println(res);
              }
            }
        }
      }
      else {
        if (seconds_mode) {
          Serial.println("SECONDS MODE activated. Press SEC to reset calculator.");
        }
        else {
          Serial.println("Calculator needs to be reset!");
        }
      }
    }
  }
  else {

  }
  button_pressed = false;
}

void put_in_num1(int n) {
  switch (num1_pointer) {
    case 0 :
      num1_0 = n;
      num1_pointer = 1;
      n1 = n;
      break;
    case 1 :
      num1_1 = n;
      num1_pointer = 2;
      n1 = n1 * 10 + n;
      break;
    case 2 :
      num1_2 = n;
      num1_pointer = 3;
      n1 = n1 * 10 + n;
      break;
    case 3 :
      num1_3 = n;
      num1_pointer = 4;
      n1 = n1 * 10 + n;
      state = OP_INPUT;
      break;
  }
}

void put_in_num2(int n) {
  switch (num2_pointer) {
    case 0 :
      num2_0 = n;
      num2_pointer = 1;
      n2 = n;
      break;
    case 1 :
      num2_1 = n;
      num2_pointer = 2;
      n2 = n2 * 10 + n;
      break;
    case 2 :
      num2_2 = n;
      num2_pointer = 3;
      n2 = n2 * 10 + n;
      break;
    case 3 :
      num2_3 = n;
      num2_pointer = 4;
      n2 = n2 * 10 + n;
      break;
  }
}

int concatenate_num(int a, int b, int c, int d, int x) {
  switch (x) {
    case 1:
      return (d);
      break;
    case 2:
      return (c + d * 10);
      break;
    case 3:
      return (b + c * 10 + d * 100);
      break;
    case 4:
      return (a + b * 10 + c * 100 + d * 1000);
      break;
  }
}
