
  
const int trigpin0=2;    // For Entry Purpose
const int echopin0=3;    // For Entry Purpose
const int trigpin1=4;    // Slot 1
const int echopin1=5;    // Slot 1
const int trigpin2=6;    // Slot 2
const int echopin2=7;    // Slot 2
const int trigpin3=8;    // Slot 3
const int echopin3=9;    // Slot 3
long duration0,duration1,duration2,duration3;
int distance0,distance1,distance2,distance3;
int slot1=0,slot2=0,slot3=0,maindata=0;

void setup()
{
  Serial.begin(19200);
  pinMode(trigpin0,OUTPUT);
  pinMode(echopin0,INPUT);
  pinMode(trigpin1,OUTPUT);
  pinMode(echopin1,INPUT);
  pinMode(trigpin2,OUTPUT);
  pinMode(echopin2,INPUT);
  pinMode(trigpin3,OUTPUT);
  pinMode(echopin3,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Distance 0
  digitalWrite(trigpin0,LOW);  
  delay(2);
  digitalWrite(trigpin0,HIGH);
  delay(10);
  digitalWrite(trigpin0,LOW);
  duration0=pulseIn(echopin0,HIGH);
  distance0=duration0*0.034/2;
  
  
  //Distance 1
  digitalWrite(trigpin1,LOW);  
  delay(2);
  digitalWrite(trigpin1,HIGH);
  delay(10);
  digitalWrite(trigpin1,LOW);
  duration1=pulseIn(echopin1,HIGH);
  distance1=duration1*0.034/2;
  if(distance1<=5 && distance1>0)
  {slot1=1;}
  else{slot1=0;}
  
  //Distance 2
  digitalWrite(trigpin2,LOW);  
  delay(2);
  digitalWrite(trigpin2,HIGH);
  delay(10);
  digitalWrite(trigpin2,LOW);
  duration2=pulseIn(echopin2,HIGH);
  distance2=duration2*0.034/2;
  if(distance2<=5 && distance2>0)
  {slot2=1;}
  else{slot2=0;}
  
  //Distance 3
  digitalWrite(trigpin3,LOW);  
  delay(2);
  digitalWrite(trigpin3,HIGH);
  delay(10);
  digitalWrite(trigpin3,LOW);
  duration3=pulseIn(echopin3,HIGH);
  distance3=duration3*0.034/2;
  if(distance3<=5 && distance3>0)
  {slot3=1;}
  else{slot3=0;}
 
  Serial.println(String(distance0)+"\t"+(distance1)+"\t"+(distance2)+"\t"+(distance3));
  /*
  lcd.setCursor(0,0);
  lcd.print("Free Slots : "+String(3-(slot1+slot2+slot3)));
  //lcd.print("S1:"+String(slot1)+"  S2:"+String(slot2));
  lcd.setCursor(0,1);
  //lcd.print("S3:"+String(slot3));
  //lcd.setCursor(7,1);
  lcd.print("Occupied Slots:"+String(slot1+slot2+slot3));
  */
  /*if(slot1==1 && slot2==1 && slot3==1)
  {
    lcd.print("Not Allowed");
  }
  else{lcd.print("Allowed");}*/
  maindata=(slot1*100)+(slot2*10)+slot3;
  //Serial.println(maindata);
  /*
  if(s.available() > 0)
  {
    s.write(maindata);
    Serial.println("\t\t\t\t"+String(maindata));
  }*/
  //delay(500);
}
