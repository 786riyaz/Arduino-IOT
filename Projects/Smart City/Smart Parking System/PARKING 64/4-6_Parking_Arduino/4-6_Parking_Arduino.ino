int data;
void setup() 
{  
  Serial.begin(9600);
}

void loop() 
{
  data = Serial.read(); //Read the serial data and store it
  Serial.println(data);
  delay(1000);
}
