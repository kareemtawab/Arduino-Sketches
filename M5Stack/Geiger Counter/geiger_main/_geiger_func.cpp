#include "def_data.c"
#include "_geiger_func.h"
#include "_audio_func.h"
#include "_eeprom_func.h"
#include "_batt_led_vib_func.h"
#include "_eeprom_func.h"
#include <Arduino.h>

unsigned long previousMillis_cpmCalculation;  // Time measurement
unsigned long previousMillis_alarmSound;      // Time measurement
unsigned long previousMillis_yPlots;          // Time measurement
unsigned long previousMillis_vibrate;         // Time measurement
extern int cpm_per_uSv_per_h;
bool gm_pulse_detected;
float graphYPlotPointsOLD[60];
float graphYPlotPoints[60];
float graphSMAYPlotPointsOLD[60];
float graphSMAYPlotPoints[60];
int gm_pulses = 0;      //variable containing the number of GM Tube events withinthe LOGtime
int gm_pulses_arr[10];  // array for storing the measured amounts of impulses in10 consecutive 1 second periods
int avg_cpm = 0;        //variable containing the floating average number ofcounts over a fixed moving windo period
float usv_h, avg_usv_h, mrem, mr;
int t = 0;
int yPlotAddress = 59;

void init_geiger(void) {
  for (int x = 0; x < 10; x++) {  //put all data in the Array gm_pulses_arr to 0 (Array positionsrun from 0 to 10;
    gm_pulses_arr[x] = 0;         //10 positions covering a period of 10 seconds
  }
  pinMode(GEIGER_TUBE_PIN, INPUT);
  // interrupts();
  attachInterrupt(GEIGER_TUBE_PIN, geiger_ISR, FALLING);  //define external interrupts
}

void IRAM_ATTR geiger_ISR(void) {
  gm_pulses++;
  gm_pulse_detected = true;
}

void geiger_update(void) {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_cpmCalculation > GM_LOG_PERIOD) {
    previousMillis_cpmCalculation = currentMillis;
    gm_pulses_arr[t] = gm_pulses;
    int TenSecCPM = 0;
    for (int y = 0; y < 10; y++) {               //add all data in the Array gm_pulses_arr together
      TenSecCPM = TenSecCPM + gm_pulses_arr[y];  //and calculate the rolling average CPM over a 10 secondperiod
    }
    avg_cpm = 6 * TenSecCPM;
    t++;
    if (t > 9) {
      t = 0;
    }
    usv_h = (avg_cpm / (float)eeprom_get_cpm_per_uSv_per_h());
    gm_pulses = 0;
  }

  if (currentMillis - previousMillis_yPlots > Y_PLOTS_UPDATE_PERIOD) {
    previousMillis_yPlots = currentMillis;
    memcpy(&graphYPlotPointsOLD, &graphYPlotPoints, sizeof(graphYPlotPointsOLD));
    memcpy(&graphSMAYPlotPointsOLD, &graphSMAYPlotPoints, sizeof(graphSMAYPlotPointsOLD));
    for (int i = 0; i <= 59; i++) {
      if (i == 0) {
        continue;
      } else {
        graphYPlotPoints[i - 1] = graphYPlotPoints[i];
        graphSMAYPlotPoints[i - 1] = graphSMAYPlotPoints[i];
      }
    }
    graphYPlotPoints[59] = geiger_getSieverts();
    graphSMAYPlotPoints[59] = geiger_getAvgSieverts();
  }

  if (currentMillis - previousMillis_vibrate > VIBRATE_UPDATE_PERIOD) {
    previousMillis_vibrate = currentMillis;
    if (geiger_getDangerLevel() > 1) {
      if (eeprom_get_vibrate()) vibrate(200);
    }
  }
}

int geiger_getCPM(void) {
  // return random(99, 9999);
  if (avg_cpm > 9999) {
    return -1;
  } else {
    return avg_cpm;
  }
}

float geiger_getSieverts(void) {
  // return float(random(45, 60)) / 100.0f;
  if (usv_h > 99.99) {
    return -1;
  } else {
    return usv_h;
  }
}


float geiger_getAvgSieverts(void) {
  float sum = 0.0f;  // sum will be larger than an item, long for safety.
  for (int i = 0; i < 60; i++) sum += graphYPlotPoints[i];
  avg_usv_h = sum / 60.0f;  // average will be fractional, so float may be appropriate.
  if (avg_usv_h > 99.99) {
    return -1;
  } else {
    return avg_usv_h;
  }
}

float geiger_getRems(void) {
  mrem = geiger_getSieverts() * 0.10f;
  if (mrem > 9.9) {
    return -1;
  } else {
    return mrem;
  }
}

float geiger_getRoentgens(void) {
  mr = geiger_getSieverts() * 10.0f;
  if (mr > 999.9) {
    return -1;
  } else {
    return mr;
  }
}

int geiger_getDangerLevel(void) {
  if (geiger_getSieverts() >= 0 && geiger_getSieverts() < RADIATION_LEVEL_0) {
    return 0;
  } else if (geiger_getSieverts() >= RADIATION_LEVEL_0 && geiger_getSieverts() < RADIATION_LEVEL_1) {
    return 1;
  } else if (geiger_getSieverts() >= RADIATION_LEVEL_1 && geiger_getSieverts() < RADIATION_LEVEL_2) {
    return 2;
  } else if (geiger_getSieverts() >= RADIATION_LEVEL_2 && geiger_getSieverts() < RADIATION_LEVEL_3) {
    return 3;
  } else if (geiger_getSieverts() >= RADIATION_LEVEL_3 && geiger_getSieverts() < RADIATION_LEVEL_4) {
    return 4;
  } else if (geiger_getSieverts() >= RADIATION_LEVEL_4) {
    return 5;
  } else {
    return 0;
  }
}