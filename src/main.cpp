
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
  PROGRAM: V to VCC, S to GPIO 5, GND to GND

  Boost Converter:
  VIN+ to LiPo Battery VCC
  VIN- to LiPo Battery GND

  LiPo Battery:
  VCC to Boost Converter VIN+
  GND to Boost Converter VIN-
  -------------------------------------------------------------------------------------------------------------------------------------------------
*/

// From DHT library, BH1750 library. wire library for I2C for BH1750 SDL and SCL, Wifi library for ESP32, Json library for formatting data to send to server
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// waterStatus.h for soil moisture status,
#include "waterStatus.h"
#include "updateMister.h"

#define DHTTYPE DHT11

const int SOIL_PIN = 35;
const int SDA_PIN  = 21;
const int SCL_PIN  = 22;
const int DHT_PIN  = 14;

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


//string to display status
String status;

void setup() {

  //begin serial monitor
  
  Serial.begin(115200);

  //Initialize the I2C bus, setting SDL-GPIO 21 and SCL-GPIO 22
  Wire.begin(SDA_PIN, SCL_PIN);

  //Start DHT11
  dht.begin();
  Serial.println("Temp and Humidity Sensor Begin"); 

  //Start BH1750
  lightSensor.begin();
  Serial.println("Light Sensor Begin");

  // set MISTER_PIN as OUTPUT mister,  starts low
  pinMode(MISTER_PIN, OUTPUT);
  digitalWrite(MISTER_PIN, LOW);


}// setup

void loop() {
  // 2 sec delay for DHT11, readings from DHT11 take 250 ms
  delay(2000);

  
  //DHT library reads analing humidity value, stored in humidity variable
   humidity = dht.readHumidity();

  //Read temp as Celsius, checks if read failed and convert to Fahrenheit
   tempCelsius = dht.readTemperature();
  
  
   // check if any reads failed and try up to 3 times before giving up and going back to sleep
  if (isnan(humidity) || isnan(tempCelsius)) {
    Serial.println("DHT11 failed to read from sensor, trying again");
    readingAttempts = readingAttempts + 1;
   
  if(readingAttempts >= 3) {
    Serial.println("DHT11 failed to read from sensor 3 times, aborting..");
    readingAttempts = 0;
    //go back to deep sleep here
     } // if(){if}    
     return;
   }//if
   else {
    readingAttempts = 0;
    tempFehrenheit = (tempCelsius * 1.8) + 32;
   }//else

 
  //BH1750 library converts analong reading into lux value, stored in lux variable
   lux = lightSensor.readLightLevel();

  // read RAW mostiure reading
    rawValue = analogRead(SOIL_PIN);
   status = updateWaterStatus(rawValue);

  //check to see if the mister needs to be turned on based on humidity threshold
 // updateMister(humidity);   
     
 /*Rough Lux scale:
   Direct sunlight: ~32,000-100,000 lux
   Overcast daylight: ~1,000-10,000 lux
   A well-lit room: ~100-500 lux
  Dim room/dusk: ~10-50 lux
 */
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
 
