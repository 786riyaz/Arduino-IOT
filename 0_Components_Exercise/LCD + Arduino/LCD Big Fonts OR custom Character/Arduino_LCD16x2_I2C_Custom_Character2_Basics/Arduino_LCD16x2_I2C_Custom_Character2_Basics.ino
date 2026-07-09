#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA=A4 SCL=A5
//you can also start with 0
byte customChar1[] = {  B11111,  B11111,  B11111,  B00001,  B00001,  B00001,  B00001,  B00001};
byte customChar2[] = {  B11111,  B11111,  B11111,  B10000,  B10000,  B10000,  B10000,  B10000};
byte customChar3[] = {  B00001,  B00001,  B00001,  B00001,  B00001,  B11111,  B11111,  B11111};
byte customChar4[] = {  B10000,  B10000,  B10000,  B10000,  B10000,  B11111,  B11111,  B11111};
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  //you can also start with 0
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  lcd.createChar(4, customChar4);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(1, 0);
  lcd.write(2);
  lcd.setCursor(0, 1);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
}

// the loop function runs over and over again forever
void loop() {
}
