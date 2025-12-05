// #include <Servo.h>
// #include <SPI.h>
// #include <MFRC522.h>

// Servo myServo;

// const int irPin1 = 9;    
// const int irPin2 = 6;     
// const int servoPin = 8;

// #define SS_PIN 10
// #define RST_PIN 7

// MFRC522 rfid(SS_PIN, RST_PIN);

// byte allowedTag[4] = {0xF2, 0xCB, 0x12, 0x01};

// bool hasTriggeredForward = false;
// bool hasTriggeredBackward = false;

// int count = 0;

// void setup() {
//   pinMode(irPin1, INPUT);
//   pinMode(irPin2, INPUT);

//   SPI.begin();
//   rfid.PCD_Init();

//   myServo.attach(servoPin);
//   myServo.detach();  
// }

// bool checkRFID() {
//   if (!rfid.PICC_IsNewCardPresent()) return false;
//   if (!rfid.PICC_ReadCardSerial()) return false;

//   for (byte i = 0; i < 4; i++) {
//     if (rfid.uid.uidByte[i] != allowedTag[i]) return false;
//   }
//   return true;
// }

// void loop() {
//   bool ir1Detected = (digitalRead(irPin1) == LOW);
//   bool ir2Detected = (digitalRead(irPin2) == LOW);
//   bool rfidDetected = checkRFID();

//   if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
//     hasTriggeredForward = true;
//     count = 1;  

//     myServo.attach(servoPin);
//     myServo.write(0); 
//     delay(400);
//     myServo.detach();
//   }

//   if (!ir1Detected && !rfidDetected) {
//     hasTriggeredForward = false;
//   }

//   if (count == 1 && ir2Detected && !hasTriggeredBackward) {
//     hasTriggeredBackward = true;
//     count = 0;  

//     myServo.attach(servoPin);
//     myServo.write(180); 
//     delay(400);
//     myServo.detach();
//   }

//   if (!ir2Detected) {
//     hasTriggeredBackward = false;
//   }
// }
// #include <Servo.h>
// #include <SPI.h>
// #include <MFRC522.h>

// Servo myServo;

// const int irPin1 = 3;
// const int irPin2 = 4;
// const int servoPin = 8;

// #define SS_PIN 10
// #define RST_PIN 7

// MFRC522 rfid(SS_PIN, RST_PIN);

// byte allowedTag[4] = {0xF2, 0xCB, 0x12, 0x01};

// bool hasTriggeredForward = false;
// bool hasTriggeredBackward = false;

// int count = 0;

// void setup() {
//   Serial.begin(9600);

//   pinMode(irPin1, INPUT);
//   pinMode(irPin2, INPUT);

//   SPI.begin();
//   rfid.PCD_Init();

//   myServo.attach(servoPin);
//   myServo.detach();
// }

// bool checkRFID() {
//   if (!rfid.PICC_IsNewCardPresent()) return false;
//   if (!rfid.PICC_ReadCardSerial()) return false;

//   for (byte i = 0; i < 4; i++) {
//     if (rfid.uid.uidByte[i] != allowedTag[i]) return false;
//   }
//   return true;
// }

// void loop() {
//   bool ir1Detected = (digitalRead(irPin1) == LOW);
//   bool ir2Detected = (digitalRead(irPin2) == LOW);
//   bool rfidDetected = checkRFID();

//   if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
//     hasTriggeredForward = true;
//     count = 1;

//     Serial.println("Servo Rotating FORWARD");

//     myServo.attach(servoPin);
//     myServo.write(0);
//     delay(400);
//     myServo.detach();
//   }

//   if (!ir1Detected && !rfidDetected) {
//     hasTriggeredForward = false;
//   }

//   if (count == 1 && ir2Detected && !hasTriggeredBackward) {
//     hasTriggeredBackward = true;
//     count = 0;

//     Serial.println("Servo Rotating BACKWARD");

//     myServo.attach(servoPin);
//     myServo.write(180);
//     delay(400);
//     myServo.detach();
//   }

//   if (!ir2Detected) {
//     hasTriggeredBackward = false;
//   }
// }

// -------------------------------- ESP32 ---------------------------------------- //
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

Servo myServo;

// IR Sensor Pins
const int irPin1 = 18;
const int irPin2 = 21;

// Servo Pin (ESP32 PWM)
const int servoPin = 5;

// RFID Pins (ESP32 SPI)
#define SS_PIN 22
#define RST_PIN 3
#define SCK_PIN 19
#define MOSI_PIN 23
#define MISO_PIN 25

MFRC522 rfid(SS_PIN, RST_PIN);

// Allowed Tag
byte allowedTag[4] = {0xF2, 0xCB, 0x12, 0x01};

bool hasTriggeredForward = false;
bool hasTriggeredBackward = false;
int count = 0;

void setup() {
  Serial.begin(115200);

  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  // ESP32 SPI initialization
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  rfid.PCD_Init();

  // ATTENTION: Remove invalid Servo::attach() line
  // Correct code is below:

  myServo.attach(servoPin);
  myServo.detach();

  Serial.println("ESP32 RFID + IR + Servo ready");
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

  // Forward rotation
  if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
    hasTriggeredForward = true;
    count = 1;

    Serial.println("Servo Rotating FORWARD");

    myServo.attach(servoPin);
    myServo.write(0);
    delay(400);
    myServo.detach();
  }

  if (!ir1Detected && !rfidDetected) {
    hasTriggeredForward = false;
  }

  // Backward rotation
  if (count == 1 && ir2Detected && !hasTriggeredBackward) {
    hasTriggeredBackward = true;
    count = 0;

    Serial.println("Servo Rotating BACKWARD");

    myServo.attach(servoPin);
    myServo.write(180);
    delay(400);
    myServo.detach();
  }

  if (!ir2Detected) {
    hasTriggeredBackward = false;
  }
}
