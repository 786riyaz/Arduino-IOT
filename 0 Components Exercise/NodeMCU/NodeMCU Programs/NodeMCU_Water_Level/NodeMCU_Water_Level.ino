#define inputpin  D8
#define motor D7

#define level10 D1
#define level25 D2
#define level50 D3
#define level75 D4
#define level90 D5
int waterlevel=0;
void setup() {
  // put your setup code here, to run once:
  
  pinMode(inputpin,INPUT);
  pinMode(level10,OUTPUT);
  pinMode(level25,OUTPUT);
  pinMode(level50,OUTPUT);
  pinMode(level75,OUTPUT);
  pinMode(level90,OUTPUT);
  pinMode(motor,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(level10,HIGH);
  digitalWrite(level25,HIGH);
  digitalWrite(level50,HIGH);
  digitalWrite(level75,HIGH);
  digitalWrite(level90,HIGH);
  Serial.println(".");
  int initial=digitalRead(inputpin);
  //Checking all pin 
  if(initial == 1)
  {
    //Checking for level 10
    digitalWrite(level10,LOW);
    digitalWrite(level25,LOW);
    digitalWrite(level50,LOW);
    digitalWrite(level75,LOW);
    digitalWrite(level90,LOW);

    int l10=digitalRead(inputpin);
    if(l10 == 0)
    {
      Serial.println("Water level 10");
      waterlevel=10;
    }
    if(walterlevel == 10)
    {
      digitalWrite(level10,HIGH);
      digitalWrite(level50,HIGH);
      digitalWrite(level75,HIGH);
      digitalWrite(level90,HIGH);
      digitalWrite(level25,LOW);
      l25=digitalRead(inputpin);

      if(l25 == 0)
      {
        Serial.println("Water level 25");
        waterlevel = 25;
      }
      else if(l25 == 1)
      {
        Serial.println("Water Level > 25");
      }
    }
  
    
  }
  else
  {
    Serial.println("Level less than 10");
    //Serial.println(" WaterLevel="+String(waterlevel));
    Serial.println();
  }
delay(500);
}
