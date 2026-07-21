#include <Wire.h>
#include <U8g2lib.h>

// Initialize OLED with page mode (I2C address 0x3C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Days of the week in PROGMEM (uppercase)
const char daysOfTheWeek0[] PROGMEM = "SUN";
const char daysOfTheWeek1[] PROGMEM = "MON";
const char daysOfTheWeek2[] PROGMEM = "TUE";
const char daysOfTheWeek3[] PROGMEM = "WED";
const char daysOfTheWeek4[] PROGMEM = "THU";
const char daysOfTheWeek5[] PROGMEM = "FRI";
const char daysOfTheWeek6[] PROGMEM = "SAT";
const char* const daysOfTheWeek[] PROGMEM = {daysOfTheWeek0, daysOfTheWeek1, daysOfTheWeek2, daysOfTheWeek3, daysOfTheWeek4, daysOfTheWeek5, daysOfTheWeek6};

// Clock variables (start at 4:27 PM IST, August 24, 2025, Sunday)
unsigned long startMillis = 0;
unsigned long seconds = 0;
unsigned int year = 2025;
unsigned char month = 8;
unsigned char day = 25;
unsigned char hour = 12;
unsigned char minute = 45;
unsigned char second = 0;
unsigned char dayOfWeek = 1; // 0 = Sunday, August 24, 2025

// Days in each month (non-leap year for 2025)
const unsigned char daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Arduino..."));

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
  
  startMillis = millis(); // Record start time
}

void loop() {
  // Update time based on millis()
  unsigned long currentMillis = millis();
  if (currentMillis - startMillis >= 1000) {
    seconds++;
    startMillis += 1000; // Increment by 1 second
    
    // Update time
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
          dayOfWeek = (dayOfWeek + 1) % 7; // Update day of week
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
  
  // Convert to 12-hour format
  char timeBuf[12]; // Extra space for " AM"/" PM"
  unsigned char displayHour = hour % 12;
  if (displayHour == 0) displayHour = 12; // Convert 0 to 12 for 12-hour format
  const char* ampm = (hour < 12) ? " AM" : " PM";
  sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, minute, second, ampm);
  
  // Format date in DD/MM format (e.g., 24/08)
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
  Serial.println(F("-------------------"));
  
  // Display on OLED with larger fonts
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB24_tr); // Larger font for time
    u8g2.drawStr(-2, 24, timeBuf); // Adjusted Y to fit larger font
    u8g2.setFont(u8g2_font_ncenB12_tr); // Larger font for date and day
    u8g2.drawStr(0, 48, dateBuf); // Adjusted Y for layout
    u8g2.drawStr(80, 48, dayBuf); // Day of week on the right
  } while (u8g2.nextPage());
  
  delay(100); // Small delay to prevent excessive looping
}