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
unsigned char day = 25;
unsigned char hour = 12;
unsigned char minute = 45;
unsigned char second = 0;
unsigned char dayOfWeek = 1; // 0 = Sunday

// Buzzer and button pins
#define BUZZER_PIN 13
#define BUTTON_PIN 2

// Array of intervals in milliseconds: 1 hour, 30 min, 15 min, 10 min, 5 min, 1 min
const unsigned long intervals[] = {3600000, 1800000, 900000, 600000, 300000, 60000};
const int numIntervals = 6; // Number of intervals
int currentIntervalIndex = 0; // Start with 1 hour (index 0)
unsigned long lastBeepTime = 0; // Store the last time the buzzer beeped
unsigned long lastButtonPress = 0; // Store the last button press time for debouncing
const unsigned long beepDuration = 100; // Beep for 0.1 seconds
const unsigned long debounceDelay = 350; // Debounce delay in milliseconds

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Arduino..."));

  // Initialize buzzer and button pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
      rtc.adjust(DateTime(2025, 8, 25, 12, 45, 0));
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

  // Handle button press for interval change
  if (digitalRead(BUTTON_PIN) == LOW && currentTime - lastButtonPress > debounceDelay) {
    currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepDuration);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.print(F("Buzzer beeped for mode change to: "));
    Serial.print(intervals[currentIntervalIndex] / 60000);
    Serial.println(F(" minutes"));
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

  // Calculate remaining time until next beep
  unsigned long elapsedTime = currentTime - lastBeepTime;
  unsigned long remainingTimeMs = (elapsedTime < intervals[currentIntervalIndex]) ? (intervals[currentIntervalIndex] - elapsedTime) : 0;
  unsigned int remainingSeconds = remainingTimeMs / 1000;
  unsigned int remainingMinutes = remainingSeconds / 60;
  unsigned int remainingHours = remainingMinutes / 60;
  remainingMinutes %= 60;
  remainingSeconds %= 60;

  // Format remaining time (e.g., "1h", "59m", "59s")
  char timerBuf[5];
  if (remainingHours >= 1) {
    sprintf(timerBuf, "%dh", remainingHours);
  } else if (remainingMinutes >= 1) {
    sprintf(timerBuf, "%dm", remainingMinutes);
  } else {
    sprintf(timerBuf, "%ds", remainingSeconds);
  }

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
  Serial.println(F("-------------------"));

  // Display on OLED
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB24_tr); // Larger font for time
    u8g2.drawStr(-3, 24, timeBuf);
    u8g2.setFont(u8g2_font_ncenB12_tr); // Smaller font for date, timer, and day
    u8g2.drawStr(-1, 38, dateBuf);
    u8g2.drawStr(47, 38, timerBuf); // Timer in between date and day, adjusted x-coordinate
    u8g2.drawStr(85, 38, dayBuf);
  } while (u8g2.nextPage());

  delay(100);
}