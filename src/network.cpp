// Module implamentation for connecting to WiFi network  and syncing/retrieving current time
// returns void / string 
#include <Arduino.h>
#include "network.h"
#include <WiFi.h>
#include <time.h>


// Eastern Time zone offsets (Rochester, NY)
// gmtOffsetSec: standard time offset from UTC (EST = UTC-5, so -5 * 3600 seconds)
// daylightOffsetSec: additional offset added during daylight saving time (EDT adds 1 hour = 3600 seconds)
const long gmtOffsetSec = -5 * 3600;
const int daylightOffsetSec = 3600;


// NTP server used to get accurate current time once online
const char* ntpServer = "pool.ntp.org";



void connectToWiFi(const char* ssid, const char* password) {

    
    //Print connection message to serial monitor and attempt to connect to WiFi network
    Serial.print("Attempting to connect to WiFi network: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.print(".");
   
        // if connection fails due to incorrect credentials or timeout, print error message and return to main loop
        if(WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Connection failed, check  SSID and password.");
            return;
        }
    }
    Serial.print("Successfully connected to: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());


    
    // sync ESP32's internal clock with NTP server, now that WiFi is connected
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
    Serial.println("Syncing time with NTP server...");

    // wait until the time has actually been set before continuing
    struct tm timeInfo;
    int syncAttempts = 0;
    while (!getLocalTime(&timeInfo) && syncAttempts < 10) {
        Serial.println("Waiting for NTP time sync...");
        delay(500);
        syncAttempts++;
    }

    if (syncAttempts >= 10) {
        Serial.println("NTP time sync failed after multiple attempts");
    } else {
        Serial.println("Time synced successfully");
    }
}

String getTimestamp() {
    // struct tm holds the broken-down local time (year, month, day, hour, minute, second)
    struct tm timeInfo;

    // getLocalTime fills timeInfo using the synced clock
    // returns false if time hasnt been set yet
    if (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to obtain time");
        return "0000-00-00 00:00:00"; // value if time sync hasnt completed
    }

    // buffer to hold the formatted timestamp string
    char buffer[20];

    // format timeInfo into "YYYY-MM-DD HH:MM:SS" using strftime
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

    return String(buffer);
}