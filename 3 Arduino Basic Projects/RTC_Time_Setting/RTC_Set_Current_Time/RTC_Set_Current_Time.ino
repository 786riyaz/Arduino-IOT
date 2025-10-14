#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc; // Initialize RTC object (use RTC_DS1307 for DS1307 module)

const int minutesToAdd = 5; // Configurable variable to add minutes (e.g., 5 or 10)

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C communication (A4: SDA, A5: SCL)

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // Halt if RTC not found
  }

  // Check if RTC lost power and needs time set
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
    // Get current time from system (compilation time)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Get current time
  DateTime now = rtc.now();

  // Add configurable minutes to current time
  DateTime adjustedTime = now + TimeSpan(0, 0, minutesToAdd, 0); // Add minutesToAdd minutes

  // Set RTC to adjusted time
  rtc.adjust(adjustedTime);

  // Print confirmation
  Serial.print("Time set to: ");
  printDateTime(adjustedTime);
}

void loop() {
  // Print current RTC time every second
  DateTime now = rtc.now();
  printDateTime(now);
  delay(1000);
}

// Helper function to print date and time
void printDateTime(DateTime dt) {
  Serial.print(dt.year(), DEC);
  Serial.print('/');
  Serial.print(dt.month(), DEC);
  Serial.print('/');
  Serial.print(dt.day(), DEC);
  Serial.print(" ");
  Serial.print(dt.hour(), DEC);
  Serial.print(':');
  Serial.print(dt.minute(), DEC);
  Serial.print(':');
  Serial.print(dt.second(), DEC);
  Serial.println();
}