// Module implamentation for connecting to WiFi network
// returns void
#include <Arduino.h>
#include "network.h"
#include <WiFi.h>


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
}