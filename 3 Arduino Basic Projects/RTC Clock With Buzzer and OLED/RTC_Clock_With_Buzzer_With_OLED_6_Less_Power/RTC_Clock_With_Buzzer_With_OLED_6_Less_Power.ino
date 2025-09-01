#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>

// Initialize OLED with page mode (I2C address 0x3C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Initialize RTC
RTC_DS3231 rtc;

// Days of the week in PROGMEM
const char daysOfTheWeek0[] PROGMEM = "SUN";
const char daysOfTheWeek1[] PROGMEM = "MON";
const char daysOfTheWeek2[] PROGMEM = "TUE";
const char daysOfTheWeek3[] PROGMEM = "WED";
const char daysOfTheWeek4[] PROGMEM = "THU";
const char daysOfTheWeek5[] PROGMEM = "FRI";
const char daysOfTheWeek6[] PROGMEM = "SAT";
const char* const daysOfTheWeek[] PROGMEM = {daysOfTheWeek0, daysOfTheWeek1, daysOfTheWeek2, daysOfTheWeek3, daysOfTheWeek4, daysOfTheWeek5, daysOfTheWeek6};

// Clock variables
unsigned int year = 2025;
unsigned char month = 8;
unsigned char day = 27;
unsigned char hour = 12;
unsigned char minute = 35;
unsigned char second = 0;
unsigned char dayOfWeek = 3; // 3 = Wednesday
unsigned char lastSecond = 0;

// Pins
#define BUZZER_PIN 13
#define BUTTON_DECREASE_PIN 3
#define BUTTON_INCREASE_PIN 2
#define BUTTON_HOURLY_BEEP_PIN 6
#define BUTTON_30MIN_BEEP_PIN 11
#define BUTTON_15MIN_BEEP_PIN 10
#define BUTTON_10MIN_BEEP_PIN 9
#define BUTTON_5MIN_BEEP_PIN 8
#define BUTTON_1MIN_BEEP_PIN 7
#define RTC_SQW_PIN 2  // Interrupt pin for RTC SQW

// Intervals in milliseconds
const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000, 60000, ULONG_MAX};
const int numIntervals = 7;
int currentIntervalIndex = 6;
unsigned long lastBeepTime = 0;
unsigned long lastButtonPress = 0;
const unsigned long beepDuration = 100;
const unsigned long debounceDelay = 350;

// Time-based beep flags
bool beepEnabled[6] = {false};
const int timeBeepPins[6] = {BUTTON_HOURLY_BEEP_PIN, BUTTON_30MIN_BEEP_PIN, BUTTON_15MIN_BEEP_PIN, BUTTON_10MIN_BEEP_PIN, BUTTON_5MIN_BEEP_PIN, BUTTON_1MIN_BEEP_PIN};
const char* beepNames[6] = {"Hourly", "30-min", "15-min", "10-min", "5-min", "1-min"};
bool lastButtonState[6] = {true, true, true, true, true, true};

volatile bool updateFlag = false;
unsigned long lastUpdateTime = 0;

void rtcInterrupt() {
  updateFlag = true;
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Arduino..."));

  // Initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off
  pinMode(BUTTON_DECREASE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_INCREASE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_HOURLY_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_30MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_15MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_10MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_5MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_1MIN_BEEP_PIN, INPUT_PULLUP);
  pinMode(RTC_SQW_PIN, INPUT_PULLUP);
  Serial.println(F("Default interval: No beep"));

  Wire.begin();

  // Initialize OLED
  Serial.println(F("Initializing OLED..."));
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() == 0) {
    Serial.println(F("OLED found at 0x3C"));
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(0, 20, "Initializing...");
    u8g2.sendBuffer();
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
      rtc.adjust(DateTime(2025, 8, 27, 13, 35, 0)); // Updated to current time
    }
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
    Serial.println(F("RTC initialized with 1Hz SQW."));
  } else {
    Serial.println(F("ERROR: RTC not found!"));
    while (1);
  }

  // Setup interrupt
  attachInterrupt(digitalPinToInterrupt(RTC_SQW_PIN), rtcInterrupt, FALLING);

  // Initial display
  updateTimeAndDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // Manual 1-second update fallback
  if (currentTime - lastUpdateTime >= 1000) {
    updateTimeAndDisplay();
    checkTimeBasedBeeps();
    lastUpdateTime = currentTime;
  }

  // Handle interrupt-driven update
  if (updateFlag) {
    updateFlag = false;
    Serial.println(F("RTC interrupt triggered"));
    updateTimeAndDisplay();
    checkTimeBasedBeeps();
  }

  // Handle sliders
  for (int i = 0; i < 6; i++) {
    bool currentState = digitalRead(timeBeepPins[i]);
    if (currentState != lastButtonState[i] && currentTime - lastButtonPress > debounceDelay) {
      if (currentState == LOW) {
        beepEnabled[i] = true;
        digitalWrite(BUZZER_PIN, HIGH);
        delay(beepDuration);
        digitalWrite(BUZZER_PIN, LOW);
        Serial.print(beepNames[i]);
        Serial.println(F(" beep enabled"));
      } else {
        beepEnabled[i] = false;
        digitalWrite(BUZZER_PIN, HIGH);
        delay(beepDuration);
        digitalWrite(BUZZER_PIN, LOW);
        Serial.print(beepNames[i]);
        Serial.println(F(" beep disabled"));
      }
      lastButtonPress = currentTime;
      lastButtonState[i] = currentState;
      updateTimeAndDisplay();
    }
  }

  // Handle decrease button
  if (digitalRead(BUTTON_DECREASE_PIN) == LOW && currentTime - lastButtonPress > debounceDelay) {
    currentIntervalIndex = (currentIntervalIndex - 1 + numIntervals) % numIntervals;
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Interval set to: "));
    if (currentIntervalIndex == numIntervals - 1) {
      Serial.println(F("No beep"));
    } else {
      Serial.print(intervals[currentIntervalIndex] / 60000);
      Serial.println(F(" minutes (decreased)"));
    }
    lastButtonPress = currentTime;
    lastBeepTime = currentTime;
    updateTimeAndDisplay();
  }

  // Handle increase button
  if (digitalRead(BUTTON_INCREASE_PIN) == LOW && currentTime - lastButtonPress > debounceDelay) {
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Interval set to: "));
    if (currentIntervalIndex == numIntervals - 1) {
      Serial.println(F("No beep"));
    } else {
      Serial.print(intervals[currentIntervalIndex] / 60000);
      Serial.println(F(" minutes (increased)"));
    }
    lastButtonPress = currentTime;
    lastBeepTime = currentTime;
    updateTimeAndDisplay();
  }

  // Interval beep check
  if (currentIntervalIndex != numIntervals - 1 && currentTime - lastBeepTime >= intervals[currentIntervalIndex]) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Interval beep: "));
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(F(" minutes"));
    lastBeepTime = currentTime;
    updateTimeAndDisplay();
  }

  delay(10); // Small delay for loop stability
}

void updateTimeAndDisplay() {
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  dayOfWeek = now.dayOfTheWeek();

  // Debug time
  Serial.print(F("Updating OLED - Time: "));
  Serial.print(hour);
  Serial.print(F(":"));
  if (minute < 10) Serial.print(F("0"));
  Serial.print(minute);
  Serial.print(F(":"));
  if (second < 10) Serial.print(F("0"));
  Serial.println(second);

  // Calculate remaining time
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastBeepTime;
  unsigned long remainingTimeMs = (currentIntervalIndex != numIntervals - 1 && elapsedTime < intervals[currentIntervalIndex]) ? (intervals[currentIntervalIndex] - elapsedTime) : 0;
  unsigned int remainingSeconds = remainingTimeMs / 1000;
  unsigned int remainingMinutes = remainingSeconds / 60;
  unsigned int remainingHours = remainingMinutes / 60;
  remainingMinutes %= 60;
  remainingSeconds %= 60;
  char timerBuf[16];
  sprintf(timerBuf, "Beep: %02d:%02d:%02d", remainingHours, remainingMinutes, remainingSeconds);

  // Format time, date, day
  char timeBuf[12];
  unsigned char displayHour = hour % 12;
  if (displayHour == 0) displayHour = 12;
  const char* ampm = (hour < 12) ? " AM" : " PM";
  sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, minute, second, ampm);
  char dateBuf[6];
  sprintf(dateBuf, "%02d/%02d", day, month);
  char dayBuf[4];
  strcpy_P(dayBuf, (char*)pgm_read_word(&daysOfTheWeek[dayOfWeek]));

  // Display on OLED
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB24_tr);
    u8g2.drawStr(-3, 24, timeBuf);
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 38, dateBuf);
    u8g2.drawStr(81, 38, dayBuf);
    u8g2.drawStr(0, 55, timerBuf);
  } while (u8g2.nextPage());
}

void checkTimeBasedBeeps() {
  bool anyBeepEnabled = false;
  for (int i = 0; i < 6; i++) {
    if (beepEnabled[i]) {
      anyBeepEnabled = true;
      break;
    }
  }
  if (anyBeepEnabled && second == 0 && lastSecond == 59) {
    bool shouldBeep = false;
    if (beepEnabled[0] && minute == 0) {
      shouldBeep = true;
      Serial.println(F("Hourly beep triggered"));
    }
    if (beepEnabled[1] && (minute == 0 || minute == 30)) {
      shouldBeep = true;
      Serial.println(F("30-min beep triggered"));
    }
    if (beepEnabled[2] && (minute % 15 == 0)) {
      shouldBeep = true;
      Serial.println(F("15-min beep triggered"));
    }
    if (beepEnabled[3] && (minute % 10 == 0)) {
      shouldBeep = true;
      Serial.println(F("10-min beep triggered"));
    }
    if (beepEnabled[4] && (minute % 5 == 0)) {
      shouldBeep = true;
      Serial.println(F("5-min beep triggered"));
    }
    if (beepEnabled[5]) {
      shouldBeep = true;
      Serial.println(F("1-min beep triggered"));
    }
    if (shouldBeep) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(beepDuration);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.print(F("Time-based beep at "));
      Serial.print(hour);
      Serial.print(F(":"));
      if (minute < 10) Serial.print(F("0"));
      Serial.print(minute);
      Serial.println(F(":00"));
    }
  }
  lastSecond = second;
}