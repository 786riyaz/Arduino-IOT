int i,j;
void setup()
{
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);  
}

void loop()
{
 for(i=0;i<5;i++)
 {    
    digitalWrite(13,HIGH);
    delay(50);
    digitalWrite(13,LOW);
    //delay(100);
    
    digitalWrite(12,HIGH);
    delay(50);
    digitalWrite(12,LOW);
    //delay(100);
   
    digitalWrite(11,HIGH);
    delay(50);
    digitalWrite(11,LOW);
    //delay(100);
    
    digitalWrite(10,HIGH);
    delay(50);
    digitalWrite(10,LOW);
    //delay(100);
    
    digitalWrite(9,HIGH);
    delay(50);
    digitalWrite(9,LOW);
    //delay(100);
    
    digitalWrite(8,HIGH);
    delay(50);
    digitalWrite(8,LOW);
    //delay(100);
    
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    //delay(100);
    
    digitalWrite(6,HIGH);
    delay(50);
    digitalWrite(6,LOW);
    //delay(100);

    digitalWrite(5,HIGH);
    delay(50);
    digitalWrite(5,LOW);
    //delay(100);
    
    digitalWrite(4,HIGH);
    delay(50);
    digitalWrite(4,LOW);
    //delay(100);

    
 } 
 //delay(1000);
}
