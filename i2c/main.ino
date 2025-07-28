// Include necessary libraries
#include <Wire.h>                  // Handles I2C communication
#include <Adafruit_LiquidCrystal.h> // Adafruit library for I2C LCD

// Create two LCD objects with different I2C addresses
// These addresses must match the actual hardware settings (check with I2C scanner)
Adafruit_LiquidCrystal lcd1(0x20);  // First LCD at address 0x20
Adafruit_LiquidCrystal lcd2(0x21);  // Second LCD at address 0x21

void setup() {
  // Initialize the first LCD with 16 columns and 2 rows
  lcd1.begin(16, 2);           // Set dimensions
  lcd1.setBacklight(1);        // Turn on backlight
  lcd1.setCursor(0, 0);        // Move cursor to top-left corner
  lcd1.print("LCD 1: Hello!"); // Print message on the first row

  // Initialize the second LCD with same dimensions
  lcd2.begin(16, 2);
  lcd2.setBacklight(1);
  lcd2.setCursor(0, 0);
  lcd2.print("LCD 2: World!"); // Print message on the first row
}

void loop() {
  // Wait for 1 second between updates
  delay(1000);

  // Update second row of LCD 1 with elapsed time in seconds
  lcd1.setCursor(0, 1);                // Move cursor to start of second row
  lcd1.print("Time: ");                // Print label
  lcd1.print(millis() / 1000);         // Print number of seconds since program started

  // Update second row of LCD 2 with the same elapsed time
  lcd2.setCursor(0, 1);
  lcd2.print("Millis: ");
  lcd2.print(millis() / 1000);         // Also showing seconds here
}
