#include <SoftwareSerial.h>

// ----------------------- Pin Definitions -------------------------
const int buttonPin = 2;       // Digital input from button (used to send Morse)
const int ledPin = 8;          // LED output for sending and receiving Morse feedback
const int buzzerPin = 9;       // Buzzer output for received Morse feedback
const int rxPin = 10;          // SoftwareSerial receive pin (Rx)
const int txPin = 11;          // SoftwareSerial transmit pin (Tx)

SoftwareSerial softSerial(rxPin, txPin);  // Create software-based serial port

// ------------------------ Timing Settings ------------------------
const int dotThreshold = 200;      // Press duration ≤ 200ms = dot
const int dashThreshold = 500;     // Press duration ≤ 500ms = dash
const int letterGap = 900;         // Gap ≥ 900ms = new letter
const int wordGap = 1200;          // Gap ≥ 1200ms = new word
const int debounceDelay = 50;      // Delay to prevent false button reading

// ----------------------- State Variables -------------------------
bool lastButtonState = HIGH;       // Tracks last state of the button
unsigned long pressStartTime = 0;  // Timestamp when button was pressed
unsigned long releaseTime = 0;     // Timestamp when button was released
unsigned long lastDebounceTime = 0;// For debouncing input

// Button interaction state machine
enum State { IDLE, PRESSED, RELEASED };
State buttonState = IDLE;

// Buffers for Morse characters
String currentMorseLetter = "";     // For collecting user input
String receivedMorseBuffer = "";    // For collecting incoming characters

// -------------------- Morse Code Lookup Table --------------------
struct MorseMap {
  const char* code;
  char letter;
};

// Complete table for A-Z and 0-9 in Morse
MorseMap morseTable[] = {
  {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'},
  {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'},
  {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'},
  {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
  {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'},
  {"--..", 'Z'},
  {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'},
  {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}
};

// ----------------------------------------------------------------
// Function: decodeMorse
// Description: Convert Morse code string (e.g., ".-") to corresponding letter.
// Returns '?' if pattern doesn't match.
// ----------------------------------------------------------------
char decodeMorse(String morse) {
  for (MorseMap entry : morseTable) {
    if (morse.equals(entry.code)) {
      return entry.letter;
    }
  }
  return '?';  // Unknown code
}

// ----------------------------------------------------------------
// Function: blinkLED
// Description: Blink LED (used while sending Morse signals).
// Only LED is triggered (no buzzer).
// ----------------------------------------------------------------
void blinkLED(int duration) {
  digitalWrite(ledPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  delay(100);  // small pause between elements
}

// ----------------------------------------------------------------
// Function: blinkLEDandBuzzer
// Description: Used when receiving Morse code. LED and buzzer activate.
// ----------------------------------------------------------------
void blinkLEDandBuzzer(int duration) {
  digitalWrite(ledPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  delay(100);
}

// ----------------------------------------------------------------
// Function: setup
// Description: Initializes pins, serial communication, and states.
// ----------------------------------------------------------------
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Using internal pull-up resistor
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);       // For debugging
  softSerial.begin(9600);   // Communication with another Arduino

  Serial.println("Bidirectional Morse Ready");
}

// ----------------------------------------------------------------
// Function: loop
// Description: Continuously checks for button presses and serial input.
// ----------------------------------------------------------------
void loop() {
  handleButton();   // Handle Morse code sending (via button)
  handleReceive();  // Handle Morse code receiving (via serial)
}

// ----------------------------------------------------------------
// Function: handleButton
// Description: Manages state machine for button interaction.
// Detects dot/dash based on press duration and manages timing gaps
// to detect letter and word boundaries.
// ----------------------------------------------------------------
void handleButton() {
  bool currentButtonState = digitalRead(buttonPin);
  unsigned long now = millis();

  // Debounce logic
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = now;
  }

  if ((now - lastDebounceTime) > debounceDelay) {
    bool isPressed = (currentButtonState == LOW);

    switch (buttonState) {
      case IDLE:
        if (isPressed) {
          pressStartTime = now;
          buttonState = PRESSED;
        }
        break;

      case PRESSED:
        if (!isPressed) {
          unsigned long pressDuration = now - pressStartTime;

          if (pressDuration <= dotThreshold) {
            currentMorseLetter += ".";
            Serial.println("[sent] Morse: .");
            blinkLED(200);
          } else if (pressDuration <= dashThreshold) {
            currentMorseLetter += "-";
            Serial.println("[sent] Morse: -");
            blinkLED(600);
          }

          releaseTime = now;
          buttonState = RELEASED;
        }
        break;

      case RELEASED:
        if (isPressed) {
          pressStartTime = now;
          buttonState = PRESSED;
        } else {
          unsigned long gap = now - releaseTime;

          // Handle word gap
          if (gap >= wordGap) {
            sendAndPrintCurrentLetter();
            softSerial.print(" ");  // Send word space
            Serial.println("[sent] Word gap");
            buttonState = IDLE;
          }
          // Handle letter gap
          else if (gap >= letterGap) {
            sendAndPrintCurrentLetter();
            softSerial.print("/"); // Send letter separator
            Serial.println("[sent] Letter gap");
            buttonState = IDLE;
          }
        }
        break;
    }
  }

  lastButtonState = currentButtonState;
}

// ----------------------------------------------------------------
// Function: sendAndPrintCurrentLetter
// Description: Converts current Morse buffer to a letter, prints it,
// sends the raw Morse to the other Arduino, and resets buffer.
// ----------------------------------------------------------------
void sendAndPrintCurrentLetter() {
  if (currentMorseLetter.length() > 0) {
    Serial.print("[final sent] Morse: ");
    Serial.println(currentMorseLetter);

    char letter = decodeMorse(currentMorseLetter);
    Serial.print("[decoded]: ");
    Serial.println(letter);

    softSerial.print(currentMorseLetter); // Send Morse raw
    currentMorseLetter = "";              // Clear buffer
  }
}

// ----------------------------------------------------------------
// Function: handleReceive
// Description: Reads incoming Morse characters from softSerial.
// Handles decoding when a full letter or word is received.
// ----------------------------------------------------------------
void handleReceive() {
  while (softSerial.available()) {
    char c = softSerial.read();

    if (c == '.' || c == '-') {
      receivedMorseBuffer += c;
      blinkLEDandBuzzer(c == '.' ? 200 : 600);
    }
    // End of letter
    else if (c == '/') {
      if (receivedMorseBuffer.length() > 0) {
        char decoded = decodeMorse(receivedMorseBuffer);
        Serial.print("[recv] Morse: ");
        Serial.println(receivedMorseBuffer);
        Serial.print("[decoded]: ");
        Serial.println(decoded);
        receivedMorseBuffer = "";
      }
      delay(200);
    }
    // End of word
    else if (c == ' ') {
      if (receivedMorseBuffer.length() > 0) {
        char decoded = decodeMorse(receivedMorseBuffer);
        Serial.print("[recv] Morse: ");
        Serial.println(receivedMorseBuffer);
        Serial.print("[decoded]: ");
        Serial.println(decoded);
        receivedMorseBuffer = "";
      }
      Serial.println("[recv] Word gap");
      delay(400);
    }
  }
}
