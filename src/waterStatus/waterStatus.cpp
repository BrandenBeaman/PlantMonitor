// Module implementation to return state of the soil
// returned as strings
#include <Arduino.h>
#include "waterStatus.h"

// varibles of type waterStatus, using to determine which state where in, defult state is needWater, prevState is set to !state to ensure first reading is new state
waterStatus state = needWater;
waterStatus prevState = healthyWater;



String updateWaterStatus(int rawSoilReading) {
       int m = rawSoilReading;
       prevState = state; // save state before this reading changes it

      switch (state) {
        //when reading is over 1850 state is needwater
        case needWater:
          if (m > 1850) {
            state = needWater;
          }
          else if ((m <= 1850) && (m >= 900))  {
            state = healthyWater;
          }
          else if (m < 900) {
            state = overWater;
          }
        break;

        // if reading is equal to or in between 1850 and 900 state is healthywater
        case healthyWater:
          if (m > 1850) {
            state = needWater;
          }
          else if ((m <= 1850) && (m >= 900))  {
            state = healthyWater;
          }
          else if (m < 900) {
            state = overWater;
          }
        break;
        
        // if reading is under 900 state is underWater
        case overWater:
          if (m > 1850) {
            state = needWater;
          }
          else if ((m <= 1850) && (m >= 900))  {
            state = healthyWater;
          }
          else if (m < 900) {
            state = overWater;
          }
        break;
      }//switch()

    
      //if its a new state eneter the switch and return the state as a string

        switch (state) {
          case needWater:
            return "Needs Water";
            break;
          case healthyWater:
            return "healthy";
            break;
          case overWater:
            return "Over Watered";
            break;
          default: 
             return " "; // default case as a fall back
             break;
        }

    }//updateWaterStatus()