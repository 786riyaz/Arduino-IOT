#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA=A4 SCL=A5

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  INITIALIZING  ");
  lcd.setCursor(0, 1);
  lcd.print("     SYSTEM     ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   RIYAZ KHAN  ");
  lcd.setCursor(0, 1);
  lcd.print("  TEST PATTERN  ");
  delay(1000);
  //lcd.clear();

}

// the loop function runs over and over again forever
void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   TESTING...   ");
  lcd.setCursor(0, 1);
  lcd.print("*");
  //lcd.print("ABCD        1234");
  for (int i = 1; i < 16; i++)
  {
    lcd.setCursor(i, 1);
    lcd.print("*");
    delay(50);
  }
}