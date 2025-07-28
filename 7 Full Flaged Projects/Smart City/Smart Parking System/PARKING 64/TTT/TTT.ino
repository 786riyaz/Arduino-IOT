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
  Serial.begin(9600);
  Serial.println("Start");
  pinMode(trigpin1,OUTPUT);
  pinMode(echopin1,INPUT);

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
  
  Serial.print("\t");
  Serial.print(distance1);
  
  Serial.print("\n");
}
