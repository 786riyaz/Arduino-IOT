#include<SoftwareSerial.h>
SoftwareSerial s(3,1);
const int trigpin1=D8;   
const int echopin1=D7;
const int trigpin2=D6;   
const int echopin2=D5;
const int trigpin3=D0;   
const int echopin3=D1;
const int trigpin4=D2;   
const int echopin4=D3;

long duration1,duration2,duration3,duration4;
int distance1,distance2,distance3,distance4;
int slot1,slot2,slot3,slots;

void setup() {
  // put your setup code here, to run once:
  s.begin(9600);
  Serial.begin(19200);
  Serial.println("Start");
  pinMode(trigpin1,OUTPUT);
  pinMode(echopin1,INPUT);
  pinMode(trigpin2,OUTPUT);
  pinMode(echopin2,INPUT);
  pinMode(trigpin3,OUTPUT);
  pinMode(echopin3,INPUT);
  pinMode(trigpin4,OUTPUT);
  pinMode(echopin4,INPUT);

}

void loop() {
  // distance 1
  digitalWrite(trigpin1,LOW);
  delay(2);
  digitalWrite(trigpin1,HIGH);
  delay(10);
  digitalWrite(trigpin1,LOW);

  duration1=pulseIn(echopin1,HIGH);
  distance1=duration1*0.034/2;
  
  // distance 2
  digitalWrite(trigpin2,LOW);
  delay(2);
  digitalWrite(trigpin2,HIGH);
  delay(10);
  digitalWrite(trigpin2,LOW);

  duration2=pulseIn(echopin2,HIGH);
  distance2=duration2*0.034/2;

  // distance 3
  digitalWrite(trigpin3,LOW);
  delay(2);
  digitalWrite(trigpin3,HIGH);
  delay(10);
  digitalWrite(trigpin3,LOW);

  duration3=pulseIn(echopin3,HIGH);
  distance3=duration3*0.034/2;

  // distance 4
  digitalWrite(trigpin4,LOW);
  delay(2);
  digitalWrite(trigpin4,HIGH);
  delay(10);
  digitalWrite(trigpin4,LOW);

  duration4=pulseIn(echopin4,HIGH);
  distance4=duration4*0.034/2;
  //--------------------------------------
  if(distance1>0 && distance1<=5)
  {slot1=1;}
  else
  {slot1=0;}

  if(distance2>0 && distance2<=5)
  {slot2=1;}
  else
  {slot2=0;}

  if(distance3>0 && distance3<=5)
  {slot3=1;}
  else
  {slot3=0;}

  slots=slot1*100+slot2*10+slot3;
  //s.write(slots);

  //-------------------------------------
  Serial.print("\t");
  Serial.print(distance1);
  Serial.print("\t");
  Serial.print(distance2);
  Serial.print("\t");
  Serial.print(distance3);
  Serial.print("\t");
  Serial.print(distance4);
  Serial.print("\n");
  Serial.println("------------>"+String(slots));
  delay(1000);
}
