#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FirebaseArduino.h>
BlynkTimer timer;
#define FIREBASE_HOST "smart-city-2883e.firebaseio.com"                   
#define FIREBASE_AUTH "KLPO7nSFcTmK7VHck8gbCZPBa7E9wlTaElDxKPf3"       

const int trigpin=D5;
const int echopin=D6;
const int led=D4;
long duration;
int distancein;//distanceinch;

//int val;
char auth[] = "Y7slPvjoRlHNocRB8ZwAbgSBje-wGqBd";

char ssid[] = "OPPO A5";
char pass[] = "12345678";
int flag=0;

void notifyongarbage()
{
  digitalWrite(trigpin,LOW);
  delay(2);
  digitalWrite(trigpin,HIGH);
  delay(10);
  digitalWrite(trigpin,LOW);
  
  duration=pulseIn(echopin,HIGH);
  distancein=duration*0.034/2;
  //distanceinch=duration*0.0133/2;
  //Serial.println("Distance = "+distancein);
  
  if(distancein<=5 && distancein>0 && flag==0)
  {
    Serial.println("Garbage Full!");
    Blynk.notify("Alert: Your Garbage is full");
    Firebase.setInt("smart_waste/status",1);
    flag=1;
  }
  else if(distancein>5 && flag==1)
  {
    flag=0; 
    Firebase.setInt("smart_waste/status",0);
  }
}
void setup()
{
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  Blynk.begin(auth, ssid, pass);
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.setInt("smart_waste/status",0);
  timer.setInterval(3000L,notifyongarbage);
}

void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
void loop()
{
  if (Firebase.failed())
  {
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }
  Blynk.run();
  timer.run();
  Serial.println("\t"+String(distancein));
  if(distancein<=5 && distancein>0)
  {
    digitalWrite(led,HIGH);
  }
  else
  {
    digitalWrite(led,LOW);
  }
  
}
