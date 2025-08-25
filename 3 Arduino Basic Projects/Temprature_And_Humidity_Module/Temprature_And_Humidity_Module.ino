#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// I2C LCD address (commonly 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust 0x27 if needed

// DHT settings
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Serial Monitor for logs
  Serial.begin(9600);
  Serial.println("Initializing system...");

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize DHT sensor
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("--Riyaz Pathan--");
  lcd.setCursor(0, 1);
  lcd.print("Temp & Humidity");
  Serial.println("LCD and DHT initialized.");
  delay(2000);
}

void loop() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  // Error check
  if (isnan(temp) || isnan(humid)) {
    lcd.clear();
    lcd.print("Sensor error");
    Serial.println("Error reading DHT!");
    delay(500*2);
    return;
  }

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print((char)223); // Degree symbol
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humid, 1);
  lcd.print("%");

  // Output to Serial
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println(" %");

  delay(500*4); // Read every 2 seconds
}
