#define inputpin  D8
#define out1 D1
#define out2 D2

void setup() {
  // put your setup code here, to run once:
  pinMode(inputpin,INPUT);
  pinMode(out1,OUTPUT);
  pinMode(out2,OUTPUT);

  digitalWrite(out1,LOW);
  digitalWrite(out2,LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(out1,HIGH);
  digitalWrite(out2,HIGH);
  int l10=digitalRead(inputpin);  
  Serial.println("10="+String(l10));
  Serial.println();
  delay(1000);
}
