#include <Servo.h>

Servo myServo;
const int trigPin = 9; // Trig pin
const int echoPin = 8; // Echo pin
const int servoPin = 10;
const int distanceThreshold = 15; // Distance in cm to detect person
bool isServoPulled = false; // Track servo state

long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // Convert to distance in cm
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin);
  myServo.write(0); // Initial position at 0 degrees (rope released)
  isServoPulled = false; // Initial state
  Serial.begin(9600); // Initialize Serial communication
}

void loop() {
  long distance = readUltrasonicDistance();
  
  // Print distance to Serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  if (distance < distanceThreshold && distance > 0) {
    if (!isServoPulled) {
      myServo.write(90); // Rotate to 180 degrees to pull the rope
      isServoPulled = true; // Update state
      delay(3000); // Wait 3 seconds after pulling
    }
  } else {
    myServo.write(0); // Return to 0 degrees to release the rope
    isServoPulled = false; // Reset state
  }
  
  delay(100); // Small delay for stability
}