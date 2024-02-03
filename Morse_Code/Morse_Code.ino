#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

const int dotButtonPin = D3; // Connect the dot button to this pin
const int dashButtonPin = D4; // Connect the dash button to this pin
String morseInput = "";
unsigned long lastInputTime = 0;
String translatedMessage = ""; // To store the translated message

// Define a Morse code to alphabet lookup table
const char* morseAlphabet[] = {
  ".-",   // A
  "-...", // B
  "-.-.", // C
  "-..",  // D
  ".",    // E
  "..-.", // F
  "--.",  // G
  "....", // H
  "..",   // I
  ".---", // J
  "-.-",  // K
  ".-..", // L
  "--",   // M
  "-.",   // N
  "---",  // O
  ".--.", // P
  "--.-", // Q
  ".-.",  // R
  "...",  // S
  "-",    // T
  "..-",  // U
  "...-", // V
  ".--",  // W
  "-..-", // X
  "-.--", // Y
  "--..", // Z
};

void setup(void)
{
  Serial.begin(9600);
  pinMode(dotButtonPin, INPUT_PULLUP);
  pinMode(dashButtonPin, INPUT_PULLUP);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("Press the dot button for dots and the dash button for dashes.");
}

void loop(void)
{
  // Check if the dot button is pressed
  if (digitalRead(dotButtonPin) == LOW) {
    morseInput += ".";
    lastInputTime = millis();
    // Serial.println(".");
    delay(100); // Adjust this delay as needed
    while (digitalRead(dotButtonPin) == LOW) {
      delay(10); // Wait for the button to be released
    }
  }

  // Check if the dash button is pressed
  if (digitalRead(dashButtonPin) == LOW) {
    morseInput += "-";
    lastInputTime = millis();
    // Serial.println("-");
    delay(100); // Adjust this delay as needed
    while (digitalRead(dashButtonPin) == LOW) {
      delay(10); // Wait for the button to be released
    }
  }

  // Check if 7 dots were entered to insert a space
  if (morseInput.length() >= 7 && morseInput.endsWith(".......")) {
    morseInput = morseInput.substring(0, morseInput.length() - 7) + " "; // Insert a space
  }

  if (millis() - lastInputTime >= 2000 && morseInput.length() > 0) {
    translateMorseToAlphabet(); // Translate the Morse input
    Serial.println("No input for 2 seconds. Printing the Morse input: " + morseInput);
    Serial.println("message: " + translatedMessage);
    morseInput = ""; // Clear the input after printing
  }

  if (Serial.available() > 0) {
    String message = Serial.readString();
    message.trim();
    Serial.print("Received message: ");
    Serial.println(message);
    // No need to convert to Morse here, as requested
  }
}

void translateMorseToAlphabet() {
  String currentCharacter = "";

  for (int i = 0; i < morseInput.length(); i++) {
    char morseChar = morseInput[i];
    if (morseChar == '.' || morseChar == '-') {
      currentCharacter += morseChar;
    } else if (morseChar == ' ') {
      if (currentCharacter.length() > 0) {
        for (int j = 0; j < 26; j++) {
          if (currentCharacter == morseAlphabet[j]) {
            translatedMessage += char('A' + j);
            break;
          }
        }
      }
      currentCharacter = "";
      translatedMessage += ' '; // Add a space
    }
  }

  // Check for the last Morse code character
  if (currentCharacter.length() > 0) {
    for (int j = 0; j < 26; j++) {
      if (currentCharacter == morseAlphabet[j]) {
        translatedMessage += char('A' + j);
        break;
      }
    }
  }
}
