#include "time.h"
#include <TimeLib.h>
#include <string>
#include <Wire.h>
#include <DS3232RTC.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// -------------------- Wi-Fi / NTP --------------------
const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASS";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


DS3232RTC rtc;
#define WAKE_PIN 33 // connected to DS3231 INT/SQW


// -------------------- Wi-Fi NTP --------------------
bool fetchNTPandSetRTC(tmElements_t &now) {
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

    //Set the RTC
    rtc.set(makeTime(now));

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

void loop()
{

}

void setup()
{
    Serial.begin(115200);

    // Start I2C for DS3231 on GPIO 25 (SDA) and GPIO 26 (SCL)
    Wire.begin(25, 26);
    pinMode(WAKE_PIN, INPUT_PULLUP);
    rtc.begin();

    tmElements_t tmNow;
    rtc.read(tmNow); // try to read the time

    if (tmNow.Year == 0) {
        Serial.println("RTC not found or not responding!");
        while (1);
    }

    if (rtcLostPower()) {
    Serial.println("RTC lost power");
    }
    

    bool success = fetchNTPandSetRTC(tmNow);
    if (success){
        Serial.println("Set RTC via NTP!");
    }
    else{
        Serial.println("Failed!");
        return;
    }
    
    clearRTCLostPowerFlag();

    rtc.read(tmNow);

    Serial.print("Date: ");
    Serial.print(tmNow.Year+1970);
    Serial.print(" ");
    Serial.print(tmNow.Month);
    Serial.print(" ");
    Serial.println(tmNow.Day);


    Serial.println("Done");

}

