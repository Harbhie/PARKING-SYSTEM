
// -------------------------------- ESP32 ---------------------------------------- //
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

Servo myServo;

// Updated pins for ESP32
const int irPin1 = 15;     // IR1
const int irPin2 = 14;     // IR2
const int servoPin = 13;   // Servo (SAFE PIN)

// RFID Pins for ESP32
#define SS_PIN 5
#define RST_PIN 22

// SPI pins for ESP32: SCK=18, MISO=19, MOSI=23
MFRC522 rfid(SS_PIN, RST_PIN);

// Allowed RFID tags
byte allowedTag1[4] = {0xF2, 0xCB, 0x12, 0x01};
byte allowedTag2[4] = {0x22, 0x73, 0xB4, 0xAB};

bool hasTriggeredForward = false;
bool hasTriggeredBackward = false;

int count = 0;

void setup() {
  Serial.begin(115200);

  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  SPI.begin(18, 19, 23);   
  rfid.PCD_Init();

  myServo.attach(servoPin);
  myServo.detach();

  Serial.println("System Ready...");
}

// Check if scanned tag matches allowedTag1 OR allowedTag2
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

void loop() {
  bool ir1Detected = (digitalRead(irPin1) == LOW);
  bool ir2Detected = (digitalRead(irPin2) == LOW);
  bool rfidDetected = checkRFID();

  // Forward Trigger
  if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
    hasTriggeredForward = true;
    count = 1;

    myServo.attach(servoPin);
    myServo.write(0);
    delay(400);
    myServo.detach();

    Serial.println("Forward Triggered");
  }

  if (!ir1Detected && !rfidDetected) {
    hasTriggeredForward = false;
  }

  // Backward Trigger
  if (count == 1 && ir2Detected && !hasTriggeredBackward) {
    hasTriggeredBackward = true;
    count = 0;

    myServo.attach(servoPin);
    myServo.write(180);
    delay(400);
    myServo.detach();

    Serial.println("Backward Triggered");
  }

  if (!ir2Detected) {
    hasTriggeredBackward = false;
  }
}
