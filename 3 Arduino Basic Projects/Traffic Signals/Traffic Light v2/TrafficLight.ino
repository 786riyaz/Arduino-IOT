#define RED_PIN 7
#define YELLOW_PIN 6
#define GREEN_PIN 5
#define MODE_TOGGLE_PIN 12 // Button to toggle mode and advance to next step in manual mode
#define AUTO_MODE_PIN 8    // Button to return to auto mode

bool isAutoMode = true;      // Start in auto mode
int currentManualPhase = 0;  // 0: Red, 1: Yellow, 2: Green in manual mode
unsigned long lastModeDebounceTime = 0; // For mode button debouncing
unsigned long lastAutoDebounceTime = 0; // For auto button debouncing
bool lastModeButtonState = LOW;         // Last state of mode toggle button
bool lastAutoButtonState = LOW;         // Last state of auto mode button
const unsigned long debounceDelay = 100; // Debounce time in ms

// Timing variables for auto mode
unsigned long redStartTime = 0;
unsigned long yellowStartTime = 0;
unsigned long greenStartTime = 0;
int currentAutoPhase = 0; // 0: Red, 1: Yellow, 2: Green
int countdown = 5;        // Countdown for red and green phases
bool isInitialized = false; // Flag to ensure initialization is complete

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(MODE_TOGGLE_PIN, INPUT); // Button for mode toggle and manual control
  pinMode(AUTO_MODE_PIN, INPUT);   // Button to return to auto mode
  
  Serial.begin(9600); // Initialize Serial communication
  Serial.println("Traffic Signal System Initializing...");
  
  // Wait 2 seconds for hardware stabilization
  delay(2000);
  
  // Log button states multiple times to detect floating pins
  for (int i = 0; i < 5; i++) {
    Serial.print("Check ");
    Serial.print(i + 1);
    Serial.print(" - Mode Toggle Button (Pin 12) State: ");
    Serial.print(digitalRead(MODE_TOGGLE_PIN) == HIGH ? "HIGH" : "LOW");
    Serial.print(", Auto Mode Button (Pin 8) State: ");
    Serial.println(digitalRead(AUTO_MODE_PIN) == HIGH ? "HIGH" : "LOW");
    delay(100); // Short delay between checks
  }
  
  // Validate initial button states (both should be LOW)
  if (digitalRead(MODE_TOGGLE_PIN) == HIGH || digitalRead(AUTO_MODE_PIN) == HIGH) {
    Serial.println("ERROR: One or both buttons are HIGH at startup. Check wiring (pull-down resistors).");
  } else {
    Serial.println("Button states OK. Starting in Auto Mode.");
    isInitialized = true;
  }
  
  // Set initial state for auto mode (start with red)
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  redStartTime = millis();
  currentAutoPhase = 0;
}

void loop() {
  // Skip processing until initialization is complete
  if (!isInitialized) {
    return;
  }

  unsigned long currentMillis = millis();

  // Read button states
  bool modeButtonState = digitalRead(MODE_TOGGLE_PIN);
  bool autoButtonState = digitalRead(AUTO_MODE_PIN);

  // Handle mode toggle button (pin 12)
  if (modeButtonState != lastModeButtonState) {
    lastModeDebounceTime = currentMillis;
  }
  if ((currentMillis - lastModeDebounceTime) > debounceDelay && modeButtonState == HIGH) {
    Serial.print("Mode toggle button pressed, debounce time: ");
    Serial.print(debounceDelay);
    Serial.println("ms, Function: Switch to manual mode or advance phase");
    if (isAutoMode) {
      // Switch to manual mode
      isAutoMode = false;
      currentManualPhase = 0; // Start manual mode with red light
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Switched to Manual Mode - Red Light ON");
    } else {
      // In manual mode, advance to the next phase
      currentManualPhase = (currentManualPhase + 1) % 3; // Cycle: 0 (red) -> 1 (yellow) -> 2 (green) -> 0
      if (currentManualPhase == 0) {
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        Serial.println("Manual Mode - Red Light ON");
      } else if (currentManualPhase == 1) {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
        Serial.println("Manual Mode - Yellow Light ON");
      } else {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        Serial.println("Manual Mode - Green Light ON");
      }
    }
  }
  lastModeButtonState = modeButtonState;

  // Handle auto mode button (pin 8)
  if (autoButtonState != lastAutoButtonState) {
    lastAutoDebounceTime = currentMillis;
  }
  if ((currentMillis - lastAutoDebounceTime) > debounceDelay && autoButtonState == HIGH) {
    Serial.print("Auto mode button pressed, debounce time: ");
    Serial.print(debounceDelay);
    Serial.println("ms, Function: Return to auto mode");
    if (!isAutoMode) {
      isAutoMode = true;
      currentAutoPhase = 0; // Start with red phase
      redStartTime = currentMillis;
      countdown = 5;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Switched to Auto Mode - Red Light ON");
    } else {
      Serial.println("Already in Auto Mode - No action taken");
    }
  }
  lastAutoButtonState = autoButtonState;

  // Auto mode logic
  if (isAutoMode) {
    // Red phase (5 seconds)
    if (currentAutoPhase == 0) {
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      int secondsElapsed = (currentMillis - redStartTime) / 1000;
      int secondsRemaining = 5 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
        Serial.print("Stop for ");
        Serial.print(countdown);
        Serial.println(" Seconds");
      }
      if (currentMillis - redStartTime >= 5000) {
        currentAutoPhase = 1;
        yellowStartTime = currentMillis;
        countdown = 5; // Reset for next phase
        Serial.println("Yellow Light ON");
      }
    }
    // Yellow phase (2 seconds)
    else if (currentAutoPhase == 1) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      if (currentMillis - yellowStartTime >= 2000) {
        currentAutoPhase = 2;
        greenStartTime = currentMillis;
        countdown = 5; // Reset for next phase
        Serial.println("Green Light ON");
      }
    }
    // Green phase (5 seconds)
    else if (currentAutoPhase == 2) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      int secondsElapsed = (currentMillis - greenStartTime) / 1000;
      int secondsRemaining = 5 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
        Serial.print("Go for ");
        Serial.print(countdown);
        Serial.println(" Seconds");
      }
      if (currentMillis - greenStartTime >= 5000) {
        currentAutoPhase = 0;
        redStartTime = currentMillis;
        countdown = 5; // Reset for next phase
        Serial.println("Red Light ON");
      }
    }
  }
}