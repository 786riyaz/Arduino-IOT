#include<SoftwareSerial.h>
#include<Servo.h>
#include <LiquidCrystal.h>
SoftwareSerial s(13,2); // RX TX

LiquidCrystal lcd();   //lcd ke pin number function me dal dena

Servo motor;
int angle=0;
const int trigpin1=2;    // For Entry Purpose
const int echopin1=3;    // For Entry Purpose
const int trigpin2=4;    // For Slot 1
const int echopin2=5;    // For Slot 1
const int trigpin3=6;    // For Slot 2
const int echopin3=7;    // For Slot 2
const int trigpin4=8;    // For Slot 3
const int echopin4=9;    // For Slot 3

long duration1,duration2,duration3,duration4;
int distance1,distance2,distance3,distance4;

int slot1=0,slot2=0,slot3=0,slots=0;
int parking_status=0;



void setup()
{ 
  s.begin(9600);
  motor.attach(13);
  Serial.begin(19200);
  pinMode(trigpin0,OUTPUT);
  pinMode(echopin0,INPUT);
  pinMode(trigpin1,OUTPUT);
  pinMode(echopin1,INPUT);
  pinMode(trigpin2,OUTPUT);
  pinMode(echopin2,INPUT);
  pinMode(trigpin3,OUTPUT);
  pinMode(echopin3,INPUT);
  lcd.begin(16, 2);
}

void loop()
{  

  digitalWrite(trigpin0,LOW);  
  delay(2);
  digitalWrite(trigpin0,HIGH);
  delay(10);
  digitalWrite(trigpin0,LOW);

  duration1=pulseIn(echopin0,HIGH);
  distance1=duration*0.0133/2;

  if(slot1==1 && slot2==1 && slot3==1){
    parking_status=1;
    lcd.setCursor(0, 1);
    lcd.print("SORRY !");
    lcd.setCursor(0, 2);
    lcd.print("Slots Full!");
    
  }else{
    parking_status=0;
    lcd.setCursor(0, 1);
    lcd.print("WELCOME !");
    lcd.setCursor(0, 2);
    lcd.print("Slots Available!");
  }
 
  if(distance1<=5 && parking_status==0){
    
    for(angle=105;angle>=0;angle-=5){
    motor.write(angle);
    delay(5);
    delay(3000);  
  }
  }else{
    for(angle=0;angle<105;angle+=5){
    motor.write(angle);
    delay(5);
  }
  }

  digitalWrite(trigpin1,LOW);  
  delay(2);
  digitalWrite(trigpin1,HIGH);
  delay(10);
  digitalWrite(trigpin1,LOW);

  duration2=pulseIn(echopin1,HIGH);
  distance2=duration2*0.0133/2;

  digitalWrite(trigpin2,LOW);  
  delay(2);
  digitalWrite(trigpin2,HIGH);
  delay(10);
  digitalWrite(trigpin2,LOW);

  duration3=pulseIn(echopin2,HIGH);
  distance3=duration3*0.0133/2;

  digitalWrite(trigpin3,LOW);  
  delay(2);
  digitalWrite(trigpin3,HIGH);
  delay(10);
  digitalWrite(trigpin3,LOW);

  duration4=pulseIn(echopin2,HIGH);
  distance4=duration4*0.0133/2;

 if(distance2<=5){
  slot1=1;
 }else{
  slot1=0;
 }
 if(distance3<=5){
  slot2=1;
 }else{
  slot2=0;
 }
 if(distance4<=5){
  slot3=1;
 }else{
  slot3=0;
 }
 slots=(slot*100)+(slot2*10)+slot3;
  if(s.available() > 0)
  {
    
    s.write(slots);
    Serial.println(String(slots));
  }
  delay(1000*2);
  
}
