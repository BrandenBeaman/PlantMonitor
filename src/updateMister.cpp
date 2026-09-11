// Module implementation to turn on/off mister based on humidity readings
// returns void
#include <Arduino.h>
#include "updateMister.h"


// maxtime mister is allowed to run continuously after boot up (ms) 
const  unsigned long MAX_MIST_RUNTIME = 100000;   

// Humidity thresholds, want spider plant to be between 40% and 60% humidity
const  float HUMIDITY_LOW  = 40.0;   // mister turns on below this value
const  float HUMIDITY_HIGH = 50.0;  // mister turns off above this value

// flag for when mister is on
       bool misterOn = false;

// track the time the mister has been on, used to turn off after max time has passed
       unsigned long misterStartTime  = 0;


    void updateMister(float humidity) {
   
    // turn off the mister if its been running for more than 10 mins, clear mister state flag, raise max runtime flag
     if((misterOn) &&  ((millis() - misterStartTime) >= MAX_MIST_RUNTIME)) {
       digitalWrite(MISTER_PIN, LOW);

       misterOn = false;

       Serial.println("Mister reached MAX run time");
      }                                               
  
     // when the humidity drops below set threshold and the mister isnt already on, turn it on, also start tracking time and set flag, cLear run time flag 
     if((humidity <= HUMIDITY_LOW) && (!misterOn)) {
         digitalWrite(MISTER_PIN, HIGH);

         misterStartTime = millis();
         misterOn = true;

         Serial.print("Current Humidity: ");
         Serial.println(humidity);
     }

    // when humidity is above the thresthold and the mister is on turn it off and clear flag
     else if((humidity >= HUMIDITY_HIGH) && (misterOn)) {
        digitalWrite(MISTER_PIN, LOW);  

        misterOn = false;
     }
     else {
      // leave mister alone to either hit max time or proper humidity level
     }
   Serial.print("Mister Status: ");
   Serial.println(misterOn);
   Serial.print("Mister Timer (ms): ");
   // only print the mister elasped time when it is actually on, other wise print 0
   Serial.println(misterOn ? millis() - misterStartTime : 0);
}