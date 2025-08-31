#include <Wire.h>
#include <U8g2lib.h>
#include <RP2040_RTC.h> // For Pico's built-in RTC

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

// RTC object
RP2040_RTC rtc;

// Days in each month (with leap year handling)
const unsigned char daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Function to check leap year
bool isLeapYear(unsigned int y) {
  return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
}

// Function to parse __DATE__ and __TIME__ into RTC time
void setRTCFromCompileTime() {
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char monthStr[4] = {0};
  unsigned int day, year, hour, minute, second;
  unsigned char month;

  // Parse __DATE__ (format: "MMM DD YYYY")
  sscanf(__DATE__, "%s %u %u", monthStr, &day, &year);

  // Find month index
  month = 0;
  for (int i = 0; i < 12; i++) {
    if (strcmp(monthStr, months[i]) == 0) {
      month = i + 1;
      break;
    }
  }

  // Parse __TIME__ (format: "HH:MM:SS")
  sscanf(__TIME__, "%u:%u:%u", &hour, &minute, &second);

  // Set RTC (day of week will be calculated separately if needed)
  rtc.setTime(second, minute, hour, day, month, year);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Starting Pico..."));

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
  
  // Initialize and set RTC to compile time
  rtc.begin();
  setRTCFromCompileTime();
}

void loop() {
  // Get current time from RTC
  RTCTime currentTime;
  rtc.getTime(currentTime);
  
  unsigned int year = currentTime.getYear();
  unsigned char month = currentTime.getMonth();
  unsigned char day = currentTime.getDayOfMonth();
  unsigned char hour = currentTime.getHour();
  unsigned char minute = currentTime.getMinutes();
  unsigned char second = currentTime.getSeconds();
  unsigned char dayOfWeek = currentTime.getDayOfWeek(); // 0=Sun, etc.
  
  // Adjust days in February if leap year
  unsigned char febDays = isLeapYear(year) ? 29 : 28;
  // (Note: Not actively used here, but available if you add date rollover logic)

  // Convert to 12-hour format
  char timeBuf[12]; // Extra space for " AM"/" PM"
  unsigned char displayHour = hour % 12;
  if (displayHour == 0) displayHour = 12; // Convert 0 to 12 for 12-hour format
  const char* ampm = (hour < 12) ? " AM" : " PM";
  sprintf(timeBuf, "%02d:%02d:%02d%s", displayHour, minute, second, ampm);
  
  // Format date in M/D format (e.g., 1/1, 10/10)
  char dateBuf[6];
  sprintf(dateBuf, "%d/%d", month, day);
  
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
    u8g2.drawStr(0, 24, timeBuf); // Adjusted Y to fit larger font
    u8g2.setFont(u8g2_font_ncenB12_tr); // Larger font for date and day
    u8g2.drawStr(0, 48, dateBuf); // Adjusted Y for layout
    u8g2.drawStr(80, 48, dayBuf); // Day of week on the right
  } while (u8g2.nextPage());
  
  delay(100); // Small delay to prevent excessive looping
}