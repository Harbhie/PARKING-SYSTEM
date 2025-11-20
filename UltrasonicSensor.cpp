#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int trigger, int echo, int buzzer)
    : triggerPin(trigger), echoPin(echo), buzzerPin(buzzer),
      lastMeasureTime(0), measureInterval(100), 
      currentBuzzerFrequency(0), buzzerActive(false) {
}

void UltrasonicSensor::begin() {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
  
    digitalWrite(buzzerPin, LOW);
}

long UltrasonicSensor::measureDistance() {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 30000);
    
    long distance = duration * 0.01715;
    
    lastMeasureTime = millis();
    return distance;
}

long UltrasonicSensor::getCurrentDistance() {
    if (millis() - lastMeasureTime >= measureInterval) {
        return measureDistance();
    }
    return -1; 
}

int UltrasonicSensor::calculateBuzzerFrequency(long distance) {
    
    if (distance >= WARNING_DISTANCE) {
        return 0;  
    } else if (distance >= CRITICAL_DISTANCE) {
        int frequency = 500 + ((WARNING_DISTANCE - distance) * 10);
        return frequency;
    } else if (distance >= DANGER_DISTANCE) {
        int frequency = 1500 + ((CRITICAL_DISTANCE - distance) * 33);
        return frequency;
    } else {
        return 2500;
    }
}

void UltrasonicSensor::updateBuzzer() {
    long distance = getCurrentDistance();
    
    if (distance > 0) { 
        int frequency = calculateBuzzerFrequency(distance);
        
        if (frequency > 0) {
            if (frequency != currentBuzzerFrequency) {
                noTone(buzzerPin);
                tone(buzzerPin, frequency);
                currentBuzzerFrequency = frequency;
                buzzerActive = true;
            }
        } else {
            if (buzzerActive) {
                noTone(buzzerPin);
                currentBuzzerFrequency = 0;
                buzzerActive = false;
            }
        }
    }
}

void UltrasonicSensor::stopBuzzer() {
    noTone(buzzerPin);
    currentBuzzerFrequency = 0;
    buzzerActive = false;
}

void UltrasonicSensor::startBuzzer(int frequency) {
    if (frequency > 0) {
        tone(buzzerPin, frequency);
        currentBuzzerFrequency = frequency;
        buzzerActive = true;
    }
}

bool UltrasonicSensor::isCriticalDistance() {
    long distance = getCurrentDistance();
    return (distance > 0 && distance <= DANGER_DISTANCE);
}
