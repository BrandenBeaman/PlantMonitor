// Module interface to turn on/off mister based on humidity readings
// returns void
#ifndef UPDATE_MISTER_H
#define UPDATE_MISTER_H

#include <Arduino.h>

// updateMister is used to control when the mister is turned on and off based on humidity readings from the DHT11 sensor
// Mister module will be activated when humidity drops below HUMIDITY_LOW and will be deactivated when humidity rises above HUMIDITY_HIGH
// Alternatilvly, the mister will be deactivated if it has been running for more than MAX_MIST_RUNTIME milliseconds

// extern pin assignment for the mister relay
const int MISTER_PIN = 5;

// flag for when mister is on
extern bool misterOn;
extern bool misterTimeOut;


void updateMister(float humidity);

#endif // UPDATE_MISTER_H