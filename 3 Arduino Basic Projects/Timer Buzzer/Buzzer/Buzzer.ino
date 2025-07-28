void setup() {
  pinMode(13, OUTPUT); // Buzzer on pin 13
  pinMode(2, OUTPUT);  // LED for 1 hour
  pinMode(4, OUTPUT);  // LED for 30 minutes
  pinMode(6, OUTPUT);  // LED for 15 minutes
  pinMode(8, OUTPUT);  // LED for 10 minutes
  pinMode(10, OUTPUT); // LED for 5 minutes
  pinMode(12, INPUT_PULLUP); // Button on pin 12
  Serial.begin(9600);
  Serial.println("Default interval: 60 minutes (1 hour), LED on pin 2");
  digitalWrite(2, HIGH);
}

const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000, 60000};
const int ledPins[] = {2, 4, 6, 8, 10, -1};
const int numIntervals = 6;
int currentIntervalIndex = 0;

void loop() {
  unsigned long currentTime = millis();
  static unsigned long lastBeepTime = 0;
  static unsigned long lastButtonPress = 0;
  const unsigned long beepDuration = 100;
  const unsigned long debounceDelay = 350;

  if (digitalRead(12) == LOW && currentTime - lastButtonPress > debounceDelay) {
    if (ledPins[currentIntervalIndex] != -1) {
      digitalWrite(ledPins[currentIntervalIndex], LOW);
    }
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    if (ledPins[currentIntervalIndex] != -1) {
      digitalWrite(ledPins[currentIntervalIndex], HIGH);
    }
    digitalWrite(13, HIGH);
    delay(beepDuration);
    digitalWrite(13, LOW);
    Serial.print("Buzzer beeped for mode change to: ");
    Serial.print(intervals[currentIntervalIndex]);
    Serial.print(" minutes, ");
    if (ledPins[currentIntervalIndex] != -1) {
      Serial.print("LED on pin ");
      Serial.println(ledPins[currentIntervalIndex]);
    } else {
      Serial.println("No LED");
    }
    lastButtonPress = currentTime;
  }

  if (currentTime - lastBeepTime >= intervals[currentIntervalIndex]) {
    digitalWrite(13, HIGH);
    delay(beepDuration);
    digitalWrite(13, LOW);
    Serial.print("Buzzer beeped at interval: ");
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(" minutes");
    lastBeepTime = currentTime;
  }
}