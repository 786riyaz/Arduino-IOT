#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);

int s=0,m=0,h=0;
void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  lcd.print("Timing");
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,1);
  //lcd.print(millis()/1000);
  s++;
  if(s>=60)
  {
    m++;
    s=0;
    if(h>=60)
    {
      h++;
      m=0;
    }
  }
  lcd.print("H:"+String(h)+" M:"+String(m)+" S:"+String(s));
  //lcd.print("     Riyaz     "); 
  delay(1000);
}
