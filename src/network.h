// Module interface for connecting to WiFi network and syncing/retrieving current time
// returns void / String
#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>

// connectToWiFi is used to connect to a WiFi network using the  SSID and password provided in main 
// connectToWifi will block until a connection is established, printing status messages to the serial monitor
// if connection fails due to incorrect credentials or timeout, an error message is printed and the function returns to the main loop

void connectToWiFi(const char* ssid, const char* password);

// getTimestamp returns the current local date and time as a formatted string
// returns as YYYY-MM-DD HH:MM:SS ("2026-09-11 14:30:00")
// requires connectToWiFi to have run first, so the clock has been synced via NTP
String getTimestamp();


#endif // NETWORK_H 