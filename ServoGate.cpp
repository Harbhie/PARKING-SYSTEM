#include "ServoGate.h"

ServoGate::ServoGate(int pin, int openAngle, int closedAngle)
    : servoPin(pin), gateOpenAngle(openAngle), gateClosedAngle(closedAngle),
      isOpen(false), lastMoveTime(0), currentAngle(closedAngle),
      targetAngle(closedAngle), isMoving(false) {
}

void ServoGate::begin() {
    servo.attach(servoPin);
    servo.write(gateClosedAngle);
    currentAngle = gateClosedAngle;
    isOpen = false;
    Serial.println("Servo Gate initialized");
    Serial.print("Open angle: ");
    Serial.print(gateOpenAngle);
    Serial.print("°, Closed angle: ");
    Serial.print(gateClosedAngle);
    Serial.println("°");
}

void ServoGate::openGate() {
    if (!isOpen) {
        targetAngle = gateOpenAngle;
        isMoving = true;
        isOpen = true;
        Serial.println("Opening gate...");
    }
}

void ServoGate::closeGate() {
    if (isOpen) {
        targetAngle = gateClosedAngle;
        isMoving = true;
        isOpen = false;
        Serial.println("Closing gate...");
    }
}

void ServoGate::update() {
    if (!isMoving) {
        return;
    }

    if (millis() - lastMoveTime < SERVO_MOVE_DELAY) {
        return;
    }
    
    lastMoveTime = millis();

    if (currentAngle < targetAngle) {
        currentAngle++;
        servo.write(currentAngle);
    } else if (currentAngle > targetAngle) {
        currentAngle--;
        servo.write(currentAngle);
    } else {
        isMoving = false;
        if (isOpen) {
            Serial.println("Gate fully open");
        } else {
            Serial.println("Gate fully closed");
        }
    }
}

bool ServoGate::getIsOpen() {
    return isOpen;
}

bool ServoGate::isServoMoving() {
    return isMoving;
}

void ServoGate::setAngles(int openAngle, int closedAngle) {
    gateOpenAngle = openAngle;
    gateClosedAngle = closedAngle;
    Serial.println("Servo angles updated");
}

int ServoGate::getCurrentAngle() {
    return currentAngle;
}

void ServoGate::moveToAngle(int angle) {
    servo.write(angle);
    currentAngle = angle;
    isMoving = false;
}

void ServoGate::stopServo() {
    isMoving = false;
    servo.write(currentAngle);
}

void ServoGate::detach() {
    servo.detach();
}

void ServoGate::attach() {
    servo.attach(servoPin);
}
