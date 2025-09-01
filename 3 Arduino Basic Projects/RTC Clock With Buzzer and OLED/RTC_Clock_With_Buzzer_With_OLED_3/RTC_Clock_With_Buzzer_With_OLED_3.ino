#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>

// Initialize OLED with page mode (I2C address 0x3C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Initialize RTC
RTC_DS3231 rtc;

// Days of the week in PROGMEM (uppercase)
const char daysOfTheWeek0[] PROGMEM = "SUN";
const char daysOfTheWeek1[] PROGMEM = "MON";
const char daysOfTheWeek2[] PROGMEM = "TUE";
const char daysOfTheWeek3[] PROGMEM = "WED";
const char daysOfTheWeek4[] PROGMEM = "THU";
const char daysOfTheWeek5[] PROGMEM = "FRI";
const char daysOfTheWeek6[] PROGMEM = "SAT";
const char* const daysOfTheWeek[] PROGMEM = {daysOfTheWeek0, daysOfTheWeek1, daysOfTheWeek2, daysOfTheWeek3, daysOfTheWeek4, daysOfTheWeek5, daysOfTheWeek6};

// Clock variables (updated from RTC)
unsigned int year = 2025;
unsigned char month = 8;
unsigned char day = 26;
unsigned char hour = 0;
unsigned char minute = 7;
unsigned char second = 0;
unsigned char dayOfWeek = 2; // 2 = Tuesday
unsigned char lastSecond = 255; // Track previous second for minute transition

// Buzzer and button pins
#define BUZZER_PIN 13
#define BUTTON_DECREASE_PIN 2 // Button for decreasing timer interval
#define BUTTON_INCREASE_PIN 3 // Button for increasing timer interval
#define BUTTON_HOURLY_BEEP_PIN 12 // Slider for hourly beep
#define BUTTON_30MIN_BEEP_PIN 11 // Slider for 30-minute beep
#define BUTTON_15MIN_BEEP_PIN 10 // Slider for 15-minute beep
#define BUTTON_10MIN_BEEP_PIN 9  // Slider for 10-minute beep
#define BUTTON_5MIN_BEEP_PIN 8   // Slider for 5-minute beep
#define BUTTON_1MIN_BEEP_PIN 7   // Slider for 1-minute beep

// Array of intervals in milliseconds: 1 hour, 30 min, 15 min, 10 min, 5 min, 1 min
const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000, 60000};
const int numIntervals = 6; // Number of intervals
int currentIntervalIndex = 0; // Start with 1 hour (index 0)
unsigned long lastBeepTime = 0; // Store the last time the buzzer beeped for interval
unsigned long lastButtonPress = 0; // Store the last button press time for debouncing
const unsigned long beepDuration = 100; // Beep for 0.1 seconds
const unsigned long debounceDelay = 350; // Debounce delay in milliseconds

// Time-based beep enable flags
bool beepEnabled[6] = {false}; // 0: hourly, 1: 30min, 2: 15min, 3: 10min, 4: 5min, 5: 1min
const int timeBeepPins[6] = {BUTTON_HOURLY_BEEP_PIN, BUTTON_30MIN_BEEP_PIN, BUTTON_15MIN_BEEP_PIN, BUTTON_10MIN_BEEP_PIN, BUTTON_5MIN_BEEP_PIN, BUTTON_1MIN_BEEP_PIN};
const char* beepNames[6] = {"Hourly", "30-min", "15-min", "10-min", "5-min", "1-min"};
bool lastButtonState[6] = {true, true, true, true, true, true}; // Track previous state (HIGH for INPUT_PULLUP)

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Arduino..."));

  // Initialize buzzer and button pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_DECREASE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_INCREASE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_HOURLY_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_30MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_15MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_10MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_5MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_1MIN_BEEP_PIN, INPUT_PULLUP);
  Serial.println(F("Default interval: 60 minutes (1 hour)"));

  Wire.begin();
  
  // Initialize OLED
  Serial.println(F("Initializing OLED..."));
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() == 0) {
    Serial.println(F("OLED found at 0x3C"));
    u8g2.begin();
    Serial.println(F("OLED initialized."));
  } else {
    Serial.println(F("ERROR: OLED not found!"));
    while (1);
  }
  
  // Initialize RTC
  Serial.println(F("Initializing RTC..."));
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    Serial.println(F("RTC found at 0x68"));
    if (!rtc.begin()) {
      Serial.println(F("ERROR: Couldn't initialize RTC!"));
      while (1);
    }
    if (rtc.lostPower()) {
      Serial.println(F("RTC lost power, setting the time!"));
      // Set to current time (e.g., August 26, 2025, 12:07:00 AM IST)
      rtc.adjust(DateTime(2025, 8, 26, 0, 7, 0));
    }
    Serial.println(F("RTC initialized."));
  } else {
    Serial.println(F("ERROR: RTC not found!"));
    while (1);
  }
}

void loop() {
  unsigned long currentTime = millis();

  // Read current time from RTC
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  dayOfWeek = now.dayOfTheWeek();

  // Handle slider switches for time-based beep toggles (pins 12, 11, 10, 9, 8, 7)
  for (int i = 0; i < 6; i++) {
    bool currentState = digitalRead(timeBeepPins[i]);
    if (currentState != lastButtonState[i] && currentTime - lastButtonPress > debounceDelay) {
      if (currentState == LOW) { // Slider switched ON (LOW due to INPUT_PULLUP)
        // Disable all other beep modes
        for (int j = 0; j < 6; j++) {
          if (j != i) beepEnabled[j] = false;
        }
        // Enable the selected beep mode
        beepEnabled[i] = true;
        digitalWrite(BUZZER_PIN, HIGH);
        delay(beepDuration);
        digitalWrite(BUZZER_PIN, LOW);
        Serial.print(beepNames[i]);
        Serial.println(F(" beep enabled"));
      } else { // Slider switched OFF (HIGH due to INPUT_PULLUP)
        beepEnabled[i] = false;
        digitalWrite(BUZZER_PIN, HIGH);
        delay(beepDuration);
        digitalWrite(BUZZER_PIN, LOW);
        Serial.print(beepNames[i]);
        Serial.println(F(" beep disabled"));
      }
      lastButtonPress = currentTime;
      lastButtonState[i] = currentState;
    }
  }

  // Handle button press for decreasing interval (pin 2)
  if (digitalRead(BUTTON_DECREASE_PIN) == LOW && currentTime - lastButtonPress > debounceDelay) {
    currentIntervalIndex = (currentIntervalIndex - 1 + numIntervals) % numIntervals; // Decrease index, wrap around
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Buzzer beeped for mode change to: "));
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(F(" minutes (decreased)"));
    lastButtonPress = currentTime;
    lastBeepTime = currentTime; // Reset timer to avoid immediate beep
  }

  // Handle button press for increasing interval (pin 3)
  if (digitalRead(BUTTON_INCREASE_PIN) == LOW && currentTime - lastButtonPress > debounceDelay) {
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals; // Increase index, wrap around
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Buzzer beeped for mode change to: "));
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(F(" minutes (increased)"));
    lastButtonPress = currentTime;
    lastBeepTime = currentTime; // Reset timer to avoid immediate beep
  }

  // Check if the selected interval has passed
  if (currentTime - lastBeepTime >= intervals[currentIntervalIndex]) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Buzzer beeped at interval: "));
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(F(" minutes"));
    lastBeepTime = currentTime;
  }

  // Check for time-based beeps if any mode is enabled
  bool anyBeepEnabled = false;
  int activeBeepMode = -1;
  for (int i = 0; i < 6; i++) {
    if (beepEnabled[i]) {
      anyBeepEnabled = true;
      activeBeepMode = i;
      break;
    }
  }
  if (anyBeepEnabled && second == 0 && lastSecond == 59) {
    bool shouldBeep = false;
    if (activeBeepMode == 0 && minute == 0) { // Hourly beep
      shouldBeep = true;
    } else if (activeBeepMode == 1 && (minute == 0 || minute == 30)) { // 30-min beep
      shouldBeep = true;
    } else if (activeBeepMode == 2 && (minute % 15 == 0)) { // 15-min beep
      shouldBeep = true;
    } else if (activeBeepMode == 3 && (minute % 10 == 0)) { // 10-min beep
      shouldBeep = true;
    } else if (activeBeepMode == 4 && (minute % 5 == 0)) { // 5-min beep
      shouldBeep = true;
    } else if (activeBeepMode == 5) { // 1-min beep
      shouldBeep = true;
    }
    if (shouldBeep) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(beepDuration);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.print(F("Time-based beep at "));
      Serial.print(hour);
      Serial.print(F(":"));
      Serial.print(minute < 10 ? F("0") : F(""));
      Serial.print(minute);
      Serial.println(F(":00"));
    }
  }
  lastSecond = second; // Update last second for next loop

  // Calculate remaining time until next interval beep
  unsigned long elapsedTime = currentTime - lastBeepTime;
  unsigned long remainingTimeMs = (elapsedTime < intervals[currentIntervalIndex]) ? (intervals[currentIntervalIndex] - elapsedTime) : 0;
  unsigned int remainingSeconds = remainingTimeMs / 1000;
  unsigned int remainingMinutes = remainingSeconds / 60;
  unsigned int remainingHours = remainingMinutes / 60;
  remainingMinutes %= 60;
  remainingSeconds %= 60;

  // Format remaining time (e.g., "Beep: 01:00:00")
  char timerBuf[16];
  sprintf(timerBuf, "Beep: %02d:%02d:%02d", remainingHours, remainingMinutes, remainingSeconds);

  // Convert to 12-hour format
  char timeBuf[12];
  unsigned char displayHour = hour % 12;
  if (displayHour == 0) displayHour = 12;
  const char* ampm = (hour < 12) ? " AM" : " PM";
  sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, minute, second, ampm);

  // Format date in DD/MM format
  char dateBuf[6];
  sprintf(dateBuf, "%02d/%02d", day, month);

  // Get day of the week
  char dayBuf[4];
  strcpy_P(dayBuf, (char*)pgm_read_word(&daysOfTheWeek[dayOfWeek]));

  // Print to Serial Monitor
  Serial.println(F("--- Data Update ---"));
  Serial.print(F("Time: "));
  Serial.println(timeBuf);
  Serial.print(F("Date: "));
  Serial.println(dateBuf);
  Serial.print(F("Day: "));
  Serial.println(dayBuf);
  Serial.print(F("Timer: "));
  Serial.println(timerBuf);
  for (int i = 0; i < 6; i++) {
    Serial.print(beepNames[i]);
    Serial.print(F(" Beep: "));
    Serial.println(beepEnabled[i] ? F("Enabled") : F("Disabled"));
  }
  Serial.println(F("-------------------"));

  // Display on OLED
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB24_tr); // Larger font for time
    u8g2.drawStr(-3, 24, timeBuf);
    u8g2.setFont(u8g2_font_ncenB12_tr); // Smaller font for date, timer, and day
    u8g2.drawStr(1, 38, dateBuf);
    u8g2.drawStr(81, 38, dayBuf);
    u8g2.drawStr(0, 55, timerBuf); // Timer below date and day
  } while (u8g2.nextPage());

  delay(100);
}