#include <ESP8266WiFi.h>
#include<Servo.h>
#include <FirebaseArduino.h>

Servo motor;
int angle = 0;
#define FIREBASE_HOST "smart-city-2883e.firebaseio.com"
#define FIREBASE_AUTH "KLPO7nSFcTmK7VHck8gbCZPBa7E9wlTaElDxKPf3"
#define WIFI_SSID "vivo 1610"
#define WIFI_PASSWORD "rb@8347874257"
#define led1 D4
#define led2 D2
#define led3 D3
const int trigpin = D6;
const int echopin = D5;
const int trigpin2 = D1; //for entry purpose ultrasonic
const int echopin2 = D0; //for entry purpose ultrasonic
long duration;
int distance, distanceinch;
long duration2;    //for entry purpose ultrasonic
int distance2;     //for entry purpose ultrasonic
int val1, val2, val3;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  motor.attach(D8);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(trigpin2, OUTPUT);
  pinMode(echopin2, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led2, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setInt("S1", 0);
  Firebase.setInt("S2", 0);
  Firebase.setInt("S3", 0);
}
void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if (Firebase.failed())
  {
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }


  digitalWrite(trigpin2, LOW);
  delay(2);
  digitalWrite(trigpin2, HIGH);
  delay(10);
  digitalWrite(trigpin2, LOW);

  duration2 = pulseIn(echopin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  Serial.println(distance2);
  val3 = Firebase.getInt("S3");
  if (distance2 <= 6 && val3 == 0) {

    for (angle = 105; angle >= 0; angle -= 5) {
      motor.write(angle);
      delay(5);

    }
    delay(3000);
  } else {
    for (angle = 0; angle < 105; angle += 5) {
      motor.write(angle);
      delay(5);
    }
  }
  digitalWrite(trigpin, LOW);
  delay(2);
  digitalWrite(trigpin, HIGH);
  delay(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echopin, HIGH);
  distance = duration * 0.034 / 2;
  distanceinch = duration * 0.0133 / 2;
  Serial.println(distance);
  if (distance < 28 && distance >= 20) {
    Firebase.setInt("S1", 1);
  } else {
    Firebase.setInt("S1", 0);
  }
  if (distance < 20 && distance >= 12) {
    Firebase.setInt("S2", 1);
  } else {
    Firebase.setInt("S2", 0);
  }
  if (distance < 12 && distance >= 4) {
    Firebase.setInt("S3", 1);
  } else {
    Firebase.setInt("S3", 0);
  }

  val1 = Firebase.getInt("S1");
  val2 = Firebase.getInt("S2");
  val3 = Firebase.getInt("S3");
  Serial.print("Val1:");
  Serial.println(val1);
  Serial.print("Val2:");
  Serial.println(val2);
  Serial.print("Val3:");
  Serial.println(val3);
  if (val1 == 1)
  {
    digitalWrite(led1, HIGH);
    Serial.println("light 1 ON");
    delay(2);
  }
  else if (val1 == 0)
  {
    digitalWrite(led1, LOW);
    Serial.println("light 1 OFF");
    delay(2);
  }
  if (val2 == 1)
  {
    digitalWrite(led2, HIGH);
    Serial.println("light 1 ON");
    delay(2);
  }
  else if (val2 == 0)
  {
    digitalWrite(led2, LOW);
    Serial.println("light 1 OFF");
    delay(2);
  }
  if (val3 == 1)
  {
    digitalWrite(led3, HIGH);
    Serial.println("light 1 ON");
    delay(2);
  }
  else if (val3 == 0)
  {
    digitalWrite(led3, LOW);
    Serial.println("light 1 OFF");
    delay(2);
  }


}
