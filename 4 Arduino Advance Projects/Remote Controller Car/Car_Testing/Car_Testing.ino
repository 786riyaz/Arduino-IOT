void setup() {
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT); 
}

void loop() {
  
    //FORWARD
      digitalWrite(2,HIGH);
      digitalWrite(12,HIGH);
      delay(3000);
      digitalWrite(2,LOW);
      digitalWrite(12,LOW);
      //REVERSE
      digitalWrite(4,HIGH);
      digitalWrite(13,HIGH);
      delay(3000);
      digitalWrite(4,LOW);
      digitalWrite(13,LOW);
      /*
    //RIGHT
      digitalWrite(12,HIGH);
      delay(1000);
      digitalWrite(12,LOW);
    //LEFT
      digitalWrite(2,HIGH);
      delay(1000);
      digitalWrite(2,LOW);
      delay(3000);*/
    }
  
