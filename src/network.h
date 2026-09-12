// Module interface for connecting to WiFi network
// returns void
#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>

// connectToWiFi is used to connect to a WiFi network using the  SSID and password provided in main 
// connectToWifi will block until a connection is established, printing status messages to the serial monitor
// if connection fails due to incorrect credentials or timeout, an error message is printed and the function returns to the main loop

void connectToWiFi(const char* ssid, const char* password);




#endif // NETWORK_H 