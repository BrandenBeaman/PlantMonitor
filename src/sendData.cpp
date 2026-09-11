// Module implementation for sending data to the server
// returns void
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>

#include "sendData.h"

void sendData(const char* serverURL, String jsonData) {
    
    //before trying to send data, check if Wifi is connected, return is it isnt
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, exiting sendData function");
        return;
    }
    
    // WiFiClientSecyre handles encrypted connection
    // HTTPClient handles request/respone mechs
    WiFiClientSecure client;
    HTTPClient HTTP;

    // skip certificate validation for simplicity... Add certificate verification after CERT is provided from dipesh 
    client.setInsecure();
    
    // start connection to server and prepare URL
    HTTP.begin(client, serverURL);

    // let the server know Content-Type is JSON
    HTTP.addHeader("Content-Type", "application/json");
    
    // send HTTP POST request
    // Store the respose code to track if submission was sucsessful
    int httpResponseCode = HTTP.POST(jsonData);
    //print JSON for testing
    Serial.println(HTTP.getString());
    

    // interprate responde code from HTTP
    if (httpResponseCode <= 299 && httpResponseCode >= 200) {
        Serial.println("POST SUCSESS: ");
    }
    else if (httpResponseCode <= 499 && httpResponseCode >= 400) {
        Serial.println("POST FAILED: DATA SIDE");
    }
     else if (httpResponseCode <= 599 && httpResponseCode >= 500) {
        Serial.println("POST FAILED: SERVER SIDE");
    }
    else if (httpResponseCode < 0) {
        Serial.print("POST FAILED: CONNECTION ERROR");
        Serial.println(HTTP.errorToString(httpResponseCode));
    }
    else {
        Serial.print("POST: Unexpected response code: ");
        Serial.println(httpResponseCode);
    }
    
    // close the connection to the server
    HTTP.end();

}