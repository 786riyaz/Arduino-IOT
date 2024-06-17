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

  digitalWrite(level10,HIGH);
  digitalWrite(level25,HIGH);
  digitalWrite(level50,HIGH);
  digitalWrite(level75,HIGH);
  digitalWrite(level90,HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print("...");
  int initial=digitalRead(inputpin);
  //Checking all pin 
  if(initial==1)
  {
    //Serial.println("Water Level is more than 0 percent");

    //Checking level 10 pin
    digitalWrite(level10,LOW);
    int l10=digitalRead(inputpin);
    if(l10==0)
    {
      //Serial.println("Water Tank level is more than 10 percent");
      waterlevel=10;
    }
    digitalWrite(level10,HIGH);
    
    //checking level 25 pin
    digitalWrite(level25,LOW);
    int l25=digitalRead(inputpin);
    if(l25==0)
    {
      //Serial.println("Water Tank level is more than 25 percent");
      waterlevel=25;
    }
    digitalWrite(level25,HIGH);

    //checking level 50 pin
    digitalWrite(level50,LOW);
    int l50=digitalRead(inputpin);
    if(l50==0)
    {
      //Serial.println("Water Tank level is more than 50 percent");
      waterlevel=50;      
    }
    digitalWrite(level50,HIGH);

    //checking level 75 pin
    digitalWrite(level75,LOW);
    int l75=digitalRead(inputpin);
    if(l75==0)
    {
      //Serial.println("Water Tank level is more than 75 percent");
      waterlevel=75;
    }
    digitalWrite(level75,HIGH);

    //checking level 90 pin
    digitalWrite(level90,LOW);
    int l90=digitalRead(inputpin);
    if(l90==0)
    {
      //Serial.println("Water Tank level is more than 90 percent");
      waterlevel=90;
    }
    digitalWrite(level90,HIGH);
    
    Serial.print("L0="+String(initial));
    Serial.print(" L10="+String(l10));
    Serial.print(" L25="+String(l25));
    Serial.print(" L50="+String(l50));
    Serial.print(" L75="+String(l75));
    Serial.print(" L90="+String(l90));
    Serial.println(" WaterLevel="+String(waterlevel));
    Serial.println();
  }
  else
  {
    Serial.println("Level less than 10");
    //Serial.println(" WaterLevel="+String(waterlevel));
    Serial.println();
  }
delay(500);
}
