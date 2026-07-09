  void setup() {
  //pinMode(D0,INPUT_PULLDOWN_16);
  pinMode(D1,INPUT_PULLUP);
  //pinMode(D1,OUTPUT);
  pinMode(D2,INPUT_PULLUP);
  pinMode(D3,INPUT_PULLUP);
  pinMode(D4,INPUT_PULLUP);
  pinMode(D5,INPUT_PULLUP);
  pinMode(A0,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //int d0=digitalRead(D0);
  int d1=digitalRead(D1);
  int d2=digitalRead(D2);
  int d3=digitalRead(D3);
  int d4=digitalRead(D4);
  int d5=digitalRead(D5);
  //int d6=digitalRead(D6);
  //int d7=digitalRead(D7);
  //int d8=digitalRead(D8);
  //int a0=analogRead(A0);
  //Serial.print("D0="+String(d0)+" D1="+String(d1)+" D2="+String(d2)+" D3="+String(d3)+" D4="+String(d4));
  //Serial.println(" D5="+String(d5)+" D6="+String(d6)+" D7="+String(d7)+" D8="+String(d8)+" A0="+String(a0));

  //analogWrite(A0,1000);
  //digitalWrite(D1,HIGH);

  Serial.print("D1="+String(d1)+" D2="+String(d2)+" D3="+String(d3)+" D4="+(d4)+" D5="+(d5));
  Serial.println();
}
