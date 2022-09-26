#include "_geiger_func.h"
#include "_audio_func.h"
//#include <M5Core2.h>
#include <M5Unified.h>
#include "_eeprom_func.h"

#define RADIATION_LEVEL_0 0.25
#define RADIATION_LEVEL_1 0.35
#define RADIATION_LEVEL_2 0.45
#define RADIATION_LEVEL_3 0.55
#define RADIATION_LEVEL_4 0.65
#define RADIATION_LEVEL_5 0.75

#define GEIGER_TUBE_PIN 36 //26
#define GM_LOG_PERIOD 500 //Logging time in milliseconds
#define ALARM_SOUND_PERIOD 200
unsigned long previousMillis_cpmCalculation;    // Time measurement
unsigned long previousMillis_alarmSound;        // Time measurement

extern int cpm_per_uSv_per_h_old;
extern int cpm_per_uSv_per_h_diff;
extern int cpm_per_uSv_per_h_new;
extern bool increase_cpm_per_uSv_per_h;
extern bool decrease_cpm_per_uSv_per_h;
extern bool geiger_audio_enabled;
bool gm_pulse_detected;

int gm_pulses = 0; //variable containing the number of GM Tube events withinthe LOGtime
int gm_pulses_arr[10]; // array for storing the measured amounts of impulses in10 consecutive 1 second periods
int avg_cpm = 0; //variable containing the floating average number ofcounts over a fixed moving windo period
float usv_h = 0;
int t = 0;

void init_geiger (void) {
  for (int x = 0; x < 10 ; x++) { //put all data in the Array gm_pulses_arr to 0 (Array positionsrun from 0 to 10;
    gm_pulses_arr[x] = 0; //10 positions covering a period of 10 seconds
  }
  pinMode(GEIGER_TUBE_PIN, INPUT);
  interrupts();
  attachInterrupt(GEIGER_TUBE_PIN, geiger_ISR, FALLING); //define external interrupts
  cpm_per_uSv_per_h_diff = 0;
}

void geiger_update (void) {

  cpm_per_uSv_per_h_new = cpm_per_uSv_per_h_old + cpm_per_uSv_per_h_diff;

  if (increase_cpm_per_uSv_per_h) {
    if (cpm_per_uSv_per_h_new < 999) {
      cpm_per_uSv_per_h_diff++;
    }
    else {
      cpm_per_uSv_per_h_new = 999;
    }
    increase_cpm_per_uSv_per_h = false;
  }

  if (decrease_cpm_per_uSv_per_h) {
    if (cpm_per_uSv_per_h_new > 1) {
      cpm_per_uSv_per_h_diff--;
    }
    else {
      cpm_per_uSv_per_h_new = 1;
    }
    decrease_cpm_per_uSv_per_h = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_cpmCalculation > GM_LOG_PERIOD) {
    previousMillis_cpmCalculation = currentMillis;
    gm_pulses_arr[t] = gm_pulses;
    int TenSecCPM = 0;
    for (int y = 0; y < 10 ; y++) { //add all data in the Array gm_pulses_arr together
      TenSecCPM = TenSecCPM + gm_pulses_arr[y]; //and calculate the rolling average CPM over a 10 secondperiod
    }
    avg_cpm = 6 * TenSecCPM;
    t++ ;
    if (t > 9) {
      t = 0 ;
    }
    usv_h = (avg_cpm / (float)cpm_per_uSv_per_h_old);
    gm_pulses = 0;
  }

  if (currentMillis - previousMillis_alarmSound > ALARM_SOUND_PERIOD) {
    previousMillis_alarmSound = currentMillis;
    if (geiger_getDangerLevel() == 5 && geiger_audio_enabled) {
      audio_warning();
    }
  }
}

int geiger_getCPM(void)  {
  return avg_cpm;
}

float geiger_getSieverts(void)  {
  return usv_h;
}

float geiger_getRems(void)  {
  return usv_h * 0.10;
}

int geiger_getDangerLevel(void)  {
  if (geiger_getSieverts() >= 0 && geiger_getSieverts() < RADIATION_LEVEL_0) {
    return 0;
  }
  if (geiger_getSieverts() >= RADIATION_LEVEL_0 && geiger_getSieverts() < RADIATION_LEVEL_1) {
    return 1;
  }
  if (geiger_getSieverts() >= RADIATION_LEVEL_1 && geiger_getSieverts() < RADIATION_LEVEL_2) {
    return 2;
  }
  if (geiger_getSieverts() >= RADIATION_LEVEL_2 && geiger_getSieverts() < RADIATION_LEVEL_3) {
    return 3;
  }
  if (geiger_getSieverts() >= RADIATION_LEVEL_3 && geiger_getSieverts() < RADIATION_LEVEL_4) {
    return 4;
  }
  if (geiger_getSieverts() >= RADIATION_LEVEL_4) {
    return 5;
  }
}

void geiger_ISR(void) {
  gm_pulses++;
  gm_pulse_detected = true;
}
