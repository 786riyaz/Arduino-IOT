#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc; // or RTC_DS1307 for DS1307 module

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Set the time (uncomment to set, then re-upload with it commented to avoid resetting)
  // Format: rtc.adjust(DateTime(year, month, day, hour, minute, second));
  rtc.adjust(DateTime(2025, 8, 26, 13, 0, 0)); // Set to current date and time
}

void loop() {
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000); // Update every second
}