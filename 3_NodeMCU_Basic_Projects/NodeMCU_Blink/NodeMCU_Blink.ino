// Define the built-in LED pin (GPIO 2 for ESP32 DEVKITV1)
#define LED_PIN D2

void setup() {
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);   // Turn the LED on
  delay(1000);                   // Wait for 1 second
  digitalWrite(LED_PIN, LOW);    // Turn the LED off
  delay(1000);                   // Wait for 1 second
}