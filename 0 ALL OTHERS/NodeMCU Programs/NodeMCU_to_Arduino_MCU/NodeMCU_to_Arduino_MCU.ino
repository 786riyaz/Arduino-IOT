#include<SoftwareSerial.h>
SoftwareSerial s(D6,D5); // RX TX
int data;

void setup() {
  // put your setup code here, to run once:
  s.begin(9600);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  s.write("r");
  if(s.available() > 0)
  {
    data = s.read();
    Serial.println(data);
    delay(1000);
  }
}
