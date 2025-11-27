// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 1
#include <GxEPD2_BW.h>
#include "GxEPD2_display_selection_new_style.h"

#include <Fonts/FreeMonoBold9pt7b.h>
#include "BitmapDisplay.h"
#include "TextDisplay.h"
#include "DisplayFunctions.h"

#include "time.h"
#include <TimeLib.h>
#include <string>
#include <Wire.h>
#include <DS3232RTC.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "images/new_moon.h"
#include "images/full_moon.h"
#include "images/waxing_crescent.h"
#include "images/waning_crescent.h"
#include "images/waxing_gibbous.h"
#include "images/waning_gibbous.h"
#include "images/first_quarter.h"
#include "images/third_quarter.h"

// -------------------- Wi-Fi / NTP --------------------
const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASS";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


BitmapDisplay bitmaps(display);

DS3232RTC rtc;
#define WAKE_PIN 33 // connected to DS3231 INT/SQW


void draw_text(char* text)
{
  do
  {
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);
    display.println();
    display.println(text);
  }
  while (display.nextPage());
}

void display_current_time()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char buffer[80];
  strftime(buffer,sizeof(buffer),"%m-%d-%Y %H:%M:%S",&timeinfo);
  std::string str(buffer);
  const char* str_array = str.c_str();
  Serial.println(str_array);
  
  Serial.println(str_array);
  do
  {
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);
    display.println(str_array);
  }
  while (display.nextPage());
}

float get_moon_phase_rtc(tmElements_t dt){
    // Returns moon phase fraction (0 = new, 1 = full)
    int year  = dt.Year + 1970;
    int month = dt.Month;
    int day   = dt.Day;

    // Algorithm from John Conway, simple and fast
    if (month < 3) {
        year--;
        month += 12;
    }

    ++month; // March = 3 → 4 (formula adjustment)
    float c = 365.25 * year;
    float e = 30.6 * month;
    float jd = c + e + day - 694039.09; // days since known new moon
    jd /= 29.5305882;                    // divide by lunar cycle
    float phase = jd - floor(jd);            // fractional part = phase
    return phase;
}

void display_moon_phase(float p) {
  /*
    Moon phase float:
    0.0         = New Moon
    0.0 – 0.25  = Waxing Crescent
    0.25        = First Quarter
    0.25–0.5    = Waxing Gibbous
    0.5         = Full Moon
    0.5–0.75    = Waning Gibbous
    0.75        = Third Quarter
    0.75–1.0    = Waning Crescent
  */

  if (p < 0.03 || p > 0.97) draw_new_moon();
  else if (p < 0.22) draw_waxing_crescent();
  else if (p < 0.28) draw_first_quarter();
  else if (p < 0.47) draw_waxing_gibbous();
  else if (p < 0.53) draw_full_moon();
  else if (p < 0.72) draw_waning_gibbous();
  else if (p < 0.78) draw_third_quarter();
  else if (p <= 1) draw_waning_crescent();
  else{
    Serial.print("Invalid moonphase value: ");
    Serial.println(p);
  }
}

void draw_new_moon()
{
  Serial.println("Displaying New Moon");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, new_moon_bits, new_moon_width, new_moon_height, GxEPD_BLACK);
    Serial.println("draw new_moon");
  }
  while (display.nextPage());
}

void draw_full_moon()
{
  Serial.println("Displaying Full Moon");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, full_moon_bits, full_moon_width, full_moon_height, GxEPD_BLACK);
    Serial.println("draw full_moon");
  }
  while (display.nextPage());
}

void draw_waning_crescent()
{
  Serial.println("Displaying Waning Crescent");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, waning_crescent_bits, waning_crescent_width, waning_crescent_height, GxEPD_BLACK);
    Serial.println("draw waning_crescent");

    
  }
  while (display.nextPage());
}

void draw_waxing_crescent()
{
  Serial.println("Displaying Waxing Crescent");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, waxing_crescent_bits, waxing_crescent_width, waxing_crescent_height, GxEPD_BLACK);
    Serial.println("draw waxing_crescent");
  }
  while (display.nextPage());
}

void draw_waxing_gibbous()
{
  Serial.println("Displaying Waxing Gibbous");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, waxing_gibbous_bits, waxing_gibbous_width, waxing_gibbous_height, GxEPD_BLACK);
    Serial.println("draw waxing_gibbous");
  }
  while (display.nextPage());
}

void draw_first_quarter()
{
  Serial.println("Displaying First Quarter");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, first_quarter_bits, first_quarter_width, first_quarter_height, GxEPD_BLACK);
    Serial.println("draw first_quarter");
  }
  while (display.nextPage());
}

void draw_third_quarter()
{
  Serial.println("Displaying Third Quarter");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, third_quarter_bits, third_quarter_width, third_quarter_height, GxEPD_BLACK);
    Serial.println("draw third_quarter");
  }
  while (display.nextPage());
}

void draw_waning_gibbous()
{
  Serial.println("Displaying Waning Gibbous");
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, waning_gibbous_bits, waning_gibbous_width, waning_gibbous_height, GxEPD_BLACK);
    Serial.println("draw waning_gibbous");
  }
  while (display.nextPage());
}

void test_all_phases()
{
  draw_waning_crescent();
  draw_text("Waning Crescent");
  delay(2000);

  draw_new_moon();
  draw_text("New Moon");
  delay(2000);

  draw_waxing_crescent();
  draw_text("Waxing Crescent");
  delay(2000);

  draw_waxing_gibbous();
  draw_text("Waxing Gibbous");
  delay(2000);

  draw_first_quarter();
  draw_text("First Quarter");
  delay(2000);

  draw_third_quarter();
  draw_text("Third Quarter");
  delay(2000);

  draw_full_moon();
  draw_text("Full Moon");
  delay(2000);

  draw_waning_gibbous();
  draw_text("Waning Gibbous");
  delay(2000);
}

// -------------------- Wi-Fi NTP --------------------
bool fetchNTP(tmElements_t &now) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries++ < 20) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi failed");
        return false;
    }

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to get NTP time");
        WiFi.disconnect(true);
        return false;
    }

    now.Year = timeinfo.tm_year + 1900 - 1970; // TimeLib stores years since 1970
    now.Month = timeinfo.tm_mon + 1;
    now.Day = timeinfo.tm_mday;
    now.Hour = timeinfo.tm_hour;
    now.Minute = timeinfo.tm_min;
    now.Second = timeinfo.tm_sec;

    rtc.set(makeTime(now)); // set RTC
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
}

bool rtcLostPower() {
    byte status = rtc.readRTC(0x0F);     // DS3231/DS3232 status register
    return bitRead(status, 7);           // bit 7 = OSF (Oscillator Stop Flag)
}

void clearRTCLostPowerFlag() {
    byte status = rtc.readRTC(0x0F);
    bitClear(status, 7);
    rtc.writeRTC(0x0F, status);
}

void clearAlarms() {
  rtc.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, 0, 0, 0, 0); // clear
  rtc.setAlarm(DS3232RTC::ALM2_MATCH_HOURS, 0, 0, 0, 0);
  rtc.alarmInterrupt(DS3232RTC::ALARM_1, false);
  rtc.alarmInterrupt(DS3232RTC::ALARM_2, false);
}

void setESPAlarms(tmElements_t tmNow) {
  // --------------------- Set Next Alarm ---------------------
  // Example: 7:00 AM next day
  tmElements_t alarmTime = tmNow;
  alarmTime.Hour = 7;
  alarmTime.Minute = 0;
  alarmTime.Second = 0;

  time_t tNow = makeTime(tmNow);
  time_t tAlarm = makeTime(alarmTime);

  if (tAlarm <= tNow) tAlarm += 24 * 3600; // next day if past 7AM

  tmElements_t nextAlarm;
  breakTime(tAlarm, nextAlarm);

  rtc.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, nextAlarm.Second, nextAlarm.Minute, nextAlarm.Hour, 0);
  rtc.alarmInterrupt(DS3232RTC::ALARM_1, true);

  // Wake ESP32 on EXT1 when INT pin goes low
  esp_sleep_enable_ext1_wakeup(1ULL << WAKE_PIN, ESP_EXT1_WAKEUP_ALL_LOW);
}

void loop()
{

}

void setup()
{
  Serial.begin(115200);
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

  Serial.println("Display test text");
  char* test_text = "test";
  draw_text(test_text);

  // Start I2C for DS3231 on GPIO 25 (SDA) and GPIO 26 (SCL)
  Wire.begin(25, 26);

  rtc.begin();

  tmElements_t tmNow;
  rtc.read(tmNow); // try to read the time

  if (tmNow.Year == 0) {
      Serial.println("RTC not found or not responding!");
      while (1);
  }
  setTime(2025, 11, 26, 12, 0, 0);

  if (rtcLostPower()) {
    Serial.println("RTC lost power, fetching NTP...");
    if (!fetchNTP(tmNow)) {
        Serial.println("Failed NTP, setting default time 11/26/2025");
        setTime(2025, 11, 26, 12, 0, 0);
        rtc.set(now());
    }
    clearRTCLostPowerFlag();
  }
  else{
    Serial.println("RTC OK, clock running.");
  }

  Serial.print("Date: ");
  Serial.print(tmNow.Year+1970);
  Serial.print(" ");
  Serial.print(tmNow.Month);
  Serial.print(" ");
  Serial.print(tmNow.Day);

  clearAlarms();

  float current_moonphase = get_moon_phase_rtc(tmNow);
  display_moon_phase(current_moonphase);

  Serial.print("Moon phase fraction: ");
  Serial.println(current_moonphase);

  setESPAlarms(tmNow);

  Serial.println("Going to sleep...");
  delay(100); // ensure display finishes
  esp_deep_sleep_start();
}

