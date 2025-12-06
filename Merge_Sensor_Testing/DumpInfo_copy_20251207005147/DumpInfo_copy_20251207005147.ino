#include <ESP32Servo.h>
#define TRIG_PIN 12
#define ECHO_PIN 13
#define BUZZER_PIN 14
#define IR_PIN 27
#define SERVO_PIN 26

unsigned long buzzStartTime = 0;  // Timestamp when buzzer started
bool isBuzzing = false;           // Track buzzer state
bool objectDetectedPreviously = false; // Track previous distance state
Servo myServo;
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off
	pinMode(IR_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0); // initial position
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Convert to distance in cm
  float distance = duration * 0.0343 / 2;
  Serial.println(distance);

  // Check if distance is below threshold
  if (distance < 10) {
    if (!objectDetectedPreviously) {
      // Object just entered range, start buzzer
      isBuzzing = true;
      buzzStartTime = millis();
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("Object detected! Buzzing...");
    }
    objectDetectedPreviously = true;
  } else {
    objectDetectedPreviously = false; // Reset when object leaves range
  }

  // Stop buzzer after 10 seconds
  if (isBuzzing && millis() - buzzStartTime >= 10000) {
    isBuzzing = false;
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Buzzing stopped.");
  }
	int irState = digitalRead(IR_PIN);

  if (irState == HIGH) { // Object detected
    myServo.write(180); // rotate to 180°
  } 
  else { // No object detected
		
    myServo.write(0); // rotate back to 0°
		delay(5000);
  }

  // delay(100); // small delay for stability

  delay(100); // Short delay for stability
}
// #include <ESP32Servo.h>

// #define IR_PIN 27
// #define SERVO_PIN 26

// Servo myServo;

// void setup() {
//   pinMode(IR_PIN, INPUT);
//   myServo.attach(SERVO_PIN);
//   myServo.write(0); // initial position
// }

// void loop() {
//   int irState = digitalRead(IR_PIN);

//   if (irState == HIGH) { // Object detected
//     myServo.write(180); // rotate to 180°
//   } 
//   else { // No object detected
		
//     myServo.write(0); // rotate back to 0°
// 		delay(5000);
//   }

//   delay(100); // small delay for stability
// }
