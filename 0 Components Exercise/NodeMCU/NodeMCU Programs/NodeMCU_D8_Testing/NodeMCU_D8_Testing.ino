#define inputpin  D8
#define motor D7

#define level10 D1
#define level25 D2
#define level50 D3
#define level75 D4
#define level90 D5

void setup() {
  // put your setup code here, to run once:
  pinMode(inputpin,INPUT);
  pinMode(level10,OUTPUT);
  pinMode(level25,OUTPUT);
  pinMode(level50,OUTPUT);
  pinMode(level75,OUTPUT);
  pinMode(level90,OUTPUT);
  pinMode(motor,OUTPUT);

  digitalWrite(level10,LOW);
  digitalWrite(level25,LOW);
  digitalWrite(level50,LOW);
  digitalWrite(level75,LOW);
  digitalWrite(level90,LOW);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(level10,HIGH);
  delay(100);
  int l10=digitalRead(inputpin);
  delay(100);
  digitalWrite(level10,LOW);

  digitalWrite(level25,HIGH);
  delay(100);
  int l25=digitalRead(inputpin);
  delay(100);
  digitalWrite(level25,LOW);

  digitalWrite(level50,HIGH);
  delay(100);
  int l50=digitalRead(inputpin);
  delay(100);
  digitalWrite(level50,LOW);

  digitalWrite(level75,HIGH);
  delay(100);
  int l75=digitalRead(inputpin);
  delay(100);
  digitalWrite(level75,LOW);

  digitalWrite(level90,HIGH);
  delay(100);
  int l90=digitalRead(inputpin);
  delay(100);
  digitalWrite(level90,LOW);
  
  Serial.println("10="+String(l10));
  Serial.println("25="+String(l25));
  Serial.println("50="+String(l50));
  Serial.println("75="+String(l75));
  Serial.println("90="+String(l90));
  Serial.println();
  delay(200);
}
