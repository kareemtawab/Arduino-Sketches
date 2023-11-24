#define GM_LOG_PERIOD 1000 //Logging time in milliseconds (1 second)
unsigned long previousMillis = 0; //variablefor storing the previous time
int gm_pulses = 0; //variable containing the number of GM Tube events withinthe LOGtime
int gm_pulses_arr[10]; // array for storing the measured amounts of impulses in10 consecutive 1 second periods
int avg_cpm = 0; //variable containing the floating average number ofcounts over a fixed moving windo period
float usv_h = 0;
int t = 0;
////////////////////the setup code that follows,will run once after "Power On" or after a RESET///////////////////
void setup() {
  Serial.begin(115200);
  for (int x = 0; x < 10 ; x++) { //put all data in the Array gm_pulses_arr to 0 (Array positionsrun from 0 to 10;
    gm_pulses_arr[x] = 0; //10 positions covering a period of 10 seconds
  }
  pinMode(36, INPUT);
  interrupts();
  attachInterrupt(36, IMPULSE, FALLING); //define external interrupton pin D2/INT0 to start the interupt routine IMPULSE  (green wire)
}
////////////////////////the loop code that follows,will run repeatedly until "Power Off" or a RESET/////////
void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > GM_LOG_PERIOD)
  {
    previousMillis = currentMillis;
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

    usv_h = (avg_cpm / 318.0);
    Serial.print("gm_pulses_arr ");
    Serial.print(t);
    Serial.print(" = ");
    Serial.println(gm_pulses_arr[t]);
    Serial.print("avg_cpm = ");
    Serial.println(avg_cpm);
    Serial.print("usv_h = ");
    Serial.println (usv_h);
    gm_pulses = 0;
  }
}
//////////////////END ofLOOP////////////////////////////////////
/////////////////////////////////Hereafter follows the Function for counting the number of impulses from Geiger Counter kit
void IMPULSE()
{
  gm_pulses++;
}
