#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);// SDA=A4 SCL=A5
int s=0;
void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  //lcd.print("Hello World");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("I Love You Nihal");
  lcd.setCursor(0,1);
  //lcd.print("Details of Slots");
  delay(2000);
  //lcd.clear();
}

void loop()
{
  //lcd.setCursor(0,0);
  //lcd.print("Date : April 6");
  //lcd.setCursor(0,1);
  //lcd.print("S:"+String(s));
  s++;
  delay(1000); 
  //delay(2000);
}
