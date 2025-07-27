// Include the DHT11 library to easily communicate with the sensor
#include <DHT11.h>

int SENSOR_PIN = 2; // Define the pin where the DHT11 sensor is connected
// Create a DHT11 sensor object and specify the digital pin it is connected to (here, pin 2)
DHT11 dht11(SENSOR_PIN);

void setup() {
    // Begin serial communication so we can see output on the Serial Monitor
    // The value 9600 is the baud rate (speed of communication)
    Serial.begin(9600);
}

void loop() {
    // Variables to store the temperature (in °C) and humidity (in %)
    int temperature = 0;
    int humidity = 0;

    // Try reading data from the DHT11 sensor.
    // The sensor will update the temperature and humidity variables.
    // The result will tell us if the reading was successful or if there was an error.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check if the sensor reading was successful
    if (result == 0) {
        // If result is 0, it means the data was read correctly.
        // Now we print the values to the Serial Monitor.
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // If the result is not 0, it means an error occurred while reading the sensor.
        // This line will print a human-readable error message to help debug.
        Serial.println(DHT11::getErrorString(result));
    }

    // Optional: Add a small delay to avoid reading the sensor too quickly (can be added if needed)
    // delay(1000); // Delay for 1 second
}
