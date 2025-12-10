// ---------------------- syscontroller.h (final) -----------------------
#ifndef SYSCONTROLLER_H
#define SYSCONTROLLER_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

class SystemController {

public:

    enum EventType {
        EVT_NONE = 0,
        EVT_GATE_OPEN,
        EVT_GATE_CLOSE,
        EVT_SLOT_OCCUPIED,
        EVT_SLOT_AVAILABLE,
        EVT_BUZZER_OFF
    };

    SystemController();

    void begin();
    EventType update();

    float getDistance() const;
    String getSystemStatus();

    void setMode(int m);  // 0=AUTO, 1=MAINT, 2=SLEEP
    void manualOpen();
    void manualClose();

    void disablePinsExceptGate();
    void enableGateOnly();
    void enableAllPins();

private:

    int _mode = 0;

    int _irPin1 = 15;
    int _irPin2 = 14;
    int _irPin3 = 32;

    int _trigPin = 4;
    int _echoPin = 33;

    int _buzzerPin = 21;

    int _gateServoPin = 13; 
    int _alertServoPin = 25;

    MFRC522 _rfid;
    bool _rfidActive;
    byte _allowedTag1[4];
    byte _allowedTag2[4];

    Servo _gateServo;
    Servo _alertServo;

    bool _forwardTriggered;
    bool _backwardTriggered;

    bool _isBuzzing;
    unsigned long _buzzStartTime;

    bool _occupiedState;
    bool _availableState;

    int _gateState;
    float _lastDistance;

    bool checkRFID();
    void gateOpen();
    void gateClose();
    float readDistance();
};

#endif
