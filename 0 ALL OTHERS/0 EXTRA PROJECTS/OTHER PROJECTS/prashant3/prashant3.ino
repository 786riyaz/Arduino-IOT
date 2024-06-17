#include <LiquidCrystal.h>
LiquidCrystal lcd(2,3,4,5,6,7); // LCD Connection RS,E,D4,D5,D6,D7

#define inputpin 13
#DEFINE outputpin 12
int inputsignal,turn=0;
int delaytime = 3;//3 second delay
void setup() 
{
  //PinMode
  pinMode(inputpin,INPUT);
  pinMode(outputpin,OUTPUT);

  //Serial Communication
  //Serial.begin(19200);

  //LCD Setup
  lcd.begin(16,2);
  lcd.print("  POWER FACTOR  ");
  lcd.setCursor(0,1);
  lcd.print("   CORRECTION   ");
  delay(2000);
}

void loop() 
{
  inputsignal = digitalRead(13);
  if(inputsignal == 1)
  {
    digitalWrite(outputpin,HIGH);
    if(turn==0)
    {
      //Serial.println("Signal  Detected \t\t PF=6.5");
      lcd.clear();
      lcd.print("  POWER FACTOR  ");
      lcd.setCursor(0,1);
      lcd.print("   VALUE= 6.5   ");
      delay(1000*delaytime);
      turn=1;
    }
    else
    {
      //Serial.println("Signal Detected \t\t PF=9.5");
      lcd.clear();
      lcd.print("  POWER FACTOR  ");
      lcd.setCursor(0,1);
      lcd.print("   VALUE= 6.5   ");
    }
    
    
  }
  else if(inputsignal == 0)
  {
    digitalWrite(outputpin,LOW);
    Serial.println("No Signal \t\t PF=9.6");
    lcd.clear();
    lcd.print("  POWER FACTOR  ");
    lcd.setCursor(0,1);
    lcd.print("   VALUE= 9.6   ");
    turn=0;
  }
  delay(1000);
}
