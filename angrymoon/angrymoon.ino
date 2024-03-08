

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 1

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>
// !!!!  ============================================================================================ !!!!

#include <GxEPD2_BW.h>

#include <Fonts/FreeMonoBold9pt7b.h>

#include "BitmapDisplay.h"
#include "TextDisplay.h"
#include "DisplayFunctions.h"

// or select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

// for handling alternative SPI pins (ESP32, RP2040) see example GxEPD2_Example.ino

#include "time.h"
#include <WiFi.h>
#include "halfmoon_rotated.h"

#include "images/new_moon.h"
#include "images/full_moon.h"

#include "images/waxing_crescent.h"
#include "images/waning_crescent.h"

#include "images/waxing_gibbous.h"
#include "images/waning_gibbous.h"

#include "images/first_quarter.h"
#include "images/third_quarter.h"


const char* ssid     = "TP-Link_54D8";
const char* password = "ApartmentD26";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


BitmapDisplay bitmaps(display);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  // first update should be full refresh

  // partial refresh mode can be used to full screen,
  // effective if display panel hasFastPartialUpdate
  //helloFullScreenPartialMode(display);
  delay(1000);
  Serial.println("testsing lines");
  display.fillScreen(GxEPD_WHITE);
  delay(2000);

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

  /*
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);
    display.println();
    display.println("HELLO");
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
  }
  while (display.nextPage());
  delay(1000);
  */
  /*
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.setCursor(0, 0);
    display.drawXBitmap(0, 0, halfmoon_rotated_bits, halfmoon_rotated_width, halfmoon_rotated_height, GxEPD_BLACK);
    Serial.println("DRAW BITMAP");
  }
  while (display.nextPage());

  delay(1000);
  
  do
  {
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);
    display.println();
    display.println("Half Moon");
  }
  while (display.nextPage());
  */


  
  /*
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  display_local_time();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  

  */

  /*
  delay(1000);
  display.fillScreen(GxEPD_WHITE);
  display.drawXBitmap(0, 0, halfmoon_bits, halfmoon_width, halfmoon_height, GxEPD_BLACK);
  Serial.println("DRAW BITMAP");
  delay(5000);
  helloArduino(display);
  delay(1000);
  helloEpaper(display);
  delay(1000);
  //helloValue(display, 123.9, 1);
  //delay(1000);
  showFont(display, "FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
  delay(1000);
  //BitmapDisplay(display).drawBitmaps();
  bitmaps.drawBitmaps();
  //return;
  if (display.epd2.hasPartialUpdate)
  {
    showPartialUpdate(display);
    delay(1000);
  } // else // on GDEW0154Z04 only full update available, doesn't look nice
  //drawCornerTest();
  //showBox(16, 16, 48, 32, false);
  //showBox(16, 56, 48, 32, true);
  display.powerOff();
  deepSleepTest(display);
  Serial.println("setup done");
  */
  
  /*
  
  */
}

void loop()
{
}



void display_local_time()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char buffer[80];
  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",&timeinfo);
  std::string str(buffer);
  const char* str_array = str.c_str();
  
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

void draw_waning_crescent()
{
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

void draw_new_moon()
{
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

void draw_waxing_crescent()
{
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
