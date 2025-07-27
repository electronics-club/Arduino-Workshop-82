// SPI Master Example: Send command to SPI Slave on button press
// This sketch acts as the SPI Master.
// When a button is pressed, it sends the command 0x01 to the Slave.
// The Master waits for an ACK (0xAA) response from the Slave and prints the result.

// Include SPI library for SPI communication
#include <SPI.h>

// Define the pin connected to the pushbutton
const int buttonPin = 2;

// Define the Slave Select (SS) pin to enable/disable communication with the Slave
const int ssPin = 10;

// Variable to keep track of the previous button state for edge detection
bool lastButtonState = HIGH;

void setup() {
  // Configure button pin as input with internal pull-up resistor enabled
  pinMode(buttonPin, INPUT_PULLUP);

  // Configure SS pin as output
  pinMode(ssPin, OUTPUT);

  // Initially set SS HIGH to deselect the Slave
  digitalWrite(ssPin, HIGH);

  // Start serial communication for debugging at 9600 baud
  Serial.begin(9600);

  // Initialize SPI bus as Master
  SPI.begin();

  Serial.println("Master ready. Press the button to send command...");
}

void loop() {
  // Read current state of the button (HIGH when unpressed, LOW when pressed)
  bool buttonState = digitalRead(buttonPin);

  // Detect falling edge: button was HIGH before and now is LOW (button press event)
  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("\n[MASTER] Button pressed.");

    // Pull SS low to select the Slave and begin communication
    digitalWrite(ssPin, LOW);

    Serial.println("[MASTER] Sending command: 0x01");

    // Send command byte 0x01 to Slave and simultaneously read response byte
    byte response = SPI.transfer(0x01);

    // Set SS HIGH to end communication with Slave
    digitalWrite(ssPin, HIGH);

    // Check the response received from Slave
    if (response == 0xAA) {
      Serial.println("[MASTER] ACK received! Slave toggled the LED.");
    } else {
      Serial.print("[MASTER] Unexpected response: 0x");
      Serial.println(response, HEX);
    }

    // Small delay to avoid multiple triggers from button bounce
    delay(200);
  }

  // Update the last button state for next loop iteration
  lastButtonState = buttonState;
}
