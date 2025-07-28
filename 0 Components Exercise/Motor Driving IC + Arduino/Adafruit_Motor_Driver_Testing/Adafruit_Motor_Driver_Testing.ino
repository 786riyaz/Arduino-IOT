#include <AFMotor.h>
AF_DCMotor M3(3);
AF_DCMotor M4(4);


void setup()
{
  M3.setSpeed(250);
  M4.setSpeed(1000);
}

void loop() 
{

  M3.run(FORWARD);
  M4.run(FORWARD);
  delay(2000);
  M3.run(BACKWARD);
  M4.run(BACKWARD);
  delay(2000);
 
  

}
