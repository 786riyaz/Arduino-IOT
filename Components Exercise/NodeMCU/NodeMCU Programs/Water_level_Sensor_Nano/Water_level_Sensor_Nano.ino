#include<SoftwareSerial.h>
SoftwareSerial s(13,2); // RX TX

int waterlevel = 0;
int level_10 = 3;
int level_20 = 4;
int level_30 = 5;
int level_40 = 6;
int level_50 = 7;/*
int level_60 = 8;
int level_70 = 9;
int level_80 = 10;
int level_90 = 11;
int level_100 =12;*/

void setup()
{ 
  s.begin(9600);
  Serial.begin(19200);
  pinMode(level_10, INPUT);
  pinMode(level_20, INPUT);
  pinMode(level_30, INPUT);
  pinMode(level_40, INPUT);
  pinMode(level_50, INPUT);
 /* pinMode(level_60, INPUT);
  pinMode(level_70, INPUT);
  pinMode(level_80, INPUT);
  pinMode(level_90, INPUT);
  pinMode(level_100,INPUT);*/
}

void loop()
{  
  int L10 = digitalRead(level_10);
  int L20 = digitalRead(level_20);
  int L30 = digitalRead(level_30);
  int L40 = digitalRead(level_40);
  int L50 = digitalRead(level_50);
  /*int L60 = digitalRead(level_60);
  int L70 = digitalRead(level_70);
  int L80 = digitalRead(level_80);
  int L90 = digitalRead(level_90);
  int L100= digitalRead(level_100);*/
/*
  Serial.print("2="+String(L10));
  Serial.print(" 3="+String(L20));
  Serial.print(" 4="+String(L30));
  Serial.print(" 5="+String(L40));
  Serial.print(" 7="+String(L50));
  Serial.print(" 8="+String(L60));
  Serial.print(" 9="+String(L70));
  Serial.print(" 10="+String(L80));
  Serial.print(" 11="+String(L90));
  Serial.print(" 12="+String(L100));
  Serial.println();
*/

  if (L10 == HIGH)
  {
    waterlevel = 1;
    if (L20 == HIGH)
    {
      waterlevel = 2;
      if (L30 == HIGH)
      {
        waterlevel = 3;
        if (L40 == HIGH)
        {
          waterlevel = 4;
          if (L50 == HIGH)
          {
            waterlevel = 5;
           /* if (L60 == HIGH)
            {
              waterlevel = 60;
              if (L70 == HIGH)
              {
                waterlevel = 70;
                if (L80 == HIGH)
                {
                  waterlevel = 80;
                  if (L90 == HIGH)
                  {
                    waterlevel = 90;
                    if (L100 == HIGH)
                    {
                      waterlevel = 100;
                    }
                  }
                }
              }
            }*/
          }
        }
      }
    }
  }
  else
  {
    waterlevel = 0;
    //Serial.println("level = 0");
  }
  //s.write("s");
  if(s.available() > 0)
  {
    //char c = s.read();
    s.write(waterlevel);
    //Serial.println("\t\t\tSent");
    //Serial.println("Level = "+String(waterlevel));
    Serial.println(String(waterlevel));
  }
  delay(1000*2);
  //Serial.println("Level = "+String(waterlevel));
}
