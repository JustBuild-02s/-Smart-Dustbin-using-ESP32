Smart dustbin
#include <ESP32Servo.h>

Servo flapServo;

// Pin definitions
#define TRIG_PIN 25
#define ECHO_PIN 32
#define MOISTURE_PIN 34
#define SERVO_PIN 13
#define DRY_LED 2
#define WET_LED 4
#define BUZZER_PIN 5

// Thresholds
#define DISTANCE_THRESHOLD 15    // cm
#define MOISTURE_THRESHOLD 2500  // <2500 = wet, >2500 = dry

// Function to measure distance
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1; // no echo
  long distance = duration * 0.034 / 2; // cm
  return distance;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DRY_LED, OUTPUT);
  pinMode(WET_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  flapServo.attach(SERVO_PIN);
  flapServo.write(90); // neutral position

  Serial.println("===== SMART DUSTBIN TEST MODE =====");
}

void loop() {
  long distance = getDistance();

  Serial.print("Distance (cm): ");
  if (distance == -1) Serial.println("No object");
  else Serial.println(distance);

  if (distance > 0 && distance < DISTANCE_THRESHOLD) {
    Serial.println("Object Detected!");

    // Buzzer beep
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Buzzer: ON");
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Buzzer: OFF");

    // Read moisture sensor
    int moistureValue = analogRead(MOISTURE_PIN);
    Serial.print("Moisture Value: ");
    Serial.println(moistureValue);

    if (moistureValue < MOISTURE_THRESHOLD) {
      // Wet waste
      Serial.println("Waste Type: WET");
      digitalWrite(WET_LED, HIGH);
      digitalWrite(DRY_LED, LOW);
      flapServo.write(30); // wet side
      Serial.println("Servo Position: 30° (Wet)");
    } else {
      // Dry waste
      Serial.println("Waste Type: DRY");
      digitalWrite(DRY_LED, HIGH);
      digitalWrite(WET_LED, LOW);
      flapServo.write(150); // dry side
      Serial.println("Servo Position: 150° (Dry)");
    }

    delay(3000); // Wait for user to remove object

    // Reset LEDs and servo
    digitalWrite(DRY_LED, LOW);
    digitalWrite(WET_LED, LOW);
    flapServo.write(90); // neutral
    Serial.println("System Reset to Neutral");
  } else {
    Serial.println("No Object Detected");
  }

  Serial.println("----------------------------------");
  delay(8000);
}
