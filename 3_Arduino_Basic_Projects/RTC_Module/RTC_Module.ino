#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Testing DS3231 communication...");

  Wire.begin();
  Wire.beginTransmission(0x68); // DS3231 I2C address
  Wire.write(0x00); // Start at register 0 (seconds)
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("DS3231 detected!");
    Wire.requestFrom(0x68, 1); // Request 1 byte (seconds)
    if (Wire.available()) {
      byte seconds = Wire.read();
      Serial.print("Seconds register value: ");
      Serial.println(seconds, HEX);
    } else {
      Serial.println("Failed to read from DS3231.");
    }
  } else {
    Serial.print("Failed to communicate with DS3231. Error code: ");
    Serial.println(error);
  }
}

void loop() {}