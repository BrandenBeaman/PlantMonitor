//Module interface for sending data to the server
// returns void
#ifndef SEND_DATA_H
#define SEND_DATA_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// sendData is used to send the JSON formatted data to the server using an HTTP POST request
// serverURL is provided in main and is the URL of the server to which the data will be sent
void sendData(const char* serverURL, String jsonData);

#endif // SEND_DATA_H