#ifndef TMP36_H
#define TMP36_H

////////////////////////////////////////////////////////////////////////
// TMP36 Class
// This class provides an interface to the TMP36 temperature sensor.
// The pin to read off of board is required, as well as the
// Reference Voltage, since the values will change according to that.
// Call Read to get the temperature in Degrees C.
////////////////////////////////////////////////////////////////////////

#include "Arduino.h"

class TMP36
{
public:
   TMP36 (int pin, float refVoltage);

   // Returns the temperature from the sensor in degrees C.
   float Read ();

   // Changes the degrees C to degrees F
   float CtoF (float tempC);
   
private:
   int _pin;
   float refVolts;
   unsigned long previousRead;
   
   int numReadings = 0;
   float lastAverage = 0.0;
   const int MAX_READINGS = 10;
};

#endif
