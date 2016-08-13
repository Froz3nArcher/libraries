#ifndef EEPROMLOG_H
#define EEPROMLOG_H

////////////////////////////////////////////////////////////////////////
// EEPROM Logging class (template)
// This class provides an interface to log data to EEPROM.  The class
// takes care of advancing memory according to the size of the object
// passed in.
////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include <EEPROM.h>

template <class logDataType>

class EEPROMLog
{
public:
   
   static const unsigned int SYNC_SIZE_BYTES = 2;
   
   ////////////////////////////////////////////////////////////////////////
   // Constructor
   // Pass in the sync pattern to identify all entries.
   ////////////////////////////////////////////////////////////////////////
   EEPROMLog (byte pattern [SYNC_SIZE_BYTES]);

   ////////////////////////////////////////////////////////////////////////
   // Write function
   // This function writes the passed-in data item to EEPROM, circling
   // around to the beginning if it's near the end.
   // The EEPROM.update function is used to reduce EEPROM writes.
   ////////////////////////////////////////////////////////////////////////
   int Write (logDataType const&);

   ////////////////////////////////////////////////////////////////////////
   // Read function
   // This function retrieves a value if the sync pattern is found. It only
   // attempts one read, from readLocation.  A smarter implementation could
   // search through memory until it finds one.
   ////////////////////////////////////////////////////////////////////////
   int Read (logDataType & elem);
   
private:
   unsigned long writeCount = 0;
   int logSize = 0;
   
   unsigned int nextLocation = 0;
   unsigned int writeLocation = 0;
   unsigned int readLocation = 0;

   const unsigned int EEPROM_SIZE = 1024;

   byte SYNC_PATTERN [SYNC_SIZE_BYTES];  // will be populated at instantiation
};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
template <class logDataType>
EEPROMLog <logDataType>::EEPROMLog (byte pattern [SYNC_SIZE_BYTES])
{
   logSize = sizeof (logDataType);
   memcpy (SYNC_PATTERN, pattern, SYNC_SIZE_BYTES);
}


////////////////////////////////////////////////////////////////////////
// Write function
// This function writes the passed-in data item to EEPROM, circling
// around to the beginning if it's near the end.
// The EEPROM.update function is used to reduce EEPROM writes.
////////////////////////////////////////////////////////////////////////
template <class logDataType>
int EEPROMLog <logDataType>::Write (logDataType const& elem)
{
   byte * dataBytes;

   dataBytes = (byte *)&elem;

   // Protect against trying to exceed the EEPROM size
   if (nextLocation + logSize + SYNC_SIZE_BYTES > EEPROM_SIZE)
      nextLocation = 0;
      
   // Update telemetry
   writeLocation = nextLocation;

   // Write the sync pattern
   for (int i = 0; i < SYNC_SIZE_BYTES; i++)
   {
      EEPROM.update (nextLocation++, SYNC_PATTERN [i]);
   }

   // Write the data bytes out to EEPROM
   for (int i = 0; i < logSize; i++)
   {
      EEPROM.update (nextLocation++, *dataBytes++);
   }

   writeCount++;
   return writeCount;
}


////////////////////////////////////////////////////////////////////////
// Read function
// This function retrieves a value if the sync pattern is found. It only
// attempts one read, from readLocation.  A smarter implementation could
// search through memory until it finds one.
////////////////////////////////////////////////////////////////////////
template <class logDataType>
int EEPROMLog <logDataType>::Read (logDataType & elem)
{
   byte header [SYNC_SIZE_BYTES] = {0};

   byte * dataBytes = (byte *)&elem;

   int foundEntry = 0;

   // This ony works at the beginning of execution.  If not at the beginning
   // the nextLocation could be anywhere, and returning because we reached
   // the end of the EEPROM means we could return before reading entries
   // earlier in memory.
   if (readLocation + logSize + SYNC_SIZE_BYTES > EEPROM_SIZE)
   {
      readLocation = 0;
      nextLocation = readLocation;
      return foundEntry;
   }
   
   for (int i = 0; i < SYNC_SIZE_BYTES; i++)
   {
      header [i] = EEPROM[readLocation++];
   }

   if (memcmp (header, SYNC_PATTERN, SYNC_SIZE_BYTES) == 0)
   {
      foundEntry = 1;
      for (int i = 0; i < logSize; i++)
      {
         *dataBytes = EEPROM.read (readLocation++);
         dataBytes++;
         nextLocation = readLocation;
      }
   }
   else
   {
      // When we find the end of the existing entries, this is where we
      // want to start writing to the log.
      readLocation = readLocation - SYNC_SIZE_BYTES;
      nextLocation = readLocation;
   }
   
   return foundEntry;
   
}

#endif
