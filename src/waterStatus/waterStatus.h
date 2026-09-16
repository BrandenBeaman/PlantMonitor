// Module interface to return state of the soil
// returned as strings
#ifndef WATER_STATUS_H
#define WATER_STATUS_H

#include <Arduino.h>

// waterStatus takes in the raw ADC reading for the soil moisture sensor and returns a string of the current state of the soil
// current state of the soil (stored in waterStatus state) is determined by the boundaries defined in updateWaterStatus() function
// which are based on the raw ADC reading from the soil moisture sensor
enum waterStatus {
    healthyWater, 
    needWater, 
    overWater
};

String updateWaterStatus(int rawSoilReading);

#endif // WATER_STATUS_H