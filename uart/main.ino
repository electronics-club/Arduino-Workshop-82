#include <SoftwareSerial.h>

// Pins
const int buttonPin = 2;
const int ledPin = 8;
const int buzzerPin = 9;
const int rxPin = 10;
const int txPin = 11;

SoftwareSerial softSerial(rxPin, txPin); // RX, TX

// Timing thresholds (ms)
const int dotThreshold = 200;
const int dashThreshold = 500;
const int letterGap = 900;
const int wordGap = 1200;
const int debounceDelay = 50;

bool lastButtonState = HIGH;
unsigned long pressStartTime = 0;
unsigned long releaseTime = 0;
unsigned long lastDebounceTime = 0;

enum State { IDLE, PRESSED, RELEASED };
State buttonState = IDLE;

String currentMorseLetter = "";
String receivedMorseBuffer = "";

struct MorseMap {
  const char* code;
  char letter;
};

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

char decodeMorse(String morse) {
  for (MorseMap entry : morseTable) {
    if (morse.equals(entry.code)) {
      return entry.letter;
    }
  }
  return '?';
}

// Blink LED only (for sending)
void blinkLED(int duration) {
  digitalWrite(ledPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  delay(100);
}

// Blink LED + buzzer (for receiving)
void blinkLEDandBuzzer(int duration) {
  digitalWrite(ledPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  delay(100);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  softSerial.begin(9600);
  Serial.println("Bidirectional Morse Ready");
}

void loop() {
  handleButton();
  handleReceive();
}

// Handle button press and sending Morse
void handleButton() {
  bool currentButtonState = digitalRead(buttonPin);
  unsigned long now = millis();

  // Debounce
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
            blinkLED(200);  // LED only blink for dot
          } else if (pressDuration <= dashThreshold) {
            currentMorseLetter += "-";
            Serial.println("[sent] Morse: -");
            blinkLED(600);  // LED only blink for dash
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
          if (gap >= wordGap) {
            sendAndPrintCurrentLetter();
            softSerial.print(" ");
            Serial.println("[sent] Word gap");
            buttonState = IDLE;
          } else if (gap >= letterGap) {
            sendAndPrintCurrentLetter();
            softSerial.print("/");
            Serial.println("[sent] Letter gap");
            buttonState = IDLE;
          }
        }
        break;
    }
  }

  lastButtonState = currentButtonState;
}

// Send and decode letter
void sendAndPrintCurrentLetter() {
  if (currentMorseLetter.length() > 0) {
    Serial.print("[final sent] Morse: ");
    Serial.println(currentMorseLetter);
    char letter = decodeMorse(currentMorseLetter);
    Serial.print("[decoded]: ");
    Serial.println(letter);
    softSerial.print(currentMorseLetter);
    currentMorseLetter = "";
  }
}

// Handle received Morse input
void handleReceive() {
  while (softSerial.available()) {
    char c = softSerial.read();

    if (c == '.' || c == '-') {
      receivedMorseBuffer += c;
      blinkLEDandBuzzer(c == '.' ? 200 : 600);  // LED + buzzer on receive dot/dash
    } else if (c == '/') {
      if (receivedMorseBuffer.length() > 0) {
        char decoded = decodeMorse(receivedMorseBuffer);
        Serial.print("[recv] Morse: ");
        Serial.println(receivedMorseBuffer);
        Serial.print("[decoded]: ");
        Serial.println(decoded);
        receivedMorseBuffer = "";
      }
      delay(200);
    } else if (c == ' ') {
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
