char val;
          //ZYWVUT
//tuvwyz



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(13,OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13,HIGH);
  delay(500);
  digitalWrite(13,LOW);
  delay(100);
  
  if(Serial.available())
  {
    val=Serial.read();
    Serial.println(val);

    if(val=='Z')
    {
      digitalWrite(3,HIGH);
    }
    else if(val=='z')
    {
      digitalWrite(3,LOW);
    }
    else if(val=='Y')
    {
      digitalWrite(4,HIGH);
    }
    else if(val=='y')
    {
      digitalWrite(4,LOW);
    }
    else if(val=='W')
    {
      digitalWrite(5,HIGH);
    }
    else if(val=='w')
    {
      digitalWrite(5,LOW);
    }
    else if(val=='V')
    {
      digitalWrite(6,HIGH);
    }
    else if(val=='v')
    {
      digitalWrite(6,LOW);
    }
    else if(val=='U')
    {
      digitalWrite(7,HIGH);
    }
    else if(val=='u')
    {
      digitalWrite(7,LOW);
    }
    else if(val=='T')
    {
      digitalWrite(8,HIGH);
    }
    else if(val=='t')
    {
      digitalWrite(8,LOW);
    }
  }
}
