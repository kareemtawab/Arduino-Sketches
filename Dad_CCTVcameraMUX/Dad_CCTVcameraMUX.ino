#include <IRremote.h> //include the library
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <BlinkControl.h>
#include "Timer.h"
#include <avr/wdt.h>

#define CAMERA_COUNT_FOR_SHUFFLE 3

// GAINTECH remote codes
#define IR_KEY_1_1    0x6F90F205
#define IR_KEY_2_1    0x7788F205
#define IR_KEY_3_1    0x7B84F205
#define IR_KEY_4_1    0x7C83F205
#define IR_KEY_AUTO_1 0x718EF205

// Small remote codes
#define IR_KEY_1_2    0xBA45FF00
#define IR_KEY_2_2    0xB946FF00
#define IR_KEY_3_2    0xB847FF00
#define IR_KEY_4_2    0xBB44FF00
#define IR_KEY_AUTO_2 0xE31CFF00

#define TOUCH_KEY_FOR_CH1  11
#define TOUCH_KEY_FOR_CH2  7
#define TOUCH_KEY_FOR_CH3  3
#define TOUCH_KEY_FOR_CH4  2
#define TOUCH_KEY_FOR_AUTO 10

// the multiplexer address select lines (A/B/C)
#define MUXaddressA 3
#define MUXaddressB 2
#define MUXaddressC 4

#define AUTO_INCREMENT 0
#define MANUAL_SELECT 1
#define MPR121_ADDR 0x5A
#define LED0_PIN 13
#define LED1_PIN 9
#define LED2_PIN 10
#define LED3_PIN 11
#define LED4_PIN 12
#define IRQ_PIN 6
#define IR_PIN 5
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

uint16_t lasttouched = 0;
uint16_t currtouched = 0;
int lastButtonID = -1;
bool state = AUTO_INCREMENT;
int counter = 0;
bool s = true;

BlinkControl LED0(LED0_PIN);
BlinkControl LED1(LED1_PIN);
BlinkControl LED2(LED2_PIN);
BlinkControl LED3(LED3_PIN);
BlinkControl LED4(LED4_PIN);
IRrecv ir(IR_PIN); //create a new instance of receiver
Adafruit_MPR121 touch = Adafruit_MPR121();
Timer t;

void setup() {
  pinMode (MUXaddressA, OUTPUT);
  pinMode (MUXaddressB, OUTPUT);
  pinMode (MUXaddressC, OUTPUT);
  pinMode(IRQ_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("4CH Video MUX");
  LED0.begin();
  LED1.begin();
  LED2.begin();
  LED3.begin();
  LED4.begin();
  ir.begin(IR_PIN);
  ir.enableIRIn(); //start the receiver.
  if (!touch.begin(MPR121_ADDR)) {
    s = false;
  }
  t.every(5000, inc);
  wdt_enable(WDTO_8S);
  wdt_reset();
}

void loop() {
  wdt_reset();
  getButtonID();
  switch (state) {
    case AUTO_INCREMENT:
      LED0.on();
      t.update();
      switch (counter) {
        case 0:
          executeLEDandMUX(0);
          break;
        case 1:
          executeLEDandMUX(1);
          break;
        case 2:
          executeLEDandMUX(2);
          break;
        case 3:
          executeLEDandMUX(3);
          break;
      }
      break;
    case MANUAL_SELECT:
      LED0.off();
      switch (lastButtonID) {
        case TOUCH_KEY_FOR_CH1:
          executeLEDandMUX(0);
          break;
        case TOUCH_KEY_FOR_CH2:
          executeLEDandMUX(1);
          break;
        case TOUCH_KEY_FOR_CH3:
          executeLEDandMUX(2);
          break;
        case TOUCH_KEY_FOR_CH4:
          executeLEDandMUX(3);
          break;
      }
      break;
  }
  LED1.loop();
  LED2.loop();
  LED3.loop();
  LED4.loop();
  /*
    for (int i = 0; i < 8; i++) {
      activateMUX(i);
      Serial.println(i);
      delay(2500);
    }
  */
}

void getButtonID() {
  if (s) {
    if (ir.decode()) { //if we have received an IR signal
      //Serial.println(ir.decodedIRData.decodedRawData, HEX);
      switch (ir.decodedIRData.decodedRawData) {
        case IR_KEY_1_1:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH1;
          Serial.print("IR Remote: Button ");
          Serial.print(1);
          Serial.println(" pressed");
          break;
        case IR_KEY_1_2:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH1;
          Serial.print("IR Remote: Button ");
          Serial.print(1);
          Serial.println(" pressed");
          break;
        case IR_KEY_2_1:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH2;
          Serial.print("IR Remote: Button ");
          Serial.print(2);
          Serial.println(" pressed");
          break;
        case IR_KEY_2_2:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH2;
          Serial.print("IR Remote: Button ");
          Serial.print(2);
          Serial.println(" pressed");
          break;
        case IR_KEY_3_1:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH3;
          Serial.print("IR Remote: Button ");
          Serial.print(3);
          Serial.println(" pressed");
          break;
        case IR_KEY_3_2:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH3;
          Serial.print("IR Remote: Button ");
          Serial.print(3);
          Serial.println(" pressed");
          break;
        case IR_KEY_4_1:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH4;
          Serial.print("IR Remote: Button ");
          Serial.print(4);
          Serial.println(" pressed");
          break;
        case IR_KEY_4_2:
          state = MANUAL_SELECT;
          lastButtonID = TOUCH_KEY_FOR_CH4;
          Serial.print("IR Remote: Button ");
          Serial.print(4);
          Serial.println(" pressed");
          break;
        case IR_KEY_AUTO_1:
          state = AUTO_INCREMENT;
          Serial.print("IR Remote: Button ");
          Serial.print("OK");
          Serial.println(" pressed");
          break;
        case IR_KEY_AUTO_2:
          state = AUTO_INCREMENT;
          Serial.print("IR Remote: Button ");
          Serial.print("OK");
          Serial.println(" pressed");
          break;
      }
      ir.resume();
      return;
    }
    currtouched = touch.touched();
    for (uint8_t i = 0; i < 12; i++) {
      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
        if (i == TOUCH_KEY_FOR_AUTO) {
          state = AUTO_INCREMENT;
          Serial.print("MPR121: Button ");
          Serial.print(i);
          Serial.println(" touched");
        }
        else if (i == TOUCH_KEY_FOR_CH1 || i == TOUCH_KEY_FOR_CH2 || i == TOUCH_KEY_FOR_CH3 || i == TOUCH_KEY_FOR_CH4) {
          state = MANUAL_SELECT;
          lastButtonID = i;
          Serial.print("MPR121: Button ");
          Serial.print(i);
          Serial.println(" touched");
        }
        else {
          // error
        }
      }
    }
    lasttouched = currtouched;
  }
}

void executeLEDandMUX(int id) {
  switch (id) {
    case 0:
      activateMUX(7);
      LED1.on();
      LED2.off();
      LED3.off();
      LED4.off();
      break;
    case 1:
      activateMUX(5);
      LED1.off();
      LED2.on();
      LED3.off();
      LED4.off();
      break;
    case 2:
      activateMUX(1);
      LED1.off();
      LED2.off();
      LED3.on();
      LED4.off();
      break;
    case 3:
      activateMUX(0);
      LED1.off();
      LED2.off();
      LED3.off();
      LED4.on();
      break;
    default:
      break;
  }
}

void inc()
{
  if (counter < CAMERA_COUNT_FOR_SHUFFLE - 1) {
    counter++;
  }
  else {
    counter = 0;
  }
}

void activateMUX (const byte channel) {
  int controlPin[] = {MUXaddressA, MUXaddressB, MUXaddressC};
  int muxChannel[8][3] = {
    {0, 0, 0}, //channel 0
    {1, 0, 0}, //channel 1
    {0, 1, 0}, //channel 2
    {1, 1, 0}, //channel 3
    {0, 0, 1}, //channel 4
    {1, 0, 1}, //channel 5
    {0, 1, 1}, //channel 6
    {1, 1, 1}, //channel 7
  };
  for (int i = 0; i < 3; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
}
