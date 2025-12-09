#ifndef SYSCONTROLLER_H
#define SYSCONTROLLER_H

#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// ------------------ RFID GATE SERVO ------------------
extern Servo gateServo;
extern const int irPin1;
extern const int irPin2;
extern const int gateServoPin;

// RFID pins
#define SS_PIN 5
#define RST_PIN 22
extern MFRC522 rfid;

extern byte allowedTag1[4];
extern byte allowedTag2[4];

extern bool hasTriggeredForward;
extern bool hasTriggeredBackward;
extern int count;

// ------------------ ULTRASONIC + BUZZER + IR SERVO ------------------
#define TRIG_PIN 4
#define ECHO_PIN 33
#define BUZZER_PIN 21
#define IR_PIN 32
#define SERVO_PIN 25

extern Servo alertServo;
extern unsigned long buzzStartTime;
extern bool isBuzzing;
extern bool objectDetectedPreviously;

extern float lastDistance;

// --- Event flags ---
extern volatile bool evtGateOpen;
extern volatile bool evtGateClose;
extern volatile bool evtSlotOccupied;
extern volatile bool evtSlotAvailable;
extern volatile bool evtBuzzerOff;

// API
void systemSetup();
void systemLoop();
bool checkRFID();
void triggerGateOpen();
void triggerGateClose();

#endif
