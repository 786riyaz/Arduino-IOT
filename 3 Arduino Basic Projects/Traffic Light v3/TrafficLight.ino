#define RED_PIN 8
#define YELLOW_PIN 5
#define GREEN_PIN 2
#define MODE_TOGGLE_PIN 11 // Button to toggle mode and advance to next step in manual mode
#define AUTO_MODE_PIN 12   // Button to return to auto mode
#define BUZZER_PIN 10      // Buzzer pin for beeping each second

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool isAutoMode = true;      // Start in auto mode
int currentManualPhase = 0;  // 0: Red, 1: Yellow, 2: Green in manual mode
unsigned long lastModeDebounceTime = 0;
unsigned long lastAutoDebounceTime = 0;
bool lastModeButtonState = HIGH;
bool lastAutoButtonState = HIGH;
bool modeButtonPressed = false;
bool autoButtonPressed = false;
const unsigned long debounceDelay = 50;
bool isInitialized = false;

// Timing variables for auto mode
unsigned long redStartTime = 0;
unsigned long yellowStartTime = 0;
unsigned long greenStartTime = 0;
int currentAutoPhase = 0; // 0: Red, 1: Green, 2: Yellow
int countdown = 5;
unsigned long lastBeepTime = 0;
const unsigned long beepDuration = 100; // Buzzer beep duration in ms
bool buzzerOn = false;

void updateDisplay(const char* mode, const char* phase, int time = -1) {
  Serial.print("Display update: ");
  Serial.print(mode);
  Serial.print(", ");
  Serial.print(phase);
  if (time >= 0) {
    Serial.print(", Time: ");
    Serial.print(time);
    Serial.println("s");
  } else {
    Serial.println();
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(mode);
  display.print(": ");
  display.println(phase);
  if (time >= 0) {
    display.print(F("Time: "));
    display.print(time);
    display.println(F("s"));
  }
  display.display();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MODE_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(AUTO_MODE_PIN, INPUT_PULLUP);
  
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  updateDisplay("Auto", "Initializing");
  delay(2000);
  
  // Skip button wiring check
  Serial.println("Button check bypassed. Starting in Auto Mode.");
  isInitialized = true;
  
  // Set initial state for auto mode (Red)
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  redStartTime = millis();
  currentAutoPhase = 0;
  updateDisplay("Auto", "Red", countdown);
}

void loop() {
  if (!isInitialized) return;

  unsigned long currentMillis = millis();

  // Handle buzzer non-blocking
  if (buzzerOn && (currentMillis - lastBeepTime >= beepDuration)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }

  // Read button states
  bool modeButtonState = digitalRead(MODE_TOGGLE_PIN);
  bool autoButtonState = digitalRead(AUTO_MODE_PIN);

  // Debug button state changes
  if (modeButtonState != lastModeButtonState) {
    Serial.print("Mode button state changed to: ");
    Serial.println(modeButtonState == HIGH ? "HIGH" : "LOW");
    lastModeDebounceTime = currentMillis;
  }
  if (autoButtonState != lastAutoButtonState) {
    Serial.print("Auto button state changed to: ");
    Serial.println(autoButtonState == HIGH ? "HIGH" : "LOW");
    lastAutoDebounceTime = currentMillis;
  }

  // Check for simultaneous button presses
  if (modeButtonState == LOW && autoButtonState == LOW) {
    Serial.println("WARNING: Both buttons LOW simultaneously. Check wiring.");
  }

  // Handle mode toggle button (D11)
  if (modeButtonState == LOW && lastModeButtonState == HIGH && !modeButtonPressed) {
    lastModeDebounceTime = currentMillis;
    modeButtonPressed = true;
  }
  if (modeButtonState == HIGH && modeButtonPressed && (currentMillis - lastModeDebounceTime) > debounceDelay) {
    Serial.println("Mode toggle button pressed: Switch to manual or advance phase");
    if (isAutoMode) {
      isAutoMode = false;
      currentManualPhase = 0;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      updateDisplay("Manual", "Red");
      Serial.println("Switched to Manual Mode - Red Light ON");
    } else {
      currentManualPhase = (currentManualPhase + 1) % 3;
      if (currentManualPhase == 0) {
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        updateDisplay("Manual", "Red");
        Serial.println("Manual Mode - Red Light ON");
      } else if (currentManualPhase == 1) {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
        updateDisplay("Manual", "Yellow");
        Serial.println("Manual Mode - Yellow Light ON");
      } else {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        updateDisplay("Manual", "Green");
        Serial.println("Manual Mode - Green Light ON");
      }
    }
    modeButtonPressed = false;
  }
  lastModeButtonState = modeButtonState;

  // Handle auto mode button (D12)
  if (autoButtonState == LOW && lastAutoButtonState == HIGH && !autoButtonPressed) {
    lastAutoDebounceTime = currentMillis;
    autoButtonPressed = true;
  }
  if (autoButtonState == HIGH && autoButtonPressed && (currentMillis - lastAutoDebounceTime) > debounceDelay) {
    Serial.println("Auto mode button pressed: Return to auto mode");
    if (!isAutoMode) {
      isAutoMode = true;
      currentAutoPhase = 0;
      redStartTime = currentMillis;
      countdown = 5;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      updateDisplay("Auto", "Red", countdown);
      Serial.println("Switched to Auto Mode - Red Light ON");
    } else {
      Serial.println("Already in Auto Mode - No action taken");
    }
    autoButtonPressed = false;
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
        updateDisplay("Auto", "Red", countdown);
        if (!buzzerOn) {
          digitalWrite(BUZZER_PIN, HIGH);
          lastBeepTime = currentMillis;
          buzzerOn = true;
        }
      }
      if (currentMillis - redStartTime >= 5000) {
        currentAutoPhase = 1;
        greenStartTime = currentMillis;
        countdown = 5;
        updateDisplay("Auto", "Green", countdown);
        Serial.println("Green Light ON");
      }
    }
    // Green phase (5 seconds)
    else if (currentAutoPhase == 1) {
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
        updateDisplay("Auto", "Green", countdown);
        if (!buzzerOn) {
          digitalWrite(BUZZER_PIN, HIGH);
          lastBeepTime = currentMillis;
          buzzerOn = true;
        }
      }
      if (currentMillis - greenStartTime >= 5000) {
        currentAutoPhase = 2;
        yellowStartTime = currentMillis;
        countdown = 2;
        updateDisplay("Auto", "Yellow", countdown);
        Serial.println("Yellow Light ON");
      }
    }
    // Yellow phase (2 seconds)
    else if (currentAutoPhase == 2) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      int secondsElapsed = (currentMillis - yellowStartTime) / 1000;
      int secondsRemaining = 2 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
        updateDisplay("Auto", "Yellow", countdown);
        Serial.print("Yellow for ");
        Serial.print(countdown);
        Serial.println(" Seconds");
      }
      if (currentMillis - yellowStartTime >= 2000) {
        currentAutoPhase = 0;
        redStartTime = currentMillis;
        countdown = 5;
        updateDisplay("Auto", "Red", countdown);
        Serial.println("Red Light ON");
      }
    }
  }
}