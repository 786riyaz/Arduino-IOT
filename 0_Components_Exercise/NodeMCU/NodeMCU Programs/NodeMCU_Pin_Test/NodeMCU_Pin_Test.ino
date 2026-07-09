#define inputpin0 D0
#define inputpin1 D1
#define inputpin2 D2
#define inputpin3 D3
#define inputpin4 D4
#define inputpin5 D5
#define inputpin6 D6
#define inputpin7 D7
#define inputpin8 D8
void setup() {
  // put your setup code here, to run once:
  pinMode(inputpin0,INPUT_PULLUP);
  pinMode(inputpin1,INPUT_PULLUP);
  pinMode(inputpin2,INPUT_PULLUP);
  pinMode(inputpin3,INPUT_PULLUP);
  pinMode(inputpin4,INPUT_PULLUP);
  pinMode(inputpin5,INPUT_PULLUP);
  pinMode(inputpin6,INPUT_PULLUP);
  pinMode(inputpin7,INPUT_PULLUP);
  pinMode(inputpin8,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value0=digitalRead(inputpin0);
  int value1=digitalRead(inputpin1);
  int value2=digitalRead(inputpin2);
  int value3=digitalRead(inputpin3);
  int value4=digitalRead(inputpin4);
  int value5=digitalRead(inputpin5);
  int value6=digitalRead(inputpin6);
  int value7=digitalRead(inputpin7);
  int value8=digitalRead(inputpin8);
  Serial.print("D0="+String(value0));
  Serial.print(" D1="+String(value1));
  Serial.print(" D2="+String(value2));
  Serial.print(" D3="+String(value3));
  Serial.print(" D4="+String(value4));
  Serial.print(" D5="+String(value5));
  Serial.print(" D6="+String(value6));
  Serial.print(" D7="+String(value7));
  Serial.println(" D8="+String(value8));
  delay(500);
}
