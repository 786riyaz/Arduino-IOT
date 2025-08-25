#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Update to LCD address from I2C scanner if different

// Define pins for sensors
const int flameSensorPin = A0;
const int smokeSensorPin = A1;

// Define pins for LEDs, buzzer, and relay
const int greenLedPin = 2;
const int yellowLedPin = 3;
const int redLedPin = 4;
const int buzzerPin = 13;
const int relayPin = 12; // Relay for water pump

// Define thresholds
const int flameThreshold = 400;
const int smokeThreshold = 70;

// Warmup time for smoke sensor (in seconds)
const int warmupTime = 20;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Set sensor pins as input
  pinMode(flameSensorPin, INPUT);
  pinMode(smokeSensorPin, INPUT);
  
  // Set LED, buzzer, and relay pins as output
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  // Turn off all LEDs, buzzer, and relay
  digitalWrite(greenLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(relayPin, HIGH); // Active-low relay: HIGH = OFF
  
  // Warmup countdown
  Serial.println(F("Warming up smoke sensor..."));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Warming up..."));
  for (int i = warmupTime; i >= 0; i--) {
    Serial.print(F("Warming up remaining time: "));
    Serial.print(i);
    Serial.println(F(" seconds"));
    lcd.setCursor(0, 1);
    lcd.print(F("Time: "));
    lcd.print(i);
    lcd.print(F(" sec   "));
    delay(1000);
  }
  Serial.println(F("Warmup complete. Starting sensor readings..."));
  lcd.clear();
}

void loop() {
  // Read sensor values
  int flameValue = analogRead(flameSensorPin);
  int smokeValue = analogRead(smokeSensorPin);
  
  // Determine detection states
  bool flameDetected = (flameValue < flameThreshold);
  bool smokeDetected = (smokeValue > smokeThreshold);
  
  // Control LEDs, buzzer, relay, LCD
  lcd.clear();
  if (flameDetected) {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, LOW); // Active-low relay: LOW = Pump ON
    Serial.println(F("Red LED is on, Pump is ON"));
    lcd.setCursor(0, 0);
    lcd.print(F("Flame Detected!"));
    lcd.setCursor(0, 1);
    lcd.print(F("Value: "));
    lcd.print(flameValue);
  } else if (smokeDetected) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, HIGH); // Pump OFF
    Serial.println(F("Yellow LED is on"));
    lcd.setCursor(0, 0);
    lcd.print(F("Smoke Detected!"));
    lcd.setCursor(0, 1);
    lcd.print(F("Value: "));
    lcd.print(smokeValue);
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, HIGH); // Pump OFF
    Serial.println(F("Green LED is on"));
    lcd.setCursor(0, 0);
    lcd.print(F("All Good"));
    lcd.setCursor(0, 1);
    lcd.print(F("No Flame/Smoke"));
  }
  
  // Print sensor status
  Serial.print(F("Flame Sensor Value: "));
  Serial.print(flameValue);
  Serial.println(flameDetected ? F(" - Flame Detected!") : F(" - No Flame Detected"));
  Serial.print(F("Smoke Sensor Value: "));
  Serial.print(smokeValue);
  Serial.println(smokeDetected ? F(" - Smoke Detected!") : F(" - No Smoke Detected"));
  Serial.println(F("-------------------"));
  
  // Delay for stable readings
  delay(500);
}