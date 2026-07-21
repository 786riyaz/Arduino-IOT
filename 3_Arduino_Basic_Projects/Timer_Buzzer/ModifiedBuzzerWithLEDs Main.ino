void setup() {
  // Initialize buzzer pin
  pinMode(13, OUTPUT); // Buzzer on pin 13
  // Initialize LED pins as outputs
  pinMode(2, OUTPUT);  // LED for 1 hour
  pinMode(4, OUTPUT);  // LED for 30 minutes
  pinMode(6, OUTPUT);  // LED for 15 minutes
  pinMode(8, OUTPUT);  // LED for 10 minutes
  pinMode(10, OUTPUT); // LED for 5 minutes
  // Initialize button pin with internal pull-up
  pinMode(12, INPUT_PULLUP); // Button on pin 12
  // Initialize Serial for debugging
  Serial.begin(9600);
  Serial.println("Default interval: 60 minutes (1 hour), LED on pin 2");
  // Turn on default LED (1 hour)
  digitalWrite(2, HIGH);
}

// Array of intervals in milliseconds: 1 hour, 30 min, 15 min, 10 min, 5 min
const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000};
// const unsigned long intervals[] = {3600, 1800, 900, 600, 300};
// Array of LED pins corresponding to intervals
const int ledPins[] = {2, 4, 6, 8, 10};
const int numIntervals = 5; // Number of intervals
int currentIntervalIndex = 0; // Start with 1 hour (index 0)

void loop() {
  unsigned long currentTime = millis(); // Get current time in milliseconds
  static unsigned long lastBeepTime = 0; // Store the last time the buzzer beeped
  static unsigned long lastButtonPress = 0; // Store the last button press time for debouncing
  const unsigned long beepDuration = 100; // Beep for 0.1 seconds (100 milliseconds)
  const unsigned long debounceDelay = 350; // Debounce delay in milliseconds

  // Check for button press on pin 12 (LOW when pressed due to pull-up)
  if (digitalRead(12) == LOW && currentTime - lastButtonPress > debounceDelay) {
    // Turn off current LED
    digitalWrite(ledPins[currentIntervalIndex], LOW);
    // Increment interval index, cycle back to 0 if at the end
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    // Turn on new LED
    digitalWrite(ledPins[currentIntervalIndex], HIGH);
    // Update last button press time
    lastButtonPress = currentTime;
    // Print current interval for debugging
    Serial.print("Interval changed to: ");
    Serial.print(intervals[currentIntervalIndex] / 60000); // Convert to minutes
    Serial.print(" minutes, LED on pin ");
    Serial.println(ledPins[currentIntervalIndex]);
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