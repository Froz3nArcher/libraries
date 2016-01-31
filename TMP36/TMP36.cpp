////////////////////////////////////////////////////////////////////////
// TMP36 Class
// This class provides an interface to the TMP36 temperature sensor.
// The pin to read off of board is required, as well as the
// Reference Voltage, since the values will change according to that.
// Call Read to get the temperature in Degrees C.
// Per the Data Sheet, the range is -40C to +125C, with a reading of 0.5 V
// at 0C.  10mV / 1 deg C scale factor
////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "TMP36.h"

// Voltage offset at 0 degrees C
#define VOLTS_OFFSET 0.5

// 10 mV / 1 deg C
#define SCALE_FACTOR 100.0

TMP36::TMP36 (int pin, float refVoltage)
{
   _pin = pin;
   refVolts = refVoltage;
   previousRead = millis ();
}

// Returns the temperature from the sensor in degrees C.
float TMP36::Read ()
{
   float lastTemp = 0.0;
   
   lastTemp = ((((analogRead (_pin) / 1024.0) * refVolts) - VOLTS_OFFSET) * SCALE_FACTOR);
   return lastTemp;
}

float TMP36::CtoF (float tempC)
{
   return (tempC * 9.0 / 5.0) + 32.0;
}
