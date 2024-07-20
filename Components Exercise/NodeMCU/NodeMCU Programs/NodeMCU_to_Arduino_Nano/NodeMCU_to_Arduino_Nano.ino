#include<SoftwareSerial.h>
SoftwareSerial s(13,2); // RX TX

int waterlevel = 100;
void setup()
{ 
  s.begin(9600);
  Serial.begin(19200);
  Serial.println("Start:");
}

void loop()
{
  if(s.available() > 0)
  {
    //char c = s.read();
    s.write(waterlevel);
    //Serial.print("Receive = ");
    //Serial.write(char(c));
    //Serial.println();
    Serial.println("Sent = "+String(waterlevel));
    waterlevel++;
  }
  delay(1000);
}
