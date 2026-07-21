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

// Clock variables (will be updated from RTC)
unsigned int year = 2025;
unsigned char month = 8;
unsigned char day = 25;
unsigned char hour = 22;
unsigned char minute = 0;
unsigned char second = 0;
unsigned char dayOfWeek = 1; // 0 = Sunday

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
  
  // Initialize RTC
  Serial.println(F("Initializing RTC..."));
  Wire.beginTransmission(0x68); // DS3231 I2C address
  if (Wire.endTransmission() == 0) {
    Serial.println(F("RTC found at 0x68"));
    if (!rtc.begin()) {
      Serial.println(F("ERROR: Couldn't initialize RTC!"));
      while (1);
    }
    if (rtc.lostPower()) {
      Serial.println(F("RTC lost power, setting the time!"));
      // Set the RTC to the date & time this sketch was compiled (comment out after first run if not needed)
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

      // Or set to a specific date/time (e.g., August 25, 2025, 12:45:00)
      // rtc.adjust(DateTime(year, month, day, hour, minute, second));
    }
    Serial.println(F("RTC initialized."));
  } else {
    Serial.println(F("ERROR: RTC not found!"));
    while (1);
  }
}

void loop() {
  // Read current time from RTC
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  dayOfWeek = now.dayOfTheWeek(); // 0 = Sunday, 1 = Monday, etc.
  
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