#define RED_PIN 7
#define YELLOW_PIN 6
#define GREEN_PIN 5

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  Serial.begin(9600); // Initialize Serial communication
  Serial.println("Traffic Signal System Started");
}

void loop() {
  // Red light on for 5 seconds with countdown
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  Serial.println("Red Light ON");
  for (int i = 5; i > 0; i--) {
    Serial.print("Stop for ");
    Serial.print(i);
    Serial.println(" Seconds");
    delay(1000);
  }

  // Yellow light on for 2 seconds
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  Serial.println("Yellow Light ON");
  delay(2000);

  // Green light on for 5 seconds with countdown
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  Serial.println("Green Light ON");
  for (int i = 5; i > 0; i--) {
    Serial.print("Go for ");
    Serial.print(i);
    Serial.println(" Seconds");
    delay(1000);
  }
}