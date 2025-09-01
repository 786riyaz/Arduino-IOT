#include <Wire.h>
#include <RTClib.h>
#include <U8g2lib.h>

// Initialize OLED with page mode (I2C address 0x3C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Initialize RTC
RTC_DS3231 rtc;
bool rtcOK = false;

// Days of the week in PROGMEM
const char daysOfTheWeek0[] PROGMEM = "Sun";
const char daysOfTheWeek1[] PROGMEM = "Mon";
const char daysOfTheWeek2[] PROGMEM = "Tue";
const char daysOfTheWeek3[] PROGMEM = "Wed";
const char daysOfTheWeek4[] PROGMEM = "Thu";
const char daysOfTheWeek5[] PROGMEM = "Fri";
const char daysOfTheWeek6[] PROGMEM = "Sat";
const char* const daysOfTheWeek[] PROGMEM = {daysOfTheWeek0, daysOfTheWeek1, daysOfTheWeek2, daysOfTheWeek3, daysOfTheWeek4, daysOfTheWeek5, daysOfTheWeek6};

// Software clock fallback (start at 4:36 PM IST, August 24, 2025, Sunday)
unsigned long startMillis = 0;
unsigned long seconds = 0;
unsigned int year = 2025;
unsigned char month = 8;
unsigned char day = 24;
unsigned char hour = 16; // 4:36 PM in 24-hour format
unsigned char minute = 36;
unsigned char second = 0;
unsigned char dayOfWeek = 0; // 0 = Sunday
const unsigned char daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Arduino..."));

  Wire.begin();
  
  // Initialize RTC
  Serial.println(F("Initializing RTC..."));
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0 && rtc.begin()) {
    Serial.println(F("RTC found at 0x68"));
    rtcOK = true;
    // Set RTC to 4:36 PM IST, August 24, 2025
    rtc.adjust(DateTime(2025, 8, 24, 16, 36, 0));
    Serial.println(F("RTC set to 4:36 PM, August 24, 2025"));
  } else {
    Serial.println(F("ERROR: RTC not found! Using software clock."));
  }
  
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
  
  startMillis = millis(); // Record start time for software clock
}

void loop() {
  char timeBuf[12];
  char dateBuf[11];
  char dayBuf[4];
  
  if (rtcOK) {
    // Use RTC
    DateTime now = rtc.now();
    unsigned char displayHour = now.hour() % 12;
    if (displayHour == 0) displayHour = 12;
    const char* ampm = (now.hour() < 12) ? " AM" : " PM";
    sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, now.minute(), now.second(), ampm);
    sprintf(dateBuf, "%02d/%02d/%04d", now.month(), now.day(), now.year());
    strcpy_P(dayBuf, (char*)pgm_read_word(&daysOfTheWeek[now.dayOfTheWeek()]));
  } else {
    // Use software clock
    unsigned long currentMillis = millis();
    if (currentMillis - startMillis >= 1000) {
      seconds++;
      startMillis += 1000;
      second++;
      if (second >= 60) {
        second = 0;
        minute++;
        if (minute >= 60) {
          minute = 0;
          hour++;
          if (hour >= 24) {
            hour = 0;
            day++;
            dayOfWeek = (dayOfWeek + 1) % 7;
            if (day > daysInMonth[month - 1]) {
              day = 1;
              month++;
              if (month > 12) {
                month = 1;
                year++;
              }
            }
          }
        }
      }
    }
    unsigned char displayHour = hour % 12;
    if (displayHour == 0) displayHour = 12;
    const char* ampm = (hour < 12) ? " AM" : " PM";
    sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, minute, second, ampm);
    sprintf(dateBuf, "%02d/%02d/%04d", month, day, year);
    strcpy_P(dayBuf, (char*)pgm_read_word(&daysOfTheWeek[dayOfWeek]));
  }
  
  // Print to Serial Monitor
  Serial.println(F("--- Data Update ---"));
  Serial.print(F("Time: "));
  Serial.println(timeBuf);
  Serial.print(F("Date: "));
  Serial.println(dateBuf);
  Serial.print(F("Day: "));
  Serial.println(dayBuf);
  Serial.println(F("-------------------"));
  
  // Display on OLED
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, timeBuf);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 40, dateBuf);
    u8g2.drawStr(80, 40, dayBuf);
  } while (u8g2.nextPage());
  
  delay(100);
}