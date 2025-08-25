// Define the pin for the smoke sensor
const int smokeSensorPin = A0;  // Analog pin for smoke sensor
const int threshold = 50;      // Threshold value for smoke detection (adjust based on testing)

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Set smoke sensor pin as input
  pinMode(smokeSensorPin, INPUT);
  
  // Allow sensor to warm up (MQ-2 sensors need ~20-30 seconds)
  Serial.println("Warming up smoke sensor...");
  delay(2000);  // 20-second warmup
  Serial.println("Warmup complete");
}

void loop() {
  // Read the analog value from the smoke sensor
  int smokeValue = analogRead(smokeSensorPin);
  
  // Print the smoke sensor value to Serial Monitor
  Serial.print("Smoke Sensor Value: ");
  Serial.print(smokeValue);
  
  // Check if smoke is detected based on threshold
  if (smokeValue > threshold) {
    Serial.println(" - Smoke Detected!");
  } else {
    Serial.println(" - No Smoke Detected");
  }
  
  // Add a small delay for stable readings
  delay(500);
}