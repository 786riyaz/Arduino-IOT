#include <WiFi.h>

// WiFi credentials (replace with your network details)
const char* ssid = "Streebo";       // Replace with your WiFi SSID
const char* password = "Arbaz@2000"; // Replace with your WiFi password

// Define the built-in LED pin (GPIO 2 for ESP32 DEVKITV1)
#define LED_PIN 2

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Initialize LED pin as output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially
  
  // Start WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Connection successful
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Blink LED to indicate successful connection
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void loop() {
  // Keep the connection alive (no action needed in loop for basic connection)
  // Add your code here for further functionality
}