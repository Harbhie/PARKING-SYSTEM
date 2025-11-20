#ifndef SERVO_GATE_H
#define SERVO_GATE_H

#include <Arduino.h>
#include <Servo.h>

class ServoGate {
private:
    Servo servo;
    int servoPin;

    int gateOpenAngle;  
    int gateClosedAngle; 

    bool isOpen;
    unsigned long lastMoveTime;
    static const int SERVO_MOVE_DELAY = 50; 

    int currentAngle;
    int targetAngle;
    bool isMoving;
    
public:
    ServoGate(int pin, int openAngle = 90, int closedAngle = 0);

    void begin();

    void openGate();

    void closeGate();

    void update();

    bool getIsOpen();

    bool isServoMoving();

    void setAngles(int openAngle, int closedAngle);

    int getCurrentAngle();

    void moveToAngle(int angle);

    void stopServo();

    void detach();

    void attach();
};

#endif
