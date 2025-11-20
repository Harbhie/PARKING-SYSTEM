#include "ExitServoGate.h"

ExitServoGate::ExitServoGate(int pin, int openAngle, int closedAngle)
    : servoPin(pin), gateOpenAngle(openAngle), gateClosedAngle(closedAngle),
      isOpen(false), lastMoveTime(0), currentAngle(closedAngle),
      targetAngle(closedAngle), isMoving(false),
      gateOpenTime(0), autoCloseDelay(5000), autoCloseEnabled(true) {
}

void ExitServoGate::begin() {
    servo.attach(servoPin);
    servo.write(gateClosedAngle);
    currentAngle = gateClosedAngle;
    isOpen = false;
    Serial.println("Exit Servo Gate initialized");
    Serial.print("Open angle: ");
    Serial.print(gateOpenAngle);
    Serial.print("°, Closed angle: ");
    Serial.print(gateClosedAngle);
    Serial.println("°");
}

void ExitServoGate::openGate() {
    if (!isOpen) {
        targetAngle = gateOpenAngle;
        isMoving = true;
        isOpen = true;
        gateOpenTime = millis();
        Serial.println("Opening exit gate...");
    }
}

void ExitServoGate::closeGate() {
    if (isOpen) {
        targetAngle = gateClosedAngle;
        isMoving = true;
        isOpen = false;
        gateOpenTime = 0;
        Serial.println("Closing exit gate...");
    }
}

void ExitServoGate::update() {
    if (!isMoving) {
        if (autoCloseEnabled && isOpen && gateOpenTime > 0) {
            if (shouldAutoClose()) {
                Serial.println("Auto-closing gate due to timeout...");
                closeGate();
            }
        }
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
            Serial.println("Exit gate fully open");
        } else {
            Serial.println("Exit gate fully closed");
        }
    }
}

bool ExitServoGate::getIsOpen() {
    return isOpen;
}

bool ExitServoGate::isServoMoving() {
    return isMoving;
}

void ExitServoGate::setAngles(int openAngle, int closedAngle) {
    gateOpenAngle = openAngle;
    gateClosedAngle = closedAngle;
    Serial.println("Servo angles updated");
}

int ExitServoGate::getCurrentAngle() {
    return currentAngle;
}

void ExitServoGate::moveToAngle(int angle) {
    servo.write(angle);
    currentAngle = angle;
    isMoving = false;
}

void ExitServoGate::stopServo() {
    isMoving = false;
    servo.write(currentAngle);
}

void ExitServoGate::setAutoCloseDelay(unsigned long delayMs) {
    autoCloseDelay = delayMs;
    Serial.print("Auto-close delay set to: ");
    Serial.print(autoCloseDelay);
    Serial.println("ms");
}

bool ExitServoGate::shouldAutoClose() {
    if (gateOpenTime == 0) {
        return false;
    }
    
    return (millis() - gateOpenTime >= autoCloseDelay);
}

void ExitServoGate::enableAutoClose(bool enable) {
    autoCloseEnabled = enable;
    Serial.print("Auto-close: ");
    Serial.println(enable ? "ENABLED" : "DISABLED");
}

void ExitServoGate::detach() {
    servo.detach();
}

void ExitServoGate::attach() {
    servo.attach(servoPin);
}
