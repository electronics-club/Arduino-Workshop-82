// Include the DHT11 library by Dhruba Shah
#include <DHT11.h>

// --- Pin Definitions ---
// DHT11 sensor is connected to digital pin 2
const int SENSOR_PIN = 2;

// Button is connected to digital pin 3 (INT1 on UNO/Nano)
// This will trigger a hardware interrupt when pressed
const int BUTTON_PIN = 3;

// Built-in LED on pin 13 (used for visual feedback when button is pressed)
const int LED_PIN = 13;

// Create a DHT11 sensor object and specify its data pin
DHT11 dht11(SENSOR_PIN);

// Declare a volatile boolean flag to communicate between ISR and loop()
// Volatile ensures the compiler doesn't optimize this variable away
volatile bool interruptFlag = false;

void setup() {
  // Initialize serial communication for debugging/output
  Serial.begin(9600);

  // Set LED pin as OUTPUT to control blinking
  pinMode(LED_PIN, OUTPUT);

  // Set the button pin as INPUT with internal pull-up resistor
  // This keeps the pin HIGH when the button is unpressed, and LOW when pressed
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach an interrupt to the button pin
  // digitalPinToInterrupt converts pin number to the correct interrupt number
  // Trigger interrupt on FALLING edge (when button is pressed)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING);
}

void loop() {
  // Variables to store temperature and humidity readings
  int temperature = 0;
  int humidity = 0;

  // Read data from DHT11 sensor and store in temperature & humidity variables
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  // Check if sensor reading was successful
  if (result == 0) {
    // Print temperature and humidity to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    // If there was an error reading the sensor, print the error message
    Serial.println(DHT11::getErrorString(result));
  }

  // Check if the interrupt flag was set by the ISR
  if (interruptFlag) {
    // Reset the flag to false to avoid repeated execution
    interruptFlag = false;

    // Inform via Serial Monitor that button was pressed
    Serial.println("Button pressed! Flickering LED...");

    // Flicker (blink) LED rapidly 10 times as feedback
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_PIN, HIGH);  // Turn LED ON
      delay(150);                   // Wait 150 milliseconds
      digitalWrite(LED_PIN, LOW);   // Turn LED OFF
      delay(150);                   // Wait 150 milliseconds
    }
  }

  // Delay to allow DHT11 sensor's 1 Hz read rate
  delay(1000);
}

// --- Interrupt Service Routine (ISR) ---
// This function is executed immediately when the button is pressed
// Keep ISRs short and non-blocking (avoid Serial, delay, etc.)
void handleInterrupt() {
  // Set the flag so main loop can handle the response
  interruptFlag = true;
}
