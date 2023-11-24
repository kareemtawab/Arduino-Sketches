#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ElegantOTA.h>                          // https://github.com/ayushsharma82/ElegantOTA/tree/master تحميل المكتبة من 
#include <TimeLib.h>                             // https://github.com/PaulStoffregen/Time/tree/master تحميل المكتبة من 
#include "Timer.h"                               // https://github.com/JChristensen/Timer/tree/master تحميل المكتبة من 
#include "PrayerTimes.h"                         // https://github.com/asmaklad/Arduino-Prayer-Times/tree/master تحميل المكتبة من 
#include <DMDESP.h>                              // https://github.com/busel7/DMDESP/tree/master تحميل المكتبة من 
#include <fonts/SystemFont5x7.h>
#include <fonts/EMSans5x6.h>
#include <fonts/EMSans6x16.h>

#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1
#define DISPLAY_BRIGHTNESS 50                    // تغيير شدة اضاءة الشاشة من 50 الى 1023
#define timeToSwitchOffWiFi 180                  // تحديد عدد الثواني لايقاف الواي فاي بعد الحصول على الوقت من النت

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);
IPAddress local_IP(192, 168, 1, 53);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 168);  
IPAddress secondaryDNS(8, 8, 8, 8);
char *ssid1 = "ssid1";                           // اسم اول شبكة
char *ssid2 = "ssid2";                           // اسم ثاني شبكة
char *ssid3 = "ssid3";                           // اسم ثالث شبكة
char *pass1 = "pass1";                           // كلمة سر اول شبكة
char *pass2 = "pass2";                           // كلمة سر ثاني شبكة
char *pass3 = "pass3";                           // كلمة سر ثالث شبكة
char *host_name = "host_name";                   // اسم المتحكم على الشبكة

char timeDateForDisplay[10], dateForDisplay[150];
bool gotNTPTime;
String weekdays[] = { "", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int ticker_x = 63;
bool isDSTenabled = false;                       // تترك خاطئة لالغاء التوقيت الصيفي او صحيحة لتفعيل التوقيت الصيفي
float salatLatitude = 30.0444196;                // احداثي خط العرض للمكان
float salatLongitude = 31.2357116;               // احداثي خط الطول للمكان
int salatTimezone = 2;                           // عدد الساعات على توقيت جرينيتش
float fajr_angle = 19.5;                         // زاوية الفجر
float isha_angle = 17.5;                         // زاوية العشاء
CalculationMethod salat_calc_method = Egypt;     // طريقة حساب المواقيت
JuristicMethod asr_method = Shafii;              // المذهب لحساب توقيت العصر
double salatTimes[sizeof(TimeName) / sizeof(char*)];
int salatHours;
int salatMinutes;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", salatTimezone * 3600, 60000);
time_t ntptime;
Timer t;

void getNTPTime() {
  if (!gotNTPTime) {
    if (timeClient.forceUpdate()) {
      if (isDSTenabled) setTime(timeClient.getEpochTime() + 3600);
      if (!isDSTenabled) setTime(timeClient.getEpochTime());
      gotNTPTime = true;
    }
  }
}

void getPrayerTime() {
  if (gotNTPTime) {
    get_prayer_times(year(), month(), day(), salatLatitude, salatLongitude, salatTimezone, salatTimes);
  }
}

void updateTimeDateForDisplay() {
  if (gotNTPTime) {
    sprintf(timeDateForDisplay, "%02d-%02d", day(), month());
    Disp.setFont(EMSans5x6);
    Disp.drawText(35, 8, timeDateForDisplay);

    sprintf(timeDateForDisplay, "%02d     %02d", hourFormat12(), minute());
    Disp.setFont(EMSans6x16);
    Disp.drawText(0, 0, timeDateForDisplay);

    isPM() ? Disp.setPixel(63, 0, true) : Disp.setPixel(63, 0, false);

    if (second() % 2) {
      Disp.setPixel(15, 4, true);
      Disp.setPixel(15, 5, true);
      Disp.setPixel(16, 4, true);
      Disp.setPixel(16, 5, true);
      Disp.setPixel(15, 10, true);
      Disp.setPixel(15, 11, true);
      Disp.setPixel(16, 10, true);
      Disp.setPixel(16, 11, true);
    } else {
      Disp.setPixel(15, 4, false);
      Disp.setPixel(15, 5, false);
      Disp.setPixel(16, 4, false);
      Disp.setPixel(16, 5, false);
      Disp.setPixel(15, 10, false);
      Disp.setPixel(15, 11, false);
      Disp.setPixel(16, 10, false);
      Disp.setPixel(16, 11, false);
    }
  }
}

void updatePrayerForDisplay() {
  if (gotNTPTime) {
    int length = sprintf(dateForDisplay, "<<%s>>  <<Fajr %02d:%02d>>  <<Sunrise %02d:%02d>>  <<Dhuhr %02d:%02d>>  <<Asr %02d:%02d>>  <<Maghrib %02d:%02d>>  <<Isha %02d:%02d>>",
                         weekdays[weekday()],
                         (int)salatTimes[0], (int)((salatTimes[0] - (float)(int)salatTimes[0]) * 60.0f),
                         (int)salatTimes[1], (int)((salatTimes[1] - (float)(int)salatTimes[1]) * 60.0f),
                         (int)salatTimes[2], (int)((salatTimes[2] - (float)(int)salatTimes[2]) * 60.0f),
                         (int)salatTimes[3], (int)((salatTimes[3] - (float)(int)salatTimes[3]) * 60.0f),
                         (int)salatTimes[4], (int)((salatTimes[4] - (float)(int)salatTimes[4]) * 60.0f),
                         (int)salatTimes[6], (int)((salatTimes[6] - (float)(int)salatTimes[6]) * 60.0f));
    Disp.setFont(SystemFont5x7);
    Disp.drawText(ticker_x, 1, dateForDisplay);
    ticker_x < 17 - (length + 1)* 6 ? ticker_x = 63 : ticker_x--;
  }
}

void updateDisplay() {
  updatePrayerForDisplay();
  updateTimeDateForDisplay();
}

void handleRoot() {
  server.send(200, "text/html", "seconds since start: " + String(millis() / 1000));  //Send web page
}

void setup() {
  set_calc_method(Egypt);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(fajr_angle);
  set_isha_angle(isha_angle);

  Disp.setBrightness(DISPLAY_BRIGHTNESS);  //maximum is 1024
  Disp.start();
  Disp.loop();

  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);
  wifiMulti.run();
  WiFi.hostname(host_name);
  while (wifiMulti.run() != WL_CONNECTED) {
    Disp.setFont(SystemFont5x7);
    Disp.drawText(0, 5, "Connecting");
    Disp.loop();
  }

  ElegantOTA.begin(&server);
  server.on("/", handleRoot);
  server.begin();

  timeClient.begin();
  getNTPTime();
  getPrayerTime();

  t.every(5000, getNTPTime);
  t.every(60000, getPrayerTime);
  t.every(50, updateDisplay);
}

void loop() {
  // yield();
  Disp.loop();
  t.update();

  if (millis() / 1000 > timeToSwitchOffWiFi && gotNTPTime) {
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  } else {
    wifiMulti.run();
    server.handleClient();
  }
}
