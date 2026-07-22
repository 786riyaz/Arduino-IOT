#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include<SoftwareSerial.h>

SoftwareSerial s(D6,D5); // RX TX
#define FIREBASE_HOST "smart-city-2883e.firebaseio.com"                   
#define FIREBASE_AUTH "KLPO7nSFcTmK7VHck8gbCZPBa7E9wlTaElDxKPf3"       
#define WIFI_SSID "OPPO A12"                                              
#define WIFI_PASSWORD "12345678"
#define led D4
int slot1,slot2,slot3;
int data = 10000;

void setup() 
{
  s.begin(9600);
  Serial.begin(19200);
  pinMode(led,OUTPUT);
  
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting...");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());
 
  //connecting to firebase 

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.setInt("S1",0);
  Firebase.setInt("S2",0);
  Firebase.setInt("S3",0);     
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
  s.write("s");
  if(s.available() >= 0)
  {
    data = s.read();
    if(data>=0)
    {
      Serial.println(String(data));
      slot3=data%10;
      data=data/10;
      slot2=data%10;
      data=data/10;
      slot1=data;
      
      Firebase.setInt("S1",slot1); 
      //Serial.println("Slot1:"+String(slot1));
      Firebase.setInt("S2",slot2); 
      //Serial.println("Slot2:"+String(slot2));
      Firebase.setInt("S3",slot3); 
      //Serial.println("Slot3:"+String(slot3)); 
      Serial.println();
    }
  }
}
