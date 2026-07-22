#include<SoftwareSerial.h> 
SoftwareSerial s(3,1);
const int trigpin=D0;   
const int echopin=D1;

const int trigpin1=D6;   
const int echopin1=D5; 
   
long duration,duration1;
int distance,distance1;
int slot1,slot2,slots;
void setup() 
{
  //Serial S Begin at 9600 Baud
  s.begin(9600);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  pinMode(trigpin1,OUTPUT);
  pinMode(echopin1,INPUT);  
}

void loop() 
{
  digitalWrite(trigpin,LOW);
  delay(2);
  digitalWrite(trigpin,HIGH);
  delay(10);
  digitalWrite(trigpin,LOW);

  duration=pulseIn(echopin,HIGH);
  distance=duration*0.034/2;

  
  digitalWrite(trigpin1,LOW);
  delay(2);
  digitalWrite(trigpin1,HIGH);
  delay(10);
  digitalWrite(trigpin1,LOW);

  duration1=pulseIn(echopin1,HIGH);
  distance1=duration1*0.034/2;

  if(distance<=5)
  {slot1=1;}
  else
  {slot1=0;}
  if(distance1<=5)
  {slot2=1;}
  else
  {slot2=0;}
  
  slots=slot1*10+slot2;
  s.write(slots);
  delay(1000);
}
