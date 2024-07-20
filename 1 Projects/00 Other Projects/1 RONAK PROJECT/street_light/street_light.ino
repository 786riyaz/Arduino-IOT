#include<ESP8266WiFi.h>
#include<FirebaseArduino.h>
#define FIREBASE_HOST "smart-city-2883e.firebaseio.com"                   
#define FIREBASE_AUTH "KLPO7nSFcTmK7VHck8gbCZPBa7E9wlTaElDxKPf3"       
#define WIFI_SSID "vivo 1610"                                              
#define WIFI_PASSWORD "Rb@8347874257"
#define ir1 D1
#define ir2 D2
#define led1 D5
#define led2 D6
void setup() {
  // put your setup code here, to run once:
pinMode(ir1,INPUT);
pinMode(ir2,INPUT);
pinMode(led1,OUTPUT);
pinMode(led2,OUTPUT);
Serial.begin(9600);
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.setInt("street_light/l1",0);
  Firebase.setInt("street_light/l2",0);
}
void firebasereconnect()
{
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
void loop() {
  // put your main code here, to run repeatedly:
if (Firebase.failed())
      {
      Serial.print("setting number failed:");
      Serial.println(Firebase.error());
      firebasereconnect();
      return;
      }
int val1=digitalRead(ir1);
int val2=digitalRead(ir2);
Serial.print("val1=");
Serial.println(val1);
Serial.print("val2=");
Serial.println(val2);
if(val1== LOW){
  analogWrite(led1,255);
  Firebase.setInt("street_light/l1",1); 
  delay(500);
}else{
  analogWrite(led1,50);
  Firebase.setInt("street_light/l1",0);
}
if(val2== LOW){
  analogWrite(led2,255);
  Firebase.setInt("street_light/l2",1);
  delay(500);
}else{
  analogWrite(led2,50);
 Firebase.setInt("street_light/l2",0);
}
}
