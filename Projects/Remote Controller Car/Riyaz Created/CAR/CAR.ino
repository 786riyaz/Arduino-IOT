char val;
          //ZYWVUT
//tuvwyz
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(6,HIGH);
  if(Serial.available())
  {
    val=Serial.read();
    Serial.println(val);

    if(val=='S')
    {
      digitalWrite(2,LOW);
      digitalWrite(4,LOW);
      digitalWrite(12,LOW);
      digitalWrite(13,LOW);
    }
    else if(val=='F')
    {
      digitalWrite(2,HIGH);
      digitalWrite(12,HIGH);
    }
    else if(val=='G')
    {
      digitalWrite(4,HIGH);
      digitalWrite(13,HIGH);
    }
    else if(val=='R')
    {
      digitalWrite(12,HIGH);
    }
    else if(val=='L')
    {
      digitalWrite(2,HIGH);
    }
  }
}
