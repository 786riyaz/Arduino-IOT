void setup() {
  pinMode(13, OUTPUT); // Initialize pin 13 as output for the buzzer
  pinMode(12, INPUT_PULLUP); // Initialize pin 12 as input with internal pull-up for the button
  Serial.begin(9600); // Start Serial for debugging (optional)
  Serial.println("Default interval: 60 minutes (1 hour)"); // Confirm default interval
}

// Array of intervals in milliseconds: 1 hour, 30 min, 15 min, 10 min, 5 min
const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000};
const int numIntervals = 5; // Number of intervals
int currentIntervalIndex = 0; // Start with 1 hour (index 0)

void loop() {
  unsigned long currentTime = millis(); // Get current time in milliseconds
  static unsigned long lastBeepTime = 0; // Store the last time the buzzer beeped
  static unsigned long lastButtonPress = 0; // Store the last button press time for debouncing
  const unsigned long beepDuration = 100; // Beep for 0.1 seconds (100 milliseconds)
  const unsigned long debounceDelay = 500; // Debounce delay in milliseconds

  // Check for button press on pin 12 (LOW when pressed due to pull-up)
  if (digitalRead(12) == LOW && currentTime - lastButtonPress > debounceDelay) {
    // Increment interval index, cycle back to 0 if at the end
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    lastButtonPress = currentTime; // Update last button press time
    // Print current interval for debugging (optional)
    Serial.print("Interval changed to: ");
    Serial.print(intervals[currentIntervalIndex] / 60000); // Convert to minutes
    Serial.println(" minutes");
  }

  // Check if the selected interval has passed since the last beep
  if (currentTime - lastBeepTime >= intervals[currentIntervalIndex]) {
    digitalWrite(13, HIGH); // Turn the buzzer on (for active buzzer)
    // For a passive buzzer, use: tone(13, 1000); // 1000 Hz beep
    delay(beepDuration);    // Beep for 0.1 seconds
    digitalWrite(13, LOW);  // Turn the buzzer off
    // For a passive buzzer, use: noTone(13); // Stop the tone
    lastBeepTime = currentTime; // Update the last beep time
  }
}