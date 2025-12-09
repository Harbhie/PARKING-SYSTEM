// -------------------------------------------------ver 2---------------------------------------------------
#include "syscontroller.h"

// --- Gate servo & IR pins ---
Servo gateServo;
const int irPin1 = 15;
const int irPin2 = 14;
const int gateServoPin = 13;

// RFID
MFRC522 rfid(SS_PIN, RST_PIN);

byte allowedTag1[4] = {0xF2, 0xCB, 0x12, 0x01};
byte allowedTag2[4] = {0x22, 0x73, 0xB4, 0xAB};

bool hasTriggeredForward = false;
bool hasTriggeredBackward = false;
int count = 0;

// Alert servo + ultrasonic + buzzer + IR3
Servo alertServo;
unsigned long buzzStartTime = 0;
bool isBuzzing = false;
float lastDistance = 0.0f;

// WebSocket event flags
volatile bool evtGateOpen = false;
volatile bool evtGateClose = false;
volatile bool evtSlotOccupied = false;
volatile bool evtSlotAvailable = false;
volatile bool evtBuzzerOff = false;

// State trackers
bool wasOccupied = false;
bool wasAvailable = false;

void systemSetup() {
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  SPI.begin(18, 19, 23);
  rfid.PCD_Init();

  gateServo.attach(gateServoPin);
  gateServo.write(180);
  delay(50);
  gateServo.detach();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);

  digitalWrite(BUZZER_PIN, LOW);

  alertServo.attach(SERVO_PIN);
  alertServo.write(0);
}

bool checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial()) return false;

  bool match1 = true;
  bool match2 = true;

  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedTag1[i]) match1 = false;
    if (rfid.uid.uidByte[i] != allowedTag2[i]) match2 = false;
  }

  return (match1 || match2);
}

void triggerGateOpen() {
  if (count == 0) {
    count = 1;
    gateServo.attach(gateServoPin);
    gateServo.write(0);
    delay(400);
    gateServo.detach();
    evtGateOpen = true;
  }
}

void triggerGateClose() {
  if (count == 1) {
    count = 0;
    gateServo.attach(gateServoPin);
    gateServo.write(180);
    delay(400);
    gateServo.detach();
    evtGateClose = true;
  }
}

// --------------------------------------
// ULTRASONIC + BUZZER LOGIC
// --------------------------------------
void handleUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return;

  float distance = duration * 0.0343 / 2.0;
  lastDistance = distance;

  if (distance > 0 && distance < 10.0) {
    if (!isBuzzing) {
      isBuzzing = true;
      buzzStartTime = millis();
      digitalWrite(BUZZER_PIN, HIGH);

      if (!wasOccupied) {
        evtSlotOccupied = true;
        wasOccupied = true;
        wasAvailable = false;
      }
    }
  }

  if (isBuzzing && millis() - buzzStartTime >= 5000) {
    digitalWrite(BUZZER_PIN, LOW);
    isBuzzing = false;
    evtBuzzerOff = true;
  }
}

// --------------------------------------
// IR3 LOGIC FOR AVAILABLE
// --------------------------------------
void handleIR3() {
  int irState = digitalRead(IR_PIN);

  if (lastDistance > 0 && lastDistance < 10.0) {
    wasAvailable = false;
    return;
  }

  if (isBuzzing) {
    wasAvailable = false;
    return;
  }

  if (irState == LOW) {
    delay(30);

    if (digitalRead(IR_PIN) == LOW) {
      alertServo.write(180);

      if (!wasAvailable) {
        evtSlotAvailable = true;
        wasAvailable = true;
        wasOccupied = false;
      }
    }
  } else {
    alertServo.write(0);
    wasAvailable = false;
  }
}

// --------------------------------------
// MAIN LOOP COMBINED
// --------------------------------------
void systemLoop() {
  bool ir1Detected = (digitalRead(irPin1) == LOW);
  bool ir2Detected = (digitalRead(irPin2) == LOW);
  bool rfidDetected = checkRFID();

  if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
    hasTriggeredForward = true;
    triggerGateOpen();
  }
  if (!ir1Detected && !rfidDetected) hasTriggeredForward = false;

  if (count == 1 && ir2Detected && !hasTriggeredBackward) {
    hasTriggeredBackward = true;
    triggerGateClose();
  }
  if (!ir2Detected) hasTriggeredBackward = false;

  handleUltrasonic();
  handleIR3();

  delay(60);
}

// ---------------------------- ver 3-------------------------
