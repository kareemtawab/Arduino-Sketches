#include "system.h"
#include "display.h"
#include "sim.h"
#include "config.h"
#include "battery.h"
#include "button.h"
#include <Arduino.h>
#include <TimerEvent.h>
#include <avr/wdt.h> /* Header for watchdog timers in AVR */

TimerEvent miscSecondHeartbeatTimer;

uint8_t debugMode = DEBUG_MODE;
bool isSleepActive = false;
uint16_t secsToSleep;

extern uint8_t battStatus, battLevel;
extern bool isBattLow;
extern uint8_t SIM800LStatus;
extern uint8_t callTime;
extern char callTimeFormatted[6];
extern void battInit(void);
extern void battRun(void);
extern int getButtonID(void);
int buttonPress, blinkTick;

void systemInit() {
  pinMode(LED_PIN, OUTPUT);
  wdt_enable(WDTO_8S);  /* Enable the watchdog with a timeout of 4 seconds */
  battInit();
  buttonInit();
  displayInit(BRIGHTNESS);
  if (!debugMode) {
    drawAnimatedSplash();
  }
  else {
    drawStaticSplash();
  }
  wdt_reset();  /* Reset the watchdog */
  if (debugMode != 2) {
    simInit();
  }
  else {
    simInitSerialOnly();
  }
  miscSecondHeartbeatTimer.set(1000, miscSecondHeartbeat);
}

void systemRun() {
  wdt_reset();  /* Reset the watchdog */
  battRun();
  miscSecondHeartbeatTimer.update();
  requestDataFromSIMTimerUpdate();
  //isBattLow = true;
  if (isBattLow && battStatus == 0) {
    switchOffSimModule();
    resetSleepCounter();
    releaseScreenFromSleep();
    generateNewScreenCoordinates(0, 2);
    drawWarningScreen();
    //    for (int i; i < 30000; i++) {
    //      wdt_reset();  /* Reset the watchdog */
    //      delay(1);
    //    }
    //    forceScreenToSleep();
  }
  if (isBattLow && battStatus == 1) {
    switchOffSimModule();
    resetSleepCounter();
    releaseScreenFromSleep();
    generateNewScreenCoordinates(0, 2);
    drawCharingScreen();
  }
  if (!isBattLow) {
    wdt_reset();  /* Reset the watchdog */
    switchOnSimModule();
    generateNewScreenCoordinates(0, 2);
    manageScreenBasedOnSIMStatusAndDebugMode();
    manageActionsBasedOnSIMStatus();
  }
}

void manageScreenBasedOnSIMStatusAndDebugMode() {
  switch (debugMode) {
    case 0:
      loopSIM();
      drawBanner();
      if (SIM800LStatus == 3 || SIM800LStatus == 4) {
        resetSleepCounter();
        releaseScreenFromSleep();
        if (SIM800LStatus == 3) drawIncomingCallScreen();
        if (SIM800LStatus == 4) drawInCallScreen();
      }
      if (!isSleepActive && SIM800LStatus != 3 && SIM800LStatus != 4) {
        if (SIM800LStatus == 0) drawIdleScreen();
        if (SIM800LStatus == 1) drawNetworkScreen();
        if (SIM800LStatus == 2) drawNetworkScreen();
      }
      break;
    case 1:
      loopSIM();
      resetSleepCounter();
      releaseScreenFromSleep();
      drawDebugMode1Screen();
      break;
    case 2:
      while (Serial.available() > 0) {
        drawDebugMode2Screen(Serial.read());
      }
      break;
  }
}

void manageActionsBasedOnSIMStatus() {
  loopButton();
  if (isSleepActive && !isThereAnyButtonEvent()) {
    forceScreenToSleep();
  }
  loopButton();
  if (isSleepActive && isThereAnyButtonEvent()) {
    isSleepActive = false;
    resetSleepCounter();
    releaseScreenFromSleep(); // wakeup screen
    getButtonID();
  }
  loopButton();
  if (!isSleepActive && isThereAnyButtonEvent()) {
    resetSleepCounter();
    releaseScreenFromSleep(); // wakeup screen
    buttonPress = getButtonID();
    switch (SIM800LStatus) {
      case 0:
        if (buttonPress == 1) {
          callFirstNumber(); // call first number
        }
        if (buttonPress == 2) {
          callSecondNumber(); // call second number
        }
        break;
      case 3:
        if (buttonPress == 1) answerCall(); // accept call
        if (buttonPress == 2) hangupCall(); // reject call
        break;
      case 4:
        if (buttonPress == 1) hangupCall(); // hangup call
        if (buttonPress == 2) hangupCall(); // hangup call
        break;
    }
  }
}

void miscSecondHeartbeat() {
  if (SIM800LStatus == 4) {
    callTime++;
    short int s = callTime % 60;
    short int c = (callTime - s) / 60;
    short int m = c % 60;
    callTimeFormatted[0] = (m / 10) % 10 + '0';
    callTimeFormatted[1] = m % 10 + '0';
    callTimeFormatted[2] = ':';
    callTimeFormatted[3] = (s / 10) % 10 + '0';
    callTimeFormatted[4] = s % 10 + '0';
    callTimeFormatted[5] = '\0';
    digitalWrite(LED_PIN, HIGH);
    delay(15);
    digitalWrite(LED_PIN, LOW);
  }
  if (SIM800LStatus == 3) {
    digitalWrite(LED_PIN, HIGH);
    delay(15);
    digitalWrite(LED_PIN, LOW);
  }
  if (SIM800LStatus == 0 || SIM800LStatus == 1 || SIM800LStatus == 2) {
    callTime = 0;
    callTimeFormatted[0] = '0';
    callTimeFormatted[1] = '0';
    callTimeFormatted[2] = ':';
    callTimeFormatted[3] = '0';
    callTimeFormatted[4] = '0';
    callTimeFormatted[5] = '\0';
    blinkTick++;
    if (blinkTick > 3) {
      digitalWrite(LED_PIN, HIGH);
      delay(15);
      digitalWrite(LED_PIN, LOW);
      blinkTick = 0;
    }
  }
  if (isBattLow && battStatus == 0) {
    forceScreenToSleep();
  }
  secsToSleep++;
  (secsToSleep > SLEEP_PERIOD) ? isSleepActive = true : isSleepActive = false;
}

void resetSleepCounter() {
  secsToSleep = 0;
}
