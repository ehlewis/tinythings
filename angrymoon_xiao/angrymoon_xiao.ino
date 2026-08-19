#include <SPI.h>
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

#define EPAPER_W 250 // 250 for 2.13 inch e-ink
#define EPAPER_H 122 // 122 for 2.13 inch e-ink
#include <MoonImage.h>
#include "images/new_moon_cropped.h"
#include "images/full_moon_cropped.h"
#include "images/waxing_crescent_cropped.h"
#include "images/waning_crescent_cropped.h"
#include "images/waxing_gibbous_cropped.h"
#include "images/waning_gibbous_cropped.h"
#include "images/first_quarter_cropped.h"
#include "images/third_quarter_cropped.h"


BitmapDisplay bitmaps(display);

DS3232RTC rtc;
#define WAKE_PIN  D0 // connected to DS3231 INT/SQW

#define EPD_CS    D1
#define EPD_DC    D2
#define EPD_RST   D9
#define EPD_BUSY  D3
#define EPD_SCK   D8 // epaper clk
#define EPD_MOSI  D10 // epaper din

#define EPD_MISO -1

void clearScreen()
{
    display.firstPage();

    do {
        display.fillScreen(GxEPD_WHITE);
    }
    while (display.nextPage());
}


void draw_text(char* text)
{
  display.firstPage();
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

extern const uint8_t new_moon_cropped_bits[] PROGMEM;
MoonImage new_moon = {
    new_moon_cropped_bits,
    96, 96,    // cropped width & height
    77, 13     // offset to center
};

extern const uint8_t waxing_crescent_cropped_bits[] PROGMEM;
MoonImage waxing_crescent = {
    waxing_crescent_cropped_bits,
    94, 96,
    77, 13
};

extern const uint8_t first_quarter_cropped_bits[] PROGMEM;
MoonImage first_quarter = {
    first_quarter_cropped_bits,
    95, 95,
    77, 13
};

extern const uint8_t waxing_gibbous_cropped_bits[] PROGMEM;
MoonImage waxing_gibbous = {
    waxing_gibbous_cropped_bits,
    95, 95,
    77, 13
};

extern const uint8_t full_moon_cropped_bits[] PROGMEM;
MoonImage full_moon = {
    full_moon_cropped_bits,
    95, 95,
    77, 13
};

extern const uint8_t waning_gibbous_cropped_bits[] PROGMEM;
MoonImage waning_gibbous = {
    waning_gibbous_cropped_bits,
    95, 95,
    77, 13
};

extern const uint8_t third_quarter_cropped_bits[] PROGMEM;
MoonImage third_quarter = {
    third_quarter_cropped_bits,
    95, 95,
    77, 13
};

extern const uint8_t waning_crescent_cropped_bits[] PROGMEM;
MoonImage waning_crescent = {
    waning_crescent_cropped_bits,
    95, 95,
    77, 13
};

void drawMoon(const MoonImage& img)
{
    display.setRotation(1);
    display.firstPage();

    do {
        display.fillScreen(GxEPD_WHITE);

        int16_t x = (display.width() - img.width) / 2;
        int16_t y = (display.height() - img.height) / 2;

        display.drawXBitmap(
            x,
            y,
            img.data,
            img.width,
            img.height,
            GxEPD_BLACK
        );

    }
    while (display.nextPage());
}


void test_all_phases()
{
  drawMoon(waning_crescent);
  draw_text("Waning Crescent");
  delay(2000);

  drawMoon(new_moon);
  draw_text("New Moon");
  delay(2000);

  drawMoon(waxing_crescent);
  draw_text("Waxing Crescent");
  delay(2000);

  drawMoon(waxing_gibbous);
  draw_text("Waxing Gibbous");
  delay(2000);

  drawMoon(first_quarter);
  draw_text("First Quarter");
  delay(2000);

  drawMoon(third_quarter);
  draw_text("Third Quarter");
  delay(2000);

  drawMoon(full_moon);
  draw_text("Full Moon");
  delay(2000);

  drawMoon(waxing_gibbous);
  draw_text("Waning Gibbous");
  delay(2000);
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

  if (p < 0.03 || p > 0.97) drawMoon(new_moon);
  else if (p < 0.22) drawMoon(waxing_crescent);
  else if (p < 0.28) drawMoon(first_quarter);
  else if (p < 0.47) drawMoon(waxing_gibbous);
  else if (p < 0.53) drawMoon(full_moon);
  else if (p < 0.72) drawMoon(waning_gibbous);
  else if (p < 0.78) drawMoon(third_quarter);
  else if (p <= 1) drawMoon(waning_crescent);
  else{
    Serial.print("Invalid moonphase value: ");
    Serial.println(p);
  }
}


bool rtcLostPower() {
    byte status = rtc.readRTC(0x0F);     // DS3231/DS3232 status register
    return bitRead(status, 7);           // bit 7 = OSF (Oscillator Stop Flag)
}

void parseCompileTime(tmElements_t &tm)
{
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";

    char monthStr[4];
    int day, year;
    int hour, minute, second;

    sscanf(__DATE__, "%3s %d %d", monthStr, &day, &year);
    sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

    tm.Day = day;
    tm.Month = (strstr(months, monthStr) - months) / 3 + 1;
    tm.Year = CalendarYrToTm(year);

    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = second;
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
  delay(2000);

  Serial.println();
  Serial.println("======================");
  Serial.println("Boot");
  Serial.println("======================");

  //SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  SPI.begin(D8, -1, D10, D1);
  
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

  // Start I2C for DS3231 on GPIO 25 (SDA) and GPIO 26 (SCL) for esp32-wroom
  // Wire.begin(25, 26);
  
  // Start I2C for DS3231 on GPIO 25 (SDA) and GPIO 26 (SCL) for esp32-xiao
  Wire.begin(D4, D5);

  pinMode(WAKE_PIN, INPUT_PULLUP);
  rtc.begin();

  tmElements_t tmNow;
  rtc.read(tmNow); // try to read the time

  if (tmNow.Year == 0) {
      Serial.println("RTC not found or not responding!");
      return;
  }

  Serial.println("Checking RTC");

  if (rtc.oscStopped()) {
    Serial.println("RTC lost power. Setting time from compile time.");
    tmElements_t tm;
    parseCompileTime(tm);
    rtc.write(tm);
  
    Serial.println(__DATE__);
    Serial.println(__TIME__);
    Serial.println("RTC written.");
  }
  else{
    Serial.println("RTC OK, clock running.");
  }

  Serial.print("Date: ");
  Serial.print(tmNow.Year+1970);
  Serial.print(" ");
  Serial.print(tmNow.Month);
  Serial.print(" ");
  Serial.println(tmNow.Day);

  rtc.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, 0, 0, 0, 0); // clear
  rtc.setAlarm(DS3232RTC::ALM2_MATCH_HOURS, 0, 0, 0, 0);
  rtc.alarmInterrupt(DS3232RTC::ALARM_1, false);
  rtc.alarmInterrupt(DS3232RTC::ALARM_2, false);
  rtc.alarm(DS3232RTC::ALARM_1);
  rtc.alarm(DS3232RTC::ALARM_2);


  float current_moonphase = get_moon_phase_rtc(tmNow);
  

  clearScreen();

  Serial.println("Displaying moon");
  delay(2000); // ensure display finishes
  display_moon_phase(current_moonphase);
  Serial.println("Drew moon");

  Serial.print("Moon phase fraction: ");
  Serial.println(current_moonphase);

  Serial.print("Date: ");
  Serial.print(tmNow.Year+1970);
  Serial.print(" ");
  Serial.print(tmNow.Month);
  Serial.print(" ");
  Serial.println(tmNow.Day);

  setESPAlarms(tmNow);

  delay(5000); // ensure display finishes
  Serial.println("Going to sleep...");
  //esp_deep_sleep_start();
}

