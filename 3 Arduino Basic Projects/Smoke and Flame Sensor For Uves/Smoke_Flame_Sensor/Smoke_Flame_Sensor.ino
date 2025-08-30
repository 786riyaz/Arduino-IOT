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
const int warmupTime = 20/5;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
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
  for (int i = warmupTime; i >= 0; i--) {
    Serial.print("Warming up remaining time: ");
    Serial.print(i);
    Serial.println(" seconds");
    delay(1000);  // Wait 1 second
  }
  Serial.println("Warmup complete. Starting sensor readings...");
}

void loop() {
  // Read analog values from both sensors
  int flameValue = analogRead(flameSensorPin);
  int smokeValue = analogRead(smokeSensorPin);
  
  // Determine detection states
  bool flameDetected = (flameValue < flameThreshold);
  bool smokeDetected = (smokeValue > smokeThreshold);
  
  // Control LEDs, buzzer, and print LED status
  if (flameDetected) {
    digitalWrite(redLedPin, HIGH);    // Red LED on for flame
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);    // Buzzer on for flame
    Serial.println("Red LED is on");
  } else if (smokeDetected) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH); // Yellow LED on for smoke
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);    // Buzzer on for smoke
    Serial.println("Yellow LED is on");
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);  // Green LED on for all good
    digitalWrite(buzzerPin, LOW);     // Buzzer off
    Serial.println("Green LED is on");
  }
  
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