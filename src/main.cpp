
/*
-------------------------------------------------------------------------------------------------------------------------------------------------
Branden Beaman 9/5/2026
main program for Plant Monitor
-------------------------------------------------------------------------------------------------------------------------------------------------
-reads from DHT11, BH1750, and Soil Moisture sensor v1.2, controls mister based on humidity threshold
-data is sent to a server via WiFi using JSON format, and the ESP32 goes into deep sleep for 30 minutes before waking up and repeating the process
-deep sleep is entered after all the data is sent and the mister is off
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
#include "waterStatus/waterStatus.h"
#include "updateMister/updateMister.h"
#include "network/network.h"
#include "buildJson/buildJson.h"
#include "sendData/sendData.h"
#include "credentials.h"

// DHT sensor type
#define DHTTYPE DHT11

// Pin assignments as per the connections above
const int SOIL_PIN = 35;
const int SDA_PIN  = 21;
const int SCL_PIN  = 22;
const int DHT_PIN  = 14;

// ESP32 ELPASED sleep time (Mins to microseconds) 
 unsigned long sleepTimeMins  = 30;
 unsigned long sleepTimeMicro = sleepTimeMins * 60000000;

// server URL to send data to
const char* serverURL = "https://webhook.site/ed14ddf1-46ec-47dd-ab28-ae349e7b1a1a";


// Variables to hold sensor readings, to be sent to server in JSON format
float humidity;
float tempCelsius;
float tempFehrenheit;
float lux;

int rawValue;

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
    Initialization:
    -Initialize serial Monitor at 115200 baud rate for debugging and monitoring
    -Initialize Wifi connection using provided SSID and password
    -Initialize I2C bus for BH1750 light sensor
    -Start DHT11 sensor
    -Start BH1750 light sensor
    -Set MISTER_PIN as OUTPUT for controlling the mister, starts HIGH (off)
    -Set how long esp will sleep for once esp_deep_sleep_start() is called

    Data Collection:
    -Wait 2 seconds to let DHT11 get acurate readings if reading isnan after 3 attempts, DHT11 ERROR(humidity and temp are sent as 0)
    -Read Humidity and Temp from DHT11 and convert to F
    -Read lux from BH1750
    -Read soil Mostiure reading and determine water Status based on it

    Mister Control, POST and deep sleep:
    -Turn the Mister ON or OFF based on humidity reading, max run time for mister is 2 mins
    -Build a time stamp to send with DATA
    -Build JSON payload
    -Send payload to serverURL
    
  */

//INITIALIZATION --------------------------------------------------------------------------------------------
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

  esp_sleep_enable_timer_wakeup(sleepTimeMicro);

//DATA COLLLECTION ---------------------------------------------------------------------------------------------

  // Wait 2 seconds between DHT11 readings; readings take approximately 250 ms
  delay(2000);

// read DHT11 3 times to check if readings are vaild, if readings are still invalid after 3 trys, send DHT11 ERROR
bool readSuccess = false;
 
 for (int i = 0; i < 3; i++) {
   humidity = dht.readHumidity();
   tempCelsius = dht.readTemperature();

   if (!isnan(humidity) && !isnan(tempCelsius)) {
    Serial.println("DHT11 succsessfully read from Sensors: Attempt ");
    Serial.println(i);
    readSuccess = true;
   }
   else {
    Serial.print("DHT11 failed to read from sensor: Attempt ");
    Serial.println(i);
    delay(500); // brief pause before retrying for sensor
  }
}//for

   if (readSuccess) {
     tempFehrenheit = (tempCelsius * 1.8) + 32;
   }
   else {
    Serial.println("DHT11 failed all 3 attempts, sending HUMIDITY and TEMP as 0");
    humidity = 0;
    tempCelsius = 0;
    tempFehrenheit = 0;
   }
   

  // Read the light level in lux from the BH1750 sensor
  lux = lightSensor.readLightLevel();


  // Read the raw soil moisture value and determine its status
  rawValue = analogRead(SOIL_PIN);
  status = updateWaterStatus(rawValue);

//MISTER CONTROL, POST and DEEP SLEEP ---------------------------------------------------------------------------------------------
 

  // Check whether the mister needs to be turned on based on the humidity threshold
  updateMister(humidity);

  // while the mister is actively running, keep re-checking humidity and calling
  while (misterOn) {
    delay(2000); 

    humidity = dht.readHumidity();

    if (!isnan(humidity)) {
    updateMister(humidity);
    }
    // if a reading fails while running the mister abort
    else {
      humidity = 0;
      updateMister(humidity);
    }

  }

  //PRINTS FOR DEBUG/TESTING
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

  // condition to enter deep sleep
  if (misterTimeOut || !misterOn) { 
    
    // build time stamp tp send with data
    timeStamp = getTimestamp();


    // build JSON body and store it for transportation
    jsonData = buildJsonData(humidity, tempFehrenheit, lux, status, timeStamp);

    // send JSON payload to server
    sendData(serverURL, jsonData);

    esp_deep_sleep_start();
  }
//----------------------------------------------------------------------------------------------------------------

}// setup

void loop() {
  return;
}
 
