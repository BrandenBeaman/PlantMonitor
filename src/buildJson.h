// Module interface for building JSON formatted data to send to server
// returns string 
#ifndef BUILDJSON_H
#define BUILDJSON_H

#include <ArduinoJson.h>


// buildJsonData takes in the sensor readings and soil status and returns a JSON formatted string to be sent to the server
 String buildJsonData(float humidity, float tempFehrenheit, float lux, String soilStatus, String timeStamp);



#endif // BUILDJSON_H