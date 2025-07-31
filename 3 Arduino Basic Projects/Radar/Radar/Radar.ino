#include <Servo.h> // Include the Servo library to control the servo motor

// Define pins for the ultrasonic sensor
const int trigPin = 11; // Trigger pin for sending ultrasonic pulses
const int echoPin = 10; // Echo pin for receiving the reflected pulses

// Variables to store the duration of the pulse and the calculated distance
long duration; // Stores the time taken for the ultrasonic pulse to return
int distance; // Stores the calculated distance in centimeters
Servo myServo; // Create a Servo object to control the servo motor

void setup() {
  pinMode(trigPin, OUTPUT); // Set the trigger pin as output to send pulses
  pinMode(echoPin, INPUT); // Set the echo pin as input to receive pulses
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate for logging
  myServo.attach(12); // Attach the servo motor to pin 12
}

void loop() {
  // Sweep the servo from 15 to 165 degrees
  for (int i = 15; i <= 165; i++) {
    myServo.write(i); // Set servo to angle i
    delay(30); // Wait 30ms to allow the servo to reach the position 
    distance = calculateDistance(); // Calculate distance using the ultrasonic sensor
    // Print distance and angle in a single line
  Serial.print(i); // Sends the current degree into the Serial Port
  Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  Serial.print(distance); // Sends the distance value into the Serial Port
  Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  }
  
  // Sweep the servo back from 165 to 15 degrees
  for (int i = 165; i > 15; i--) {
    myServo.write(i); // Set servo to angle i
    delay(30); // Wait 30ms to allow the servo to reach the position
    distance = calculateDistance(); // Calculate distance using the ultrasonic sensor
    // Print distance and angle in a single line
  Serial.print(i);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");
  }
}

// Function to calculate the distance using the ultrasonic sensor
int calculateDistance() {
  digitalWrite(trigPin, LOW); // Ensure trigger pin is low to start
  delayMicroseconds(2); // Short delay to stabilize
  digitalWrite(trigPin, HIGH); // Send a 10us pulse to trigger the sensor
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Measure the time for the echo to return
  distance = duration * 0.034 / 2; // Calculate distance (speed of sound = 0.034 cm/us, divided by 2 for round trip)
  return distance; // Return the calculated distance in centimeters
}