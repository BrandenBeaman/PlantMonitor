
/*
-------------------------------------------------------------------------------------------------------------------------------------------------
Branden Beaman 9/5/2026
main program for Plant Monitor
-------------------------------------------------------------------------------------------------------------------------------------------------
reads from DHT11, BH1750, and Soil Moisture sensor v1.2, controls mister based on humidity threshold
data is sent to a server via WiFi using JSON format, and the ESP32 goes into deep sleep for 30 minutes before waking up and repeating the process
-------------------------------------------------------------------------------------------------------------------------------------------------
Connections
 BH1750:
  SCL to GPIO 22
  SDA to GPIO 21
  GND to GND
  VCC to 3.3v

 DHT11:
  returns 40 bit reading - Humidity int(8bits) + Humidity dec(8bits) + Temp int(8bits) + Temp dec(8bits) + checkSum(8bits)
  VCC  (pin 1)  to 3.3v
  DATA (pin 2) to GPIO 14
  NC   (pin 3) to -
  GND  (pin 4) to GND

 Soil Mositure v1.2:
  GND  to GND
  VCC  to 3.3v
  Aout to GPIO3 5

  Mister:
  POWER - V to Boost Converter VIN+ and GND to Boost Converter VOUT+ 
  PROGRAM: V to VCC, S(Active LOW) to GPIO 5, GND to GND

  Boost Converter:
  VIN+ to LiPo Battery VCC
  VIN- to LiPo Battery GND

  LiPo Battery:
  VCC to Boost Converter VIN+
  GND to Boost Converter VIN-
  -------------------------------------------------------------------------------------------------------------------------------------------------
*/

/* 
DHT:              choose DHT and pull readings from the sensor 
BH1750:           light sensor readings wire library for I2C communication with the BH1750 sensor
Wifi:             ESP32 WiFi connectivity 
Json:             formatting data to send to the server
HTTPClient:       sending data to the server
WiFiClientSecure: secure connection to the server
time              creating timestamp
*/
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h> 
#include <WiFiClientSecure.h> 
#include <time.h>
/*
waterStatus:  soil moisture status
updateMister: mister control
network:      WiFi connection
buildJson:    formatting data to send to the server
sendData:     sending JSON to server
credentials:  stores wifi credentials
*/
#include "waterStatus.h"
#include "updateMister.h"
#include "network.h"
#include "buildJson.h"
#include "sendData.h"
#include "credentials.h"

// DHT sensor type
#define DHTTYPE DHT11

// Pin assignments as per the connections above
const int SOIL_PIN = 35;
const int SDA_PIN  = 21;
const int SCL_PIN  = 22;
const int DHT_PIN  = 14;

 
// const char* serverURL ( get from dipesh)

// Variables to hold sensor readings, to be sent to server in JSON format
float humidity;
float tempCelsius;
float tempFehrenheit;
float lux;

int rawValue;
int readingAttempts = 0;


//Object lightSensor of class BH1750 from BH1750 library 
BH1750 lightSensor;

//Object dht of class DHT from DHT library, passing in the data pin and the type of DHT
DHT dht(DHT_PIN, DHTTYPE);

//string to display soil moisture status
String status;

//string to hold timestamp
String timeStamp;

//strig to hold the JSON formatted data to be sent to the server
String jsonData;

void setup() {
  /* Setup():
    - Initialize serial Monitor at 115200 baud rate for debugging and monitoring
    - Initialize Wifi connection using provided SSID and password
    - Initialize I2C bus for BH1750 light sensor
    - Start DHT11 sensor
    - Start BH1750 light sensor
    - Set MISTER_PIN as OUTPUT for controlling the mister, starts HIGH (off)
  */
  Serial.begin(115200);

  //credentials are stored in credentials.h
  connectToWiFi(ssid, password);

  Wire.begin(SDA_PIN, SCL_PIN);

  dht.begin();
  Serial.println("Temp and Humidity Sensor Begin"); 

  lightSensor.begin();
  Serial.println("Light Sensor Begin");

  pinMode(MISTER_PIN, OUTPUT);
  digitalWrite(MISTER_PIN, HIGH);
}// setup

void loop() {
  // Wait 2 seconds between DHT11 readings; readings take approximately 250 ms
  delay(2000);

  // Read humidity and temperature from the DHT11 sensor
  humidity = dht.readHumidity();
  tempCelsius = dht.readTemperature();


  // Check for failed readings and retry up to 3 times
  if (isnan(humidity) || isnan(tempCelsius)) {
    Serial.println("DHT11 failed to read from sensor, trying again");
    readingAttempts = readingAttempts + 1;

    if (readingAttempts >= 3) {
      Serial.println("DHT11 failed to read from sensor 3 times, aborting..");
      readingAttempts = 0;
      // Go back to deep sleep here
    }
    return;
  }
  else {
    readingAttempts = 0;
    tempFehrenheit = (tempCelsius * 1.8) + 32;
  }


  // Read the light level in lux from the BH1750 sensor
  lux = lightSensor.readLightLevel();


  // Read the raw soil moisture value and determine its status
  rawValue = analogRead(SOIL_PIN);
  status = updateWaterStatus(rawValue);


  // Check whether the mister needs to be turned on based on the humidity threshold
  updateMister(humidity);

  
  // build time stamp tp send with data
  timeStamp = getTimestamp();


  //build JSON body and store it for transportation
  jsonData = buildJsonData(humidity, tempFehrenheit, lux, status, timeStamp);
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Temp: ");
  Serial.print(tempFehrenheit);
  Serial.print(" F\t");

  Serial.print("Lux: ");
  Serial.print(lux);
  Serial.print(" lx\t");
  
  Serial.print("Soil Status: ");
  Serial.print(status);
  Serial.print("\t");

  Serial.print("Mister State: ");
  Serial.println(misterOn);
}//loop
 
