#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

Servo myServo;

const int irPin1 = 3;    
const int irPin2 = 4;     
const int servoPin = 8;

#define SS_PIN 10
#define RST_PIN 7

MFRC522 rfid(SS_PIN, RST_PIN);

byte allowedTag[4] = {0xF2, 0xCB, 0x12, 0x01};

bool hasTriggeredForward = false;
bool hasTriggeredBackward = false;

int count = 0;

void setup() {
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  SPI.begin();
  rfid.PCD_Init();

  myServo.attach(servoPin);
  myServo.detach();  
}

bool checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial()) return false;

  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedTag[i]) return false;
  }
  return true;
}

void loop() {
  bool ir1Detected = (digitalRead(irPin1) == LOW);
  bool ir2Detected = (digitalRead(irPin2) == LOW);
  bool rfidDetected = checkRFID();

  if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
    hasTriggeredForward = true;
    count = 1;  

    myServo.attach(servoPin);
    myServo.write(0); 
    delay(400);
    myServo.detach();
  }

  if (!ir1Detected && !rfidDetected) {
    hasTriggeredForward = false;
  }

  if (count == 1 && ir2Detected && !hasTriggeredBackward) {
    hasTriggeredBackward = true;
    count = 0;  

    myServo.attach(servoPin);
    myServo.write(180); 
    delay(400);
    myServo.detach();
  }

  if (!ir2Detected) {
    hasTriggeredBackward = false;
  }
}
