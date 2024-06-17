void setup() {
  // put your setup code here, to run once:
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(13,OUTPUT);
}
void allLow()
{
  digitalWrite(1,LOW);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
  digitalWrite(13,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  allLow();
  //000000000000000000
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  delay(1000);
  allLow(); 
  //111111111111111111
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  delay(1000);
  allLow(); 
  //222222222222222222
  digitalWrite(1,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow();
  //3333333333333333333
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  //444444444444444444
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  //digitalWrite(3,HIGH);
  //digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  //digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  //5555555555555555555555
  //digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  //digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  //6666666666666666666
  //digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 

  //6666666666666666666
  //digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 

  //6666666666666666666
  //digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  
  //77777777777777777777
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(6,HIGH);
  //digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  
  //888888888888
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 
  
  //9999999999999999
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  //digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(1000);
  allLow(); 





}
