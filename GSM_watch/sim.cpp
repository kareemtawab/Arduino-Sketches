#include "sim.h"
#include "config.h"
#include <Arduino.h>
#include <TimerEvent.h>
#include <GsmAsync.h>

const char* SIM800LCallerNames[] = {FIRST_NAME, SECOND_NAME};
const char* SIM800LCallerNumbers[] = {FIRST_NUMBER, SECOND_NUMBER};
char SIM800LCallerName[4];
char SIM800LCallerNumber[15];
char SIM800LOperatorName[15];
uint8_t SIM800LSignalQuality;
uint8_t SIM800LStatus = 2;
uint8_t requestOrder = 1;
uint8_t callTime;
char callTimeFormatted[6];
bool isKnownCaller = true;
bool isUARTEnabled = true;
bool occuredOnce = false;

extern uint16_t secsToSleep;

TimerEvent requestSIMServiceParamsTimer;
GsmAsync gsmAsync;

void simInit() {
  pinMode(SIM800LRST_PIN, OUTPUT);
  //resetSimModule();
  Serial.begin(9600);
  Serial.println(F("ATE0"));
  delay(200);
  Serial.println(F("AT+CLIP=1"));
  gsmAsync.init(&Serial);
  gsmAsync.registerHandler(new GsmHandler{"+CPAS:", getSIMStatus});
  gsmAsync.registerHandler(new GsmHandler{"+CSPN:", getSIMOperatorName});
  gsmAsync.registerHandler(new GsmHandler{"+CSQ:", getSIMSignalQuality});
  gsmAsync.registerHandler(new GsmHandler{"+CLIP:", getSIMCaller});
  requestSIMServiceParamsTimer.set(400, requestSIMServiceParams);
}

void simInitSerialOnly() {
  pinMode(SIM800LRST_PIN, OUTPUT);
  resetSimModule();
  Serial.begin(9600);
  Serial.println(F("ATE0"));
  delay(100);
  Serial.println(F("AT+CLIP=1"));
  delay(100);
  Serial.println(F("AT+CRSL=100"));
  delay(100);
  Serial.println(F("AT+CLVL=100"));
}

void resetSimModule() {
  digitalWrite(SIM800LRST_PIN, LOW);
  delay(100);
  digitalWrite(SIM800LRST_PIN, HIGH);
}

void loopSIM() {
  gsmAsync.doLoop();
}

void requestDataFromSIMTimerUpdate() {
  requestSIMServiceParamsTimer.update();
}

void switchOffSimModule() {
  digitalWrite(SIM800LRST_PIN, LOW);
}

void switchOnSimModule() {
  digitalWrite(SIM800LRST_PIN, HIGH);
}

void forceSimModuleToSleep() {
  isUARTEnabled = false;
  occuredOnce = true;
  if (!occuredOnce) gsmAsync.addCommand("AT+CSCLK=2");
}

void releaseSimModuleFromSleep() {
  occuredOnce = false;
  if (occuredOnce) {
    gsmAsync.addCommand("AT+CSCLK=0");
    gsmAsync.addCommand("AT+CSCLK=0");
    gsmAsync.addCommand("AT+CSCLK=0");
  }
  isUARTEnabled = true;
}

void getSIMSignalQuality(char* result) {
  if (result[1] == ',') SIM800LSignalQuality = result[0] - '0';
  if (result[2] == ',') SIM800LSignalQuality = (result[0] - '0') * 10 + (result[1] - '0');
  SIM800LSignalQuality = SIM800LSignalQuality * 100 / 31;
}

void getSIMOperatorName(char* result) {
  sscanf(result, "\"%[^\"]\"", &SIM800LOperatorName);
}

void getSIMStatus(char* result) {
  SIM800LStatus = result[0] - '0';
  //Status 0: Ready
  //Status 1: Unavailable
  //Status 2: Unkown
  //Status 3: Ringing
  //Status 4: In Call
  //Status 5: Asleep
}

void getSIMCaller(char* result) {
  sscanf(result, "\"%[^\"]\"", &SIM800LCallerNumber);
  SIM800LStatus = 3;
  for (int i = 0; i < sizeof(SIM800LCallerNumbers); i++) {
    if (!strcmp(SIM800LCallerNumber, SIM800LCallerNumbers[i])) {
      SIM800LCallerName[0] = SIM800LCallerNames[i][0];
      SIM800LCallerName[1] = SIM800LCallerNames[i][1];
      SIM800LCallerName[2] = SIM800LCallerNames[i][2];
      SIM800LCallerName[3] = '\0';
      isKnownCaller = true;
      break;
    }
    else {
      SIM800LCallerName[0] = '-';
      SIM800LCallerName[1] = '-';
      SIM800LCallerName[2] = '-';
      SIM800LCallerName[3] = '\0';
      isKnownCaller = false;
      break;
    }
  }
}

void requestSIMServiceParams() {
  if (isUARTEnabled) {
    switch (requestOrder) {
      case 1:
        //Serial.println(F("AT+CPAS"));
        gsmAsync.addCommand("AT+CPAS");
        break;
      case 2:
        //Serial.println(F("AT+CSPN?"));
        gsmAsync.addCommand("AT+CSPN?");
        break;
      case 3:
        //Serial.println(F("AT+CSQ"));
        gsmAsync.addCommand("AT+CSQ");
        break;
    }
    (requestOrder < 3) ? requestOrder++ : requestOrder = 1;
  }
}

char* returnRAWResultsBuffer(void) {
  return gsmAsync._buffer;
}

void callFirstNumber(void) {
  strncpy(SIM800LCallerName, SIM800LCallerNames[0], sizeof(SIM800LCallerName));
  Serial.print(F("ATD"));
  Serial.print(SIM800LCallerNumbers[0]);
  Serial.println(F(";"));
}

void callSecondNumber(void) {
  strncpy(SIM800LCallerName, SIM800LCallerNames[1], sizeof(SIM800LCallerName));
  Serial.print(F("ATD"));
  Serial.print(SIM800LCallerNumbers[1]);
  Serial.println(F(";"));
}

void answerCall() {
  Serial.println(F("ATA"));
}

void hangupCall() {
  Serial.println(F("ATH"));
}
