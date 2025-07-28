// Include the Wire library to use I2C communication
#include <Wire.h>

void setup() {
  // Start I2C communication
  Wire.begin();

  // Start Serial communication so we can see output on the Serial Monitor
  Serial.begin(9600);
  Serial.println("Scanning for I2C devices...");

  // Loop through all possible I2C addresses (from 1 to 126)
  for (byte address = 1; address < 127; address++) {
    // Begin transmission to a device at this address
    Wire.beginTransmission(address);
    
    // endTransmission() returns 0 if a device responds (acknowledges)
    if (Wire.endTransmission() == 0) {
      // If a device is found, print its address in hexadecimal
      Serial.print("Found device at 0x");
      Serial.println(address, HEX);
      
      // Small delay to prevent flooding the bus
      delay(10);
    }
  }
}

void loop() {
  // Empty loop because the scanning happens only once in setup()
}
