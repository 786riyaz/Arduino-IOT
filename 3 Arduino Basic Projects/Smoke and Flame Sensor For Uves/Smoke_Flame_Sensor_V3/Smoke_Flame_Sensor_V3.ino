#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Initialize LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize OLED (128x64, address 0x3C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define pins for sensors
const int flameSensorPin = A0;  // Analog pin for flame sensor
const int smokeSensorPin = A1;  // Analog pin for smoke sensor

// Define pins for LEDs and buzzer
const int greenLedPin = 2;      // Green LED for all good
const int yellowLedPin = 3;     // Yellow LED for smoke
const int redLedPin = 4;        // Red LED for flame
const int buzzerPin = 13;       // Buzzer for flame or smoke detection

// Define thresholds (adjust based on calibration)
const int flameThreshold = 400; // Lower values indicate flame
const int smokeThreshold = 70;  // Higher values indicate smoke

// Warmup time for smoke sensor (in seconds)
const int warmupTime = 20;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed. Check wiring or address.");
    for(;;); // Halt if OLED fails
  }
  oled.clearDisplay();
  oled.setTextSize(1);      // Small font for OLED
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.display();
  
  // Set sensor pins as input
  pinMode(flameSensorPin, INPUT);
  pinMode(smokeSensorPin, INPUT);
  
  // Set LED and buzzer pins as output
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Turn off all LEDs and buzzer initially
  digitalWrite(greenLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  
  // Warmup countdown for smoke sensor
  Serial.println("Warming up smoke sensor...");
  lcd.clear();
  oled.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print("Warming up...");
  oled.setCursor(0, 0);
  oled.print("Warming up...");
  for (int i = warmupTime; i >= 0; i--) {
    Serial.print("Warming up remaining time: ");
    Serial.print(i);
    Serial.println(" seconds");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(i);
    lcd.print(" sec   ");  // Clear extra digits
    oled.setCursor(0, 16); // Second line on OLED
    oled.print("Time: ");
    oled.print(i);
    oled.print(" sec   ");
    oled.display();
    delay(1000);  // Wait 1 second
  }
  Serial.println("Warmup complete. Starting sensor readings...");
  lcd.clear();
  oled.clearDisplay();
  oled.display();
}

void loop() {
  // Read analog values from both sensors
  int flameValue = analogRead(flameSensorPin);
  int smokeValue = analogRead(smokeSensorPin);
  
  // Determine detection states
  bool flameDetected = (flameValue < flameThreshold);
  bool smokeDetected = (smokeValue > smokeThreshold);
  
  // Control LEDs, buzzer, LCD, OLED, and print LED status
  lcd.clear();
  oled.clearDisplay();
  oled.setCursor(0, 0);
  if (flameDetected) {
    digitalWrite(redLedPin, HIGH);    // Red LED on for flame
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);    // Buzzer on for flame
    Serial.println("Red LED is on");
    lcd.setCursor(0, 0);
    lcd.print("Flame Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Value: ");
    lcd.print(flameValue);
    oled.print("Flame Detected!");
    oled.setCursor(0, 16);
    oled.print("Value: ");
    oled.print(flameValue);
  } else if (smokeDetected) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH); // Yellow LED on for smoke
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);    // Buzzer on for smoke
    Serial.println("Yellow LED is on");
    lcd.setCursor(0, 0);
    lcd.print("Smoke Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Value: ");
    lcd.print(smokeValue);
    oled.print("Smoke Detected!");
    oled.setCursor(0, 16);
    oled.print("Value: ");
    oled.print(smokeValue);
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);  // Green LED on for all good
    digitalWrite(buzzerPin, LOW);     // Buzzer off
    Serial.println("Green LED is on");
    lcd.setCursor(0, 0);
    lcd.print("All Good");
    lcd.setCursor(0, 1);
    lcd.print("No Flame/Smoke");
    oled.print("All Good");
    oled.setCursor(0, 16);
    oled.print("No Flame/Smoke");
  }
  oled.display();
  
  // Print flame sensor status
  Serial.print("Flame Sensor Value: ");
  Serial.print(flameValue);
  if (flameDetected) {
    Serial.println(" - Flame Detected!");
  } else {
    Serial.println(" - No Flame Detected");
  }
  
  // Print smoke sensor status
  Serial.print("Smoke Sensor Value: ");
  Serial.print(smokeValue);
  if (smokeDetected) {
    Serial.println(" - Smoke Detected!");
  } else {
    Serial.println(" - No Smoke Detected");
  }
  
  // Separator for readability
  Serial.println("-------------------");
  
  // Delay for stable readings
  delay(500);
}