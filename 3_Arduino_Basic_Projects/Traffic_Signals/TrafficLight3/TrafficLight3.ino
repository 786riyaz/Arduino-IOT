#define RED_PIN 8
#define YELLOW_PIN 5
#define GREEN_PIN 2
#define MODE_TOGGLE_PIN 11
#define AUTO_MODE_PIN 12
#define BUZZER_PIN 10

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool isAutoMode = true;
int currentManualPhase = 0;
unsigned long lastModeDebounceTime = 0;
unsigned long lastAutoDebounceTime = 0;
bool lastModeButtonState = HIGH;
bool lastAutoButtonState = HIGH;
bool modeButtonPressed = false;
bool autoButtonPressed = false;
const unsigned long debounceDelay = 50;
bool isInitialized = false;
bool isDisplayInitialized = false;

unsigned long redStartTime = 0;
unsigned long yellowStartTime = 0;
unsigned long greenStartTime = 0;
int currentAutoPhase = 0;
int countdown = 5;
unsigned long lastBeepTime = 0;
const unsigned long beepDuration = 100;
bool buzzerOn = false;

void updateDisplay(const char* mode, const char* phase, int time = -1) {
  if (isDisplayInitialized) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    
    if (time >= 0) {
      // Auto mode: Mode : Auto, LED:phase, Time : Xs
      display.setCursor(0, 0);
      display.print(F("Mode: "));
      display.print(mode);
      
      display.setCursor(0, 20);
      display.print(F("LED:"));
      display.print(phase);
      
      display.drawLine(0, 42, 127, 42, SSD1306_WHITE);
      
      display.setCursor(0, 50);
      display.print(F("Time : "));
      display.print(time);
      display.print(F("s"));
    } else {
      // Manual mode: Mode :::, Manual, LED:phase
      display.setCursor(0, 0);
      display.print(F("Mode :::"));
      
      display.setCursor(0, 20);
      display.print(F("Manual"));
      
      display.drawLine(0, 42, 127, 42, SSD1306_WHITE);
      
      display.setCursor(0, 50);
      display.print(F("LED:"));
      display.print(phase);
    }
    
    display.display();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MODE_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(AUTO_MODE_PIN, INPUT_PULLUP);
  
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    isDisplayInitialized = true;
    updateDisplay("Auto", "Init");
  }
  delay(2000);
  
  isInitialized = true;
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

  if (buzzerOn && (currentMillis - lastBeepTime >= beepDuration)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }

  bool modeButtonState = digitalRead(MODE_TOGGLE_PIN);
  bool autoButtonState = digitalRead(AUTO_MODE_PIN);

  if (modeButtonState != lastModeButtonState) {
    lastModeDebounceTime = currentMillis;
  }
  if (autoButtonState != lastAutoButtonState) {
    lastAutoDebounceTime = currentMillis;
  }

  if (modeButtonState == LOW && lastModeButtonState == HIGH && !modeButtonPressed) {
    lastModeDebounceTime = currentMillis;
    modeButtonPressed = true;
  }
  if (modeButtonState == HIGH && modeButtonPressed && (currentMillis - lastModeDebounceTime) > debounceDelay) {
    if (isAutoMode) {
      isAutoMode = false;
      currentManualPhase = 0;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      updateDisplay("Manual", "Red");
    } else {
      currentManualPhase = (currentManualPhase + 1) % 3;
      if (currentManualPhase == 0) {
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        updateDisplay("Manual", "Red");
      } else if (currentManualPhase == 1) {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
        updateDisplay("Manual", "Yellow");
      } else {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        updateDisplay("Manual", "Green");
      }
    }
    modeButtonPressed = false;
  }
  lastModeButtonState = modeButtonState;

  if (autoButtonState == LOW && lastAutoButtonState == HIGH && !autoButtonPressed) {
    lastAutoDebounceTime = currentMillis;
    autoButtonPressed = true;
  }
  if (autoButtonState == HIGH && autoButtonPressed && (currentMillis - lastAutoDebounceTime) > debounceDelay) {
    if (!isAutoMode) {
      isAutoMode = true;
      currentAutoPhase = 0;
      redStartTime = currentMillis;
      countdown = 5;
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      updateDisplay("Auto", "Red", countdown);
    }
    autoButtonPressed = false;
  }
  lastAutoButtonState = autoButtonState;

  if (isAutoMode) {
    if (currentAutoPhase == 0) {
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      int secondsElapsed = (currentMillis - redStartTime) / 1000;
      int secondsRemaining = 5 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
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
      }
    }
    else if (currentAutoPhase == 1) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      int secondsElapsed = (currentMillis - greenStartTime) / 1000;
      int secondsRemaining = 5 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
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
      }
    }
    else if (currentAutoPhase == 2) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      int secondsElapsed = (currentMillis - yellowStartTime) / 1000;
      int secondsRemaining = 2 - secondsElapsed;
      if (secondsRemaining != countdown && secondsRemaining > 0) {
        countdown = secondsRemaining;
        updateDisplay("Auto", "Yellow", countdown);
      }
      if (currentMillis - yellowStartTime >= 2000) {
        currentAutoPhase = 0;
        redStartTime = millis();
        countdown = 5;
        updateDisplay("Auto", "Red", countdown);
      }
    }
  }
}