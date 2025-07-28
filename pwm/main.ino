// PWM LED Brightness Control using a Potentiometer
// This program changes the brightness of an LED based on the position of a potentiometer.

// Define the analog input pin connected to the potentiometer
const int potPin = A0;   // Potentiometer connected to analog pin A0

// Define the PWM-capable digital pin connected to the LED
const int ledPin = 3;    // LED connected to digital pin 3 (supports PWM)

void setup() {
  // Runs once at startup
  
  // Set the LED pin as an OUTPUT so it can control the LED
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Runs repeatedly after setup()

  // Read the potentiometer value (0 to 1023)
  int potValue = analogRead(potPin);
  
  // Map the potentiometer value (0-1023) to PWM range (0-255)
  // PWM controls brightness by changing how long LED is ON vs OFF
  int brightness = map(potValue, 0, 1023, 0, 255);
  
  // Send the PWM signal to the LED to set its brightness
  analogWrite(ledPin, brightness);
  
  // Small delay to stabilize the signal and prevent flicker
  delay(10);
}
