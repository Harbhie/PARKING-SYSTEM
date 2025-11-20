#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <Arduino.h>

class IRSensor {
private:
    int sensorPin;
    bool objectDetected;
    unsigned long lastDetectionTime;
    unsigned long detectionTimeout;
    
    // Detection state tracking
    bool previousState;
    bool stateChanged;
    
    // Debouncing
    static const unsigned long DEBOUNCE_DELAY = 100;  // 100ms debounce
    unsigned long lastStateChangeTime;
    
    // Detection hysteresis
    static const int DETECTION_THRESHOLD = 500;  // Analog threshold value
    
public:
    // Constructor
    IRSensor(int pin);
    
    // Initialize sensor
    void begin();
    
    // Read sensor value
    int readSensorValue();
    
    // Check if object is detected
    bool isObjectDetected();
    
    // Check if a new object was detected (edge detection)
    bool objectDetectedEdge();
    
    // Check if object left (no longer detected)
    bool objectLeftEdge();
    
    // Get time since last detection
    unsigned long getTimeSinceDetection();
    
    // Reset detection state
    void resetDetection();
    
    // Update sensor state (call in loop)
    void update();
    
    // Get detection state change flag
    bool hasStateChanged();
    
    // Get current object status string
    String getStatusString();
    
    // Set detection threshold (for analog IR sensors)
    void setDetectionThreshold(int threshold);
    
    // Get raw sensor reading
    int getRawValue();
};

#endif
