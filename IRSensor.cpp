#include "IRSensor.h"

// Static member initialization
int IRSensor::DETECTION_THRESHOLD = 500;

IRSensor::IRSensor(int pin)
    : sensorPin(pin), objectDetected(false),
      lastDetectionTime(0), detectionTimeout(5000),
      previousState(false), stateChanged(false),
      lastStateChangeTime(0) {
}

void IRSensor::begin() {
    pinMode(sensorPin, INPUT);
    Serial.println("IR Sensor initialized");
    Serial.print("Sensor Pin: ");
    Serial.println(sensorPin);
}

int IRSensor::readSensorValue() {
    // For digital IR sensor, read 0 or 1
    // For analog IR sensor, read 0-1023
    return digitalRead(sensorPin);  // Change to analogRead() for analog sensors
}

bool IRSensor::isObjectDetected() {
    return objectDetected;
}

bool IRSensor::objectDetectedEdge() {
    // Return true only when transition from not detected to detected
    return (objectDetected && !previousState && stateChanged);
}

bool IRSensor::objectLeftEdge() {
    // Return true only when transition from detected to not detected
    return (!objectDetected && previousState && stateChanged);
}

unsigned long IRSensor::getTimeSinceDetection() {
    if (!objectDetected) {
        return millis() - lastDetectionTime;
    }
    return 0;
}

void IRSensor::resetDetection() {
    objectDetected = false;
    previousState = false;
    stateChanged = false;
    lastDetectionTime = 0;
}

void IRSensor::update() {
    // Read current sensor value
    int currentValue = readSensorValue();
    
    // Determine detection state
    // Most IR sensors: HIGH = no object, LOW = object detected
    bool currentState = (currentValue == LOW);
    
    // Check if state has changed
    if (currentState != previousState) {
        // Debounce: wait to confirm state change
        if (millis() - lastStateChangeTime >= DEBOUNCE_DELAY) {
            objectDetected = currentState;
            stateChanged = true;
            previousState = currentState;
            lastStateChangeTime = millis();
            
            // Update detection time
            if (objectDetected) {
                lastDetectionTime = millis();
                Serial.println("[IR] Object detected!");
            } else {
                Serial.println("[IR] Object left!");
            }
        }
    } else {
        stateChanged = false;
    }
}

bool IRSensor::hasStateChanged() {
    return stateChanged;
}

String IRSensor::getStatusString() {
    if (objectDetected) {
        return "DETECTED";
    } else {
        return "CLEAR";
    }
}

void IRSensor::setDetectionThreshold(int threshold) {
    DETECTION_THRESHOLD = threshold;
    Serial.print("Detection threshold set to: ");
    Serial.println(threshold);
}

int IRSensor::getRawValue() {
    return readSensorValue();
}
