#include <ESP8266WiFi.h>
#include<SoftwareSerial.h> 
SoftwareSerial s(3,1);
#define TRIGGERPIN1 D8
#define ECHOPIN1    D7
#define TRIGGERPIN2 D6
#define ECHOPIN2    D5
#define TRIGGERPIN3 D4
#define ECHOPIN3    D3
//#define TRIGGERPIN4 D2
//#define ECHOPIN4    D1
long duration1, distance1,duration2, distance2,duration3, distance3,duration4, distance4;

void setup()
{
  // Debug console
  Serial.begin(19200);
  s.begin(9600);
  pinMode(TRIGGERPIN1, OUTPUT);
  pinMode(ECHOPIN1, INPUT);
  pinMode(TRIGGERPIN2, OUTPUT);
  pinMode(ECHOPIN2, INPUT);
  pinMode(TRIGGERPIN3, OUTPUT);
  pinMode(ECHOPIN3, INPUT);
 
}

void loop()
{
  //--------Distance 1
  digitalWrite(TRIGGERPIN1, LOW);  
  delayMicroseconds(3); 
  digitalWrite(TRIGGERPIN1, HIGH);
  delayMicroseconds(12); 
  
  digitalWrite(TRIGGERPIN1, LOW);
  duration1 = pulseIn(ECHOPIN1, HIGH);
  distance1 = (duration1/2) / 29.1;

  //--------Distance 2
  digitalWrite(TRIGGERPIN2, LOW);  
  delayMicroseconds(3); 
  digitalWrite(TRIGGERPIN2, HIGH);
  delayMicroseconds(12); 
  
  digitalWrite(TRIGGERPIN2, LOW);
  duration2 = pulseIn(ECHOPIN2, HIGH);
  distance2 = (duration2/2) / 29.1;
  
  //--------Distance 3
  digitalWrite(TRIGGERPIN3, LOW);  
  delayMicroseconds(3); 
  digitalWrite(TRIGGERPIN3, HIGH);
  delayMicroseconds(12); 
  
  digitalWrite(TRIGGERPIN3, LOW);
  duration3 = pulseIn(ECHOPIN3, HIGH);
  distance3 = (duration3/2) / 29.1;
  
  //--------Distance 4
  /*digitalWrite(TRIGGERPIN4, LOW);  
  delayMicroseconds(3); 
  digitalWrite(TRIGGERPIN4, HIGH);
  delayMicroseconds(12); 
  
  digitalWrite(TRIGGERPIN4, LOW);
  duration4 = pulseIn(ECHOPIN4, HIGH);
  distance4 = (duration4/2) / 29.1;
  //----------------------------------------------
  */
  Serial.print(distance1);
  Serial.print("\t");
  Serial.print(distance2);
  Serial.print("\t");
  Serial.print(distance3);
  Serial.print("\t");
  Serial.print(distance4);
  Serial.println();
  s.write(distance1);
  //delay(200*5);
}
