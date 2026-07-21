
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  Serial.begin(115200);
  Serial.print("Start");
}

void loop() { // run over and over
  
}
