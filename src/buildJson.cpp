// Module implementation for building JSON formatted data to send to server
// returns string
#include <ArduinoJson.h>
#include "buildJson.h"


String buildJsonData(float humidity, float tempFehrenheit, float lux, String soilStatus, String timeStamp) {
    
    // creates a JSON document with a capacity of 256 bytes
    JsonDocument doc;

    // add the sensor readings and soil status to the JSON document
    doc["humidity"]        = humidity;
    doc["roomTempF"]       = tempFehrenheit;
    doc["lux"]             = lux;
    doc["soilMoisture"]    = soilStatus;
    doc["timestamp"]       = timeStamp;

    // Convert the JSON document into a formatted string to send to the server
    // example: {"humidity": 45.0, "roomTempF": 72.5, "lux": 300.0, "soilMoisture": "healthyWater", "timestamp": "2026-09-11 14:30:00" }
    String jsonString;
    serializeJson(doc, jsonString);

    // Print the JSON string to monitor for testing
    Serial.print("JSON Data: ");
    Serial.println(jsonString);

    // return the JSON string to be sent to the server
    return jsonString;
}