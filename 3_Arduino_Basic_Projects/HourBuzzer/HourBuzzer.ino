void setup() {
  pinMode(13, OUTPUT);  // Initialize pin 13 as an output for the buzzer
}

void loop() {
  unsigned long currentTime = millis();                   // Get current time in milliseconds
  static unsigned long lastBeepTime = 0;                  // Store the last time the buzzer beeped
  const unsigned long hourInterval = (3600000 / 60) / 20;  // 1 hour in milliseconds (60 * 60 * 1000)
  // const unsigned long beepDuration = 1000;                // Beep for 1 second (1000 milliseconds)
  const unsigned long beepDuration = 200;  // Beep for 0.5 seconds (500 milliseconds)


  // Check if one hour has passed since the last beep
  if (currentTime - lastBeepTime >= hourInterval) {
    digitalWrite(13, HIGH);  // Turn the buzzer on
    // For a passive buzzer, optionally use tone(13, 1000); for a 1000 Hz beep
    delay(beepDuration);         // Beep for 1 second
    digitalWrite(13, LOW);       // Turn the buzzer off
                                 // For a passive buzzer, use noTone(13); if tone() was used
    lastBeepTime = currentTime;  // Update the last beep time
  }
}