#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define ANALOG_PIN A0
#define R_REF 980.0   // Reference resistor value in ohms
#define V_IN 4.53     // Supply voltage (measured)

void setup() {
  // Initialize Serial
  Serial.begin(9600);
  delay(100); // Wait for Serial to stabilize
  Serial.println(F("Ohmmeter Start"));
  Serial.print(F("R_REF="));
  Serial.print(R_REF);
  Serial.print(F("Ohm, V_IN="));
  Serial.print(V_IN);
  Serial.println(F("V"));

  // Initialize I2C
  Wire.begin();
  Serial.println(F("I2C Initialized"));

  // Test I2C communication (default SH1106 address: 0x3C)
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() == 0) {
    Serial.println(F("I2C Device Found at 0x3C"));
  } else {
    Serial.println(F("I2C Device NOT Found at 0x3C"));
  }

  // Initialize OLED
  if (u8g2.begin()) {
    u8g2.setContrast(128);
    Serial.println(F("OLED Initialized Successfully"));
  } else {
    Serial.println(F("OLED Initialization Failed"));
  }

  // Test pattern
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(15,20,"Initiating OLED");
  } while (u8g2.nextPage());
  Serial.println(F("OLED Test Pattern Sent"));
  delay(3000);
}

void loop() {
  // Read ADC
  long sum = 0;
  int samples = 20;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(ANALOG_PIN);
    delay(10);
  }
  float adc_value = sum / (float)samples;
  Serial.print(F("ADC="));
  Serial.println(adc_value);

  // Calculate voltage
  float voltage = (adc_value / 1023.0) * V_IN;
  Serial.print(F("V="));
  Serial.print(voltage);
  Serial.println(F("V"));

  // Calculate resistance
  float resistance = 0.0;
  if (voltage > 0.05 && voltage < V_IN - 0.05) {
    float ratio = voltage / (V_IN - voltage);
    resistance = R_REF * ratio;
    Serial.print(F("Ratio="));
    Serial.print(ratio);
    Serial.print(F(", R="));
    Serial.print(resistance);
    Serial.println(F("Ohm"));
  } else {
    resistance = -1;
    Serial.println(F("Open/Invalid"));
  }

  // Update OLED
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_7x14_tr);
    int titleWidth = u8g2.getStrWidth("Ohm Meter");
    u8g2.drawStr((128 - titleWidth) / 2,20,"Ohm Meter");
    if (resistance >= 0 && resistance < 1000000) {
      u8g2.setCursor((128 - u8g2.getStrWidth("216.0 Ohm")) / 2,50); // Approximate width for centering
      if (resistance >= 1000) {
        u8g2.print(resistance / 1000.0, 1);
        u8g2.print(" kOhm");
        Serial.print(F("OLED Rendering: "));
        Serial.print(resistance / 1000.0, 1);
        Serial.println(F(" kOhm"));
      } else {
        u8g2.print(resistance, 1);
        u8g2.print(" Ohm");
        Serial.print(F("OLED Rendering: "));
        Serial.print(resistance, 1);
        Serial.println(F(" Ohm"));
      }
    } else {
      u8g2.setCursor((128 - u8g2.getStrWidth("--Empty--")) / 2,50);
      u8g2.print("--EMPTY--");
      Serial.println(F("OLED Rendering: Empty"));
    }
  } while (u8g2.nextPage());
  Serial.println(F("OLED Updated"));
  Serial.println(F("---"));
  delay(500);
}