#define RED_PIN 8
#define YELLOW_PIN 5
#define GREEN_PIN 2
#define MODE_TOGGLE_PIN 11 // Button to toggle mode and advance to next step in manual mode
#define AUTO_MODE_PIN 12   // Button to return to auto mode
#define BUZZER_PIN 10      // Buzzer pin for beeping each second

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool isAutoMode = true;      // Start in auto mode
int currentManualPhase = 0;  // 0: Red, 1: Yellow, 2: Green in manual mode
unsigned long lastModeDebounceTime = 0; // For mode button debouncing
unsigned long lastAutoDebounceTime = 0; // For auto button debouncing
bool lastModeButtonState = HIGH;        // Last state of mode toggle button (HIGH = not pressed)
bool lastAutoButtonState = HIGH;        // Last state of auto mode button (HIGH = not pressed)
bool modeButtonPressed = false;         // Track mode button press state
bool autoButtonPressed = false;         // Track auto button press state
const unsigned long debounceDelay = 50; // Debounce time in ms
bool isInitialized = false;             // Flag to ensure initialization is complete

// Timing variables for auto mode
unsigned long redStartTime = 0;c:\Users\mohri\OneDrive\Documents\Arduino\Temprature_And_Humidity_Module\Temprature_And_Humidity_Module.ino
unsigned long yellowStartTime = 0;
unsigned long greenStartTime = 0;
int currentAutoPhase = 0; // 0: Red, 1: Green, 2: Yellow
int countdown = 5;        // Countdown for red and green phases
unsigned long lastBeepTime = 0; // Track last buzzer beep time

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MODE_TOGGLE_PIN, INPUT_PULLUP); // Button with internal pull-up
  pinMode(AUTO_MODE_PIN, INPUT_PULLUP);   // Button with internal pull-up
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Halt if OLED fails
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Initializing..."));
  display.display();
  
  Serial.begin(9600); // Initialize Serial communication
  Serial.println("Traffic Signal System Initializing...");
  
  // Wait 2 seconds for hardware stabilization
  delay(2000);
  
  // Log button states multiple times to detect wiring issues
  bool wiringIssue = false;
  for (int i = 0; i < 5; i++) {
    bool modeState = digitalRead(MODE_TOGGLE_PIN);
    bool autoState = digitalRead(AUTO_MODE_PIN);
    Serial.print("Check ");
    Serial.print(i + 1);
    Serial.print(" - Mode Toggle Button (Pin 12) State: ");
    Serial.print(modeState == HIGH ? "HIGH" : "LOW");
    Serial.print(", Auto Mode Button (Pin 11) State: ");
    Serial.println(autoState == HIGH ? "HIGH" : "LOW");
    if (modeState == LOW || autoState == LOW) {
      wiringIssue = true;
    }
    delay(100); // Short delay between checks
  }
  
  // Validate initial button states
  if (wiringIssue) {
    Serial.println("ERROR: One or both buttons are LOW at startup. Check wiring (buttons should connect to ground when pressed).");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Wiring Error"));
    display.println(F("Check Buttons"));
    display.display();
    while (true); // Halt execution until reset
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
  
  // Display initial state
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("Auto: Red"));
  display.print(F("Time: "));
  display.print(countdown);
  display.println(F("s"));
  display.display();
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

  // Check for simultaneous button presses (possible wiring issue)
  if (modeButtonState == LOW && autoButtonState == LOW) {
    Serial.println("WARNING: Both buttons LOW simultaneously. Check wiring for shorts.");
  }

  // Handle mode toggle button (pin 12)
  if (modeButtonState == LOW && lastModeButtonState == HIGH && !modeButtonPressed) {
    lastModeDebounceTime = currentMillis;
    modeButtonPressed = true;
  }
  if (modeButtonState == HIGH && modeButtonPressed && (currentMillis - lastModeDebounceTime) > debounceDelay) {
    Serial.print("Mode toggle button pressed, debounce time: ");
    Serial.print(debounceDelay);
    Serial.println("ms, Function: Switch to manual mode or advance phase");
    if (isAutoMode) {
      isAutoMode = false;
      currentManualPhase = 0;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Switched to Manual Mode - Red Light ON");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println(F("Manual: Red"));
      display.display();
    } else {
      currentManualPhase = (currentManualPhase + 1) % 3;
      if (currentManualPhase == 0) {
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        Serial.println("Manual Mode - Red Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Manual: Red"));
        display.display();
      } else if (currentManualPhase == 1) {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
        Serial.println("Manual Mode - Yellow Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Manual: Yellow"));
        display.display();
      } else {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        Serial.println("Manual Mode - Green Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Manual: Green"));
        display.display();
      }
    }
    modeButtonPressed = false;
  }
  lastModeButtonState = modeButtonState;

  // Handle auto mode button (pin 11)
  if (autoButtonState == LOW && lastAutoButtonState == HIGH && !autoButtonPressed) {
    lastAutoDebounceTime = currentMillis;
    autoButtonPressed = true;
  }
  if (autoButtonState == HIGH && autoButtonPressed && (currentMillis - lastAutoDebounceTime) > debounceDelay) {
    Serial.print("Auto mode button pressed, debounce time: ");
    Serial.print(debounceDelay);
    Serial.println("ms, Function: Return to auto mode");
    if (!isAutoMode) {
      isAutoMode = true;
      currentAutoPhase = 0;
      redStartTime = currentMillis;
      countdown = 5;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Switched to Auto Mode - Red Light ON");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println(F("Auto: Red"));
      display.print(F("Time: "));
      display.print(countdown);
      display.println(F("s"));
      display.display();
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
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Auto: Red"));
        display.print(F("Time: "));
        display.print(countdown);
        display.println(F("s"));
        display.display();
        // Beep buzzer for 100ms
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        lastBeepTime = currentMillis;
        delay(10); // Small delay to prevent serial overload
      }
      if (currentMillis - redStartTime >= 5000) {
        currentAutoPhase = 1;
        greenStartTime = currentMillis;
        countdown = 5;
        Serial.println("Green Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Auto: Green"));
        display.print(F("Time: "));
        display.print(countdown);
        display.println(F("s"));
        display.display();
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
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Auto: Green"));
        display.print(F("Time: "));
        display.print(countdown);
        display.println(F("s"));
        display.display();
        // Beep buzzer for 100ms
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        lastBeepTime = currentMillis;
        delay(10); // Small delay to prevent serial overload
      }
      if (currentMillis - greenStartTime >= 5000) {
        currentAutoPhase = 2;
        yellowStartTime = currentMillis;
        countdown = 5;
        Serial.println("Yellow Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Auto: Yellow"));
        display.println(F("Time: 2s"));
        display.display();
      }
    }
    // Yellow phase (2 seconds)
    else if (currentAutoPhase == 2) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      if (currentMillis - yellowStartTime >= 2000) {
        currentAutoPhase = 0;
        redStartTime = currentMillis;
        countdown = 5;
        Serial.println("Red Light ON");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Auto: Red"));
        display.print(F("Time: "));
        display.print(countdown);
        display.println(F("s"));
        display.display();
      }
    }
  }
}