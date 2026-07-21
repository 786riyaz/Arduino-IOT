#define RED_PIN 12
#define YELLOW_PIN 9
#define GREEN_PIN 6

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  Serial.begin(9600);  // Initialize Serial communication
  Serial.println("Traffic Signal System Started");
}

void loop() {
  // Green light on for 5 seconds
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  Serial.println("Green Light ON");
  delay(5000);


  // Yellow light on for 2 seconds
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  Serial.println("Yellow Light ON");
  delay(2000);

  // Red light on for 5 seconds
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  Serial.println("Red Light ON");
  delay(5000);
}