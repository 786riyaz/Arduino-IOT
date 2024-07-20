#include <Servo.h>      
Servo servo_test;

//Ultrasonic Pins
const int trigPin = 10;
const int echoPin = 9;

//LED & Buzzer Pins
const int green = 13;
const int yellow = 11;
const int red = 12;

//Other Variables
long duration;
int distanceCm;
int pos=0;

void setup() {
  servo_test.attach(8);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(red,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  servo_test.write(0);  
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distanceCm= duration*0.034/2;

  digitalWrite(green,HIGH);
  digitalWrite(yellow,LOW);
  digitalWrite(red,LOW);
  
  if(distanceCm<=5 && distanceCm>=3)  {
    //Hands Put
    pos=100;
    servo_test.write(pos); 
    digitalWrite(green,LOW);
    digitalWrite(yellow,HIGH);
    digitalWrite(red,LOW);
    delay(1000);
    
    //Hands Removed
    servo_test.write(0);
    digitalWrite(red,HIGH) ;
    digitalWrite(green,LOW);
    digitalWrite(yellow,LOW);
    delay(1500);
    
    //Preparing
    digitalWrite(yellow,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(red,LOW);
    delay(1500);
  }
}
