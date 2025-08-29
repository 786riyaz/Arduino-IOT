// Pin configuration
#define IR_LEFT  2     // Left IR sensor output pin
#define IR_RIGHT 3     // Right IR sensor output pin
#define ENA 5         // Enable pin for left motor
#define ENB 6         // Enable pin for right motor
#define IN1 7         // Left motor forward
#define IN2 8         // Left motor backward
#define IN3 9         // Right motor forward
#define IN4 10        // Right motor backward

int motorSpeed = 150; // Adjustable motor speed
int stopCount = 0;    // Counter for stop confirmation

void setup() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  Serial.begin(9600);
  Serial.println("Setup complete. Starting line following...");
  Serial.print("Motor speed set to: "); Serial.println(motorSpeed);
}

int readSensorStable(int pin) {
  int readings = 0;
  for (int i = 0; i < 3; i++) {
    readings += digitalRead(pin);
    delay(10);
  }
  return (readings >= 2) ? 1 : 0; // Majority vote
}

void loop() {
  int leftSensor = readSensorStable(IR_LEFT);
  int rightSensor = readSensorStable(IR_RIGHT);

  Serial.print("Left: ");
  Serial.print(leftSensor);
  Serial.print(" | Right: ");
  Serial.println(rightSensor);

  // 0 = black line detected, 1 = white surface
  if (leftSensor == 0 && rightSensor == 0) {
    stopCount = 0; // Reset stop counter
    forward();
    Serial.println("Action: Going forward");
  }
  else if (leftSensor == 0 && rightSensor == 1) {
    stopCount = 0;
    left();
    Serial.println("Action: Turning left");
  }
  else if (leftSensor == 1 && rightSensor == 0) {
    stopCount = 0;
    right();
    Serial.println("Action: Turning right");
  }
  else {
    stopCount++;
    if (stopCount >= 3) { // Require 3 consecutive stops
      stopMotors();
      Serial.println("Action: Stopping motors (confirmed)");
    } else {
      Serial.println("Action: Pending stop confirmation");
    }
  }

  delay(100); // Stabilize loop
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Motors: Left forward | Right forward");
}

void left() {
  digitalWrite(IN1, LOW);  // Left motor stop (softer turn)
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); // Right motor forward
  digitalWrite(IN4, LOW);
  Serial.println("Motors: Left stopped | Right forward");
}

void right() {
  digitalWrite(IN1, HIGH); // Left motor forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  // Right motor stop (softer turn)
  digitalWrite(IN4, LOW);
  Serial.println("Motors: Left forward | Right stopped");
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motors: All stopped");
}