#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "";
const char* password = "";
String messages = "";  // To store the translated message


void setup(void) {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
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
}

void loop(void) {
  HTTPClient http;
  http.begin("http://192.168.10.22/Morse_Code_Transmission/read.php");
  int httpCode = http.GET();
  if (httpCode > 0) {
    messages = http.getString();
    Serial.println(messages);
  } else {
  }
  http.end();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MESSAGE RECEIVED");
  lcd.setCursor(0, 1);
  lcd.print(messages);
  delay(10000);
}
