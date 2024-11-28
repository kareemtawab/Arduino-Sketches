#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ElegantOTA.h>  // https://github.com/ayushsharma82/ElegantOTA/tree/master تحميل المكتبة من
#include <TimeLib.h>     // https://github.com/PaulStoffregen/Time/tree/master تحميل المكتبة من
#include "Timer.h"       // https://github.com/JChristensen/Timer/tree/master تحميل المكتبة من

#include "PrayerTimes.h"  // https://github.com/asmaklad/Arduino-Prayer-Times/tree/master تحميل المكتبة من
#include <DMDESP.h>       // https://github.com/busel7/DMDESP/tree/master تحميل المكتبة من
#include <fonts/SystemFont5x7.h>
#include <fonts/EMSans5x6.h>
#include <fonts/EMSans6x16.h>
#include <EEPROM.h>

#define BKG_COLOR "MediumBlue"
#define FRG_COLOR "white"
#define CLOCK_TITLE "Garden WiFi Clock"

#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1
#define DISPLAY_BRIGHTNESS 50     // تغيير شدة اضاءة الشاشة من 50 الى 1023
#define timeToSwitchOffWiFi 3600  // تحديد عدد الثواني لايقاف الواي فاي بعد الحصول على الوقت من النت
#define EEPROM_ADDRESS 2

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);
IPAddress local_IP(192, 168, 1, 53);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 1);
IPAddress secondaryDNS(8, 8, 8, 8);
char *ssid1 = "WAW20_HK1";                             // اسم اول شبكة
char *ssid2 = "WAW20_AE3";                             // اسم ثاني شبكة
char *ssid3 = "WAW20_AE4";                             // اسم ثالث شبكة
char *pass1 = "!219adeltawab";                         // كلمة سر اول شبكة
char *pass2 = "!219adeltawab";                         // كلمة سر ثاني شبكة
char *pass3 = "!219adeltawab";                         // كلمة سر ثالث شبكة
char *host_name = "ae_big_matrix_clock_double_width";  // اسم المتحكم على الشبكة

char timeDateForDisplay[10], dateForDisplay[150];
bool gotNTPTime;
String weekdays[] = { "", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int ticker_x = 63;
bool isDSTenabled = false;                    // تترك خاطئة لالغاء التوقيت الصيفي او صحيحة لتفعيل التوقيت الصيفي
float salatLatitude = 30.0444196;             // احداثي خط العرض للمكان
float salatLongitude = 31.2357116;            // احداثي خط الطول للمكان
float fajr_angle = 19.5;                      // زاوية الفجر
float isha_angle = 17.5;                      // زاوية العشاء
CalculationMethod salat_calc_method = Egypt;  // طريقة حساب المواقيت
JuristicMethod asr_method = Shafii;           // المذهب لحساب توقيت العصر
double salatTimes[sizeof(TimeName) / sizeof(char *)];
int salatHours;
int salatMinutes;
// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
time_t ntptime;
Timer t;

String MAIN_page() {
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>" + String(CLOCK_TITLE) + "</title>\n";
  ptr += "<style>html { font-family: Helvetica; font-color: " + String(FRG_COLOR) + "; display: table; margin: 0px auto; text-align: -webkit-center;}\n";
  ptr += "body{margin-top: 50px;background-color:" + String(BKG_COLOR) + ";} h1 {font-size: 40px;color: " + String(FRG_COLOR) + ";margin-bottom: 0px;} h2 {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 15px;} h3 {font-size: 15px;color: " + String(FRG_COLOR) + ";margin-bottom: 20px;} h4 {font-size: 10px;color: " + String(FRG_COLOR) + ";margin-bottom: 0px;}\n";
  ptr += "p {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 14px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h3>Note: Go to 'http://ip_address/update' to perform OTA update</h3>\n";
  ptr += "<h1>WiFi Clock</h1>\n";
  ptr += "<h2>" + String(CLOCK_TITLE) + "</h2>\n";
  ptr += "<h4></h4>\n";
  ptr += "<h4>Fajr " + (((int)salatTimes[0] < 10) ? "0" + String((int)salatTimes[0]) : String((int)salatTimes[0])) + ":" + (((int)((salatTimes[0] - (float)(int)salatTimes[0]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[0] - (float)(int)salatTimes[0]) * 60.0f)) : String((int)((salatTimes[0] - (float)(int)salatTimes[0]) * 60.0f))) + "</h4>\n";
  ptr += "<h4>Sunrise " + (((int)salatTimes[1] < 10) ? "0" + String((int)salatTimes[1]) : String((int)salatTimes[1])) + ":" + (((int)((salatTimes[1] - (float)(int)salatTimes[1]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[1] - (float)(int)salatTimes[1]) * 60.0f)) : String((int)((salatTimes[1] - (float)(int)salatTimes[1]) * 60.0f))) + "</h4>\n";
  ptr += "<h4>Dhuhr " + (((int)salatTimes[2] < 10) ? "0" + String((int)salatTimes[2]) : String((int)salatTimes[2])) + ":" + (((int)((salatTimes[2] - (float)(int)salatTimes[2]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[2] - (float)(int)salatTimes[2]) * 60.0f)) : String((int)((salatTimes[2] - (float)(int)salatTimes[2]) * 60.0f))) + "</h4>\n";
  ptr += "<h4>Asr " + (((int)salatTimes[3] < 10) ? "0" + String((int)salatTimes[3]) : String((int)salatTimes[3])) + ":" + (((int)((salatTimes[3] - (float)(int)salatTimes[3]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[3] - (float)(int)salatTimes[3]) * 60.0f)) : String((int)((salatTimes[3] - (float)(int)salatTimes[3]) * 60.0f))) + "</h4>\n";
  ptr += "<h4>Maghrib " + (((int)salatTimes[4] < 10) ? "0" + String((int)salatTimes[4]) : String((int)salatTimes[4])) + ":" + (((int)((salatTimes[4] - (float)(int)salatTimes[4]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[4] - (float)(int)salatTimes[4]) * 60.0f)) : String((int)((salatTimes[4] - (float)(int)salatTimes[4]) * 60.0f))) + "</h4>\n";
  ptr += "<h4>Isha " + (((int)salatTimes[6] < 10) ? "0" + String((int)salatTimes[6]) : String((int)salatTimes[6])) + ":" + (((int)((salatTimes[6] - (float)(int)salatTimes[6]) * 60.0f) < 10) ? "0" + String((int)((salatTimes[6] - (float)(int)salatTimes[6]) * 60.0f)) : String((int)((salatTimes[6] - (float)(int)salatTimes[6]) * 60.0f))) + "</h4>\n";
  ptr += "<h4></h4>\n";

  ptr += "<form action=/action_page>\n";
  ptr += "<table>\n";
  ptr += "<tr align=center>\n";
  ptr += "<td align=right style=\"color: " + String(FRG_COLOR) + ";\">Offset (seconds):</td>\n";
  ptr += "<td align=left><input type=text name=offset value=" + String(o) + "></td>\n";
  ptr += "</tr>\n";
  ptr += "</table>\n";
  ptr += "<input type=submit value=\"Save To EEPROM\">\n";
  ptr += "</form>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
String action_page() {
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>" + String(CLOCK_TITLE) + "</title>\n";
  ptr += "<style>html { font-family: Helvetica; font-color: " + String(FRG_COLOR) + "; display: table; margin: 0px auto; text-align: -webkit-center;}\n";
  ptr += "body{margin-top: 50px;background-color:" + String(BKG_COLOR) + ";} h1 {font-size: 40px;color: " + String(FRG_COLOR) + ";margin-bottom: 0px;} h2 {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 15px;} h3 {font-size: 15px;color: " + String(FRG_COLOR) + ";margin-bottom: 20px;}\n";
  ptr += "p {font-size: 20px;color: " + String(FRG_COLOR) + ";margin-bottom: 14px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h3>Note: Go to 'http://ip_address/update' to perform OTA update</h3>\n";
  ptr += "<h1>WiFi Clock</h1>\n";
  ptr += "<h2>" + String(CLOCK_TITLE) + "</h2>\n";

  ptr += "<form action=/reboot>\n";
  ptr += "<table>\n";
  ptr += "<tr align=center>\n";
  ptr += "<td align=right style=\"color: " + String(FRG_COLOR) + ";\">Offset is set to " + String(o) + " seconds </td>\n";
  ptr += "<td align=left><input type=submit value=\"Click here to reboot and apply!\"></td>\n";
  ptr += "</tr>\n";
  ptr += "</table>\n";
  ptr += "</form>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void getNTPTime() {
  if (!gotNTPTime) {
    if (timeClient.forceUpdate()) {
      long o;
      EEPROM.get(EEPROM_ADDRESS, o);
      setTime(timeClient.getEpochTime() + o);
      gotNTPTime = true;
    }
  }
}

void getPrayerTime() {
  if (gotNTPTime) {
    long o;
    EEPROM.get(EEPROM_ADDRESS, o);
    get_prayer_times(year(), month(), day(), salatLatitude, salatLongitude, o / 3600, salatTimes);
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
    ticker_x < 17 - (length + 1) * 6 ? ticker_x = 63 : ticker_x--;
  }
}

void updateDisplay() {
  updatePrayerForDisplay();
  updateTimeDateForDisplay();
}

void handleRoot() {
  server.send(200, "text/html", MAIN_page());  //Send web page
}

void handleForm() {
  String o = server.arg("offset");
  EEPROM.put(EEPROM_ADDRESS, o.toInt());
  EEPROM.commit();
  server.send(200, "text/html", action_page());  //Send web page
}

void handleReboot() {
  server.send(200, "text/html", MAIN_page());  //Send web page
  delay(5000);
  ESP.restart();
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

  EEPROM.begin(512);
  long o;
  EEPROM.get(EEPROM_ADDRESS, o);
  if (o > 14 * 3600 || o < -12 * 3600) {
    // ofs = 0;
    EEPROM.put(EEPROM_ADDRESS, 0);
    EEPROM.commit();
  }

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
  server.on("/action_page", handleForm);
  server.on("/reboot", handleReboot);
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
