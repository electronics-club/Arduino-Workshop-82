// SPI Slave Example: Toggle LED on command from SPI Master
// This sketch uses SPI communication where Arduino acts as a Slave.
// When the Slave receives command 0x01 from the Master, it toggles an LED
// and sends an acknowledgment byte (0xAA) back.

// Include SPI library to handle SPI communication
#include <SPI.h>

// Define the pin connected to the LED
const int ledPin = 9;

// Variable to store the byte received via SPI interrupt
volatile byte receivedByte = 0;

void setup() {
  // Set LED pin as output to control the LED
  pinMode(ledPin, OUTPUT);

  // Set MISO pin as output to send data back to the Master
  pinMode(MISO, OUTPUT);

  // Start Serial communication for debugging at 9600 baud
  Serial.begin(9600);

  // Enable SPI in Slave mode by setting SPE bit in SPCR register
  SPCR |= _BV(SPE);

  // Attach SPI interrupt to trigger whenever a byte is received
  SPI.attachInterrupt();

  Serial.println("Slave ready. Waiting for data...");
}

// Interrupt Service Routine (ISR) for SPI Transfer Complete
ISR(SPI_STC_vect) {
  // Read the received byte from SPI Data Register (SPDR)
  receivedByte = SPDR;

  // Check if the received command is 0x01
  if (receivedByte == 0x01) {
    // Toggle the LED state (if ON turn OFF, if OFF turn ON)
    digitalWrite(ledPin, !digitalRead(ledPin));

    // Print debug info to Serial Monitor
    Serial.println("\n[SLAVE] Received command: 0x01");
    Serial.println("[SLAVE] Toggled LED!");

    // Send ACK byte (0xAA) back to Master to confirm command processed
    SPDR = 0xAA;
    Serial.println("[SLAVE] Sent ACK (0xAA) to master.");
  } else {
    // If command is unknown, print error message
    Serial.print("[SLAVE] Unknown command: 0x");
    Serial.println(receivedByte, HEX);

    // Send NACK or zero back to Master
    SPDR = 0x00;
  }
}

void loop() {
  // Empty loop because all SPI communication is handled by interrupt
}
