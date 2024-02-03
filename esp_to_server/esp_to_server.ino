#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "";
const char* password = "";
const int sw = D5;
const int dotButtonPin = D3;   // Connect the dot button to this pin
const int dashButtonPin = D4;  // Connect the dash button to this pin
String morseInput = "";
unsigned long lastInputTime = 0;
String messages = "";  // To store the translated message

// Define a Morse code to alphabet lookup table
const char* morseAlphabet[] = {
  ".-",    // A
  "-...",  // B
  "-.-.",  // C
  "-..",   // D
  ".",     // E
  "..-.",  // F
  "--.",   // G
  "....",  // H
  "..",    // I
  ".---",  // J
  "-.-",   // K
  ".-..",  // L
  "--",    // M
  "-.",    // N
  "---",   // O
  ".--.",  // P
  "--.-",  // Q
  ".-.",   // R
  "...",   // S
  "-",     // T
  "..-",   // U
  "...-",  // V
  ".--",   // W
  "-..-",  // X
  "-.--",  // Y
  "--..",  // Z
};

void setup(void) {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(sw, INPUT_PULLUP);
  pinMode(dotButtonPin, INPUT_PULLUP);
  pinMode(dashButtonPin, INPUT_PULLUP);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi");
  lcd.setCursor(0, 1);
  lcd.print("OFFLINE");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi");
  lcd.setCursor(0, 1);
  lcd.print("ONLINE");
  Serial.println("Connected to WiFi");
  Serial.println("Press the dot button for dots and the dash button for dashes.");
}

void loop(void) {
  // Check if the dot button is pressed
  if (digitalRead(dotButtonPin) == LOW) {
    morseInput += ".";
    lastInputTime = millis();
    // Serial.println(".");
    delay(100);  // Adjust this delay as needed
    while (digitalRead(dotButtonPin) == LOW) {
      delay(10);  // Wait for the button to be released
    }
  }

  // Check if the dash button is pressed
  if (digitalRead(dashButtonPin) == LOW) {
    morseInput += "-";
    lastInputTime = millis();
    // Serial.println("-");
    delay(100);  // Adjust this delay as needed
    while (digitalRead(dashButtonPin) == LOW) {
      delay(10);  // Wait for the button to be released
    }
  }

  // Check if 7 dots were entered to insert a space
  if (morseInput.length() >= 7 && morseInput.endsWith(".......")) {
    morseInput = morseInput.substring(0, morseInput.length() - 7) + " ";  // Insert a space
  }

  if (millis() - lastInputTime >= 2000 && morseInput.length() > 0) {
    translateMorseToAlphabet();  // Translate the Morse input
    Serial.println("No input for 2 seconds. Printing the Morse input: " + morseInput);
    Serial.println("message: " + messages);
    morseInput = "";  // Clear the input after printing
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
            messages += char('A' + j);
            break;
          }
        }
      }
      currentCharacter = "";
      messages += ' ';  // Add a space
    }
  }

  // Check for the last Morse code character
  if (currentCharacter.length() > 0) {
    for (int j = 0; j < 26; j++) {
      if (currentCharacter == morseAlphabet[j]) {
        messages += char('A' + j);
        break;
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SECRET MESSAGE");
  lcd.setCursor(0, 1);
  lcd.print(messages);
  if (morseInput.length() >= 5 && morseInput.endsWith("-----")) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SECRET MESSAGE");
    lcd.setCursor(0, 1);
    lcd.print("SENDING....");
    delay(2000);
    server(messages);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SECRET MESSAGE");
    lcd.setCursor(0, 1);
    lcd.print("SENT...!!!");
    delay(2000);
  }
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("SECRET MESSAGE");
  //     lcd.setCursor(0, 1);
  //     lcd.print("SENDING....");
  //     delay(2000);
  //     server(messages);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("SECRET MESSAGE");
  //     lcd.setCursor(0, 1);
  //     lcd.print("SENT...!!!");
  //     delay(2000);
  //
}

void server(String messages) {
  HTTPClient http;
  messages.replace(" ", "%20");
  Serial.println(messages);
  http.begin("http://192.168.10.22/Morse_Code_Transmission/script.php?messages=" + messages);
  http.GET();
  http.end();
}
