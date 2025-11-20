#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
private:
    int triggerPin;
    int echoPin;
    int buzzerPin;
    
    static const int WARNING_DISTANCE = 100;  
    static const int CRITICAL_DISTANCE = 30;  
    static const int DANGER_DISTANCE = 15;    
    
    unsigned long lastMeasureTime;
    unsigned long measureInterval;
    
    int currentBuzzerFrequency;
    bool buzzerActive;

public:
    UltrasonicSensor(int trigger, int echo, int buzzer);
    
    void begin();

    long measureDistance();
    
    long getCurrentDistance();

    void updateBuzzer();

    int calculateBuzzerFrequency(long distance);

    void stopBuzzer();

    void startBuzzer(int frequency);

    bool isCriticalDistance();
};

#endif
