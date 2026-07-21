// Define the pin for the flame sensor
const int flameSensorPin = A0;  // Analog pin for flame sensor
const int threshold = 400;      // Threshold value for flame detection (adjust based on testing)

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Set flame sensor pin as input
  pinMode(flameSensorPin, INPUT);
}

void loop() {
  // Read the analog value from the flame sensor
  int flameValue = analogRead(flameSensorPin);
  
  // Print the flame sensor value to Serial Monitor
  Serial.print("Flame Sensor Value: ");
  Serial.print(flameValue);
  
  // Check if flame is detected based on threshold
  if (flameValue < threshold) {
    Serial.println(" - Flame Detected!");
  } else {
    Serial.println(" - No Flame Detected");
  }
  
  // Add a small delay for stable readings
  delay(500);
}