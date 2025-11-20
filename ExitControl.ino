/*
 * IR Sensor Parking Exit System
 * Detects car passing through exit point and controls servo motor barrier
 * 
 * Pin Configuration:
 * - IR Sensor: A0     (Analog or Digital IR sensor)
 * - Servo: 8          (PWM signal for servo motor)
 * 
 * Operation:
 * 1. IR sensor detects car approaching/passing
 * 2. Servo motor opens UP (barrier lifts/lever goes up)
 * 3. Car passes through
 * 4. After 5 seconds or no detection: servo closes DOWN (barrier lowers)
 * 
 * IR Sensor Types Supported:
 * - Digital IR sensor (HIGH/LOW output)
 * - Analog IR sensor (distance measurement 0-1023)
 * - Change readSensorValue() in IRSensor.cpp to use analogRead() if needed
 */

#include "IRSensor.h"
#include "ExitServoGate.h"

// Pin definitions
const int IR_SENSOR_PIN = A0;      // Analog pin for IR sensor
const int SERVO_PIN = 8;           // PWM pin for servo

// Create objects
IRSensor irSensor(IR_SENSOR_PIN);
ExitServoGate exitGate(SERVO_PIN, 90, 0);  // Open: 90°, Closed: 0°

// State machine for exit control
enum ExitState {
    GATE_BLOCKED,        // Car hasn't reached sensor
    OBJECT_DETECTED,     // Car detected by IR sensor
    GATE_OPENING,        // Servo opening
    GATE_OPEN,          // Gate is open, car passing
    GATE_CLOSING,       // Servo closing after car passed
    GATE_FULLY_CLOSED   // Gate closed and ready
};

ExitState currentState = GATE_BLOCKED;
unsigned long stateChangeTime = 0;

// Timing variables
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 200;

// Additional timing
unsigned long objectDetectedTime = 0;
const unsigned long OBJECT_PERSISTENCE = 500;  // Keep gate open for 500ms after detection

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("\n=== IR Parking Exit System Started ===");
    Serial.println("Initializing IR sensor...");
    
    // Initialize IR sensor
    irSensor.begin();
    
    Serial.println("Initializing exit servo gate...");
    
    // Initialize servo gate
    exitGate.begin();
    
    // Set auto-close delay to 5 seconds
    exitGate.setAutoCloseDelay(5000);
    exitGate.enableAutoClose(true);
    
    Serial.println("\n--- System Ready ---");
    Serial.println("Waiting for vehicle at exit point...");
    Serial.println();
}

void loop() {
    // Update IR sensor state
    irSensor.update();
    
    // Update servo position
    exitGate.update();
    
    // Check for detection edge (car just arrived)
    if (irSensor.objectDetectedEdge()) {
        Serial.println("[EVENT] Car detected at exit point!");
        objectDetectedTime = millis();
        currentState = OBJECT_DETECTED;
    }
    
    // State machine for exit control
    switch (currentState) {
        case GATE_BLOCKED:
            // Waiting for car to arrive at sensor
            if (irSensor.isObjectDetected()) {
                Serial.println("[STATE] Object detected - Opening gate...");
                currentState = GATE_OPENING;
                exitGate.openGate();
                objectDetectedTime = millis();
                stateChangeTime = millis();
            }
            break;
            
        case OBJECT_DETECTED:
            // Car detected, start opening gate
            Serial.println("[STATE] Opening exit gate...");
            currentState = GATE_OPENING;
            exitGate.openGate();
            stateChangeTime = millis();
            break;
            
        case GATE_OPENING:
            // Gate is opening
            if (!exitGate.isServoMoving()) {
                currentState = GATE_OPEN;
                Serial.println("[STATE] Gate fully open - Car can exit");
                stateChangeTime = millis();
            }
            break;
            
        case GATE_OPEN:
            // Gate is open, monitor for car leaving or timeout
            // Check if object is still detected
            if (!irSensor.isObjectDetected()) {
                // Car has passed, start closing gate
                if (millis() - objectDetectedTime >= OBJECT_PERSISTENCE) {
                    Serial.println("[STATE] Car has passed - Closing gate...");
                    currentState = GATE_CLOSING;
                    exitGate.closeGate();
                    stateChangeTime = millis();
                }
            } else {
                // Car still detected, keep gate open
                if (millis() - stateChangeTime > 10000) {
                    // Safety: close after 10 seconds max
                    Serial.println("[SAFETY] Max open time reached - Closing gate...");
                    currentState = GATE_CLOSING;
                    exitGate.closeGate();
                    stateChangeTime = millis();
                }
            }
            break;
            
        case GATE_CLOSING:
            // Gate is closing
            if (!exitGate.isServoMoving()) {
                currentState = GATE_FULLY_CLOSED;
                Serial.println("[STATE] Gate fully closed - Ready for next vehicle");
                Serial.println();
                stateChangeTime = millis();
            }
            break;
            
        case GATE_FULLY_CLOSED:
            // Gate closed, ready for next vehicle
            if (irSensor.isObjectDetected()) {
                Serial.println("[EVENT] New vehicle detected...");
                currentState = GATE_BLOCKED;
            }
            break;
    }
    
    // Display status periodically
    if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
        displayStatus();
        lastDisplayTime = millis();
    }
    
    delay(10);
}

void displayStatus() {
    Serial.print("Status: ");
    
    switch (currentState) {
        case GATE_BLOCKED:
            Serial.print("BLOCKED");
            break;
        case OBJECT_DETECTED:
            Serial.print("DETECTED");
            break;
        case GATE_OPENING:
            Serial.print("OPENING");
            break;
        case GATE_OPEN:
            Serial.print("OPEN");
            break;
        case GATE_CLOSING:
            Serial.print("CLOSING");
            break;
        case GATE_FULLY_CLOSED:
            Serial.print("READY");
            break;
    }
    
    Serial.print(" | IR: ");
    Serial.print(irSensor.getStatusString());
    Serial.print(" | Servo: ");
    Serial.print(exitGate.getCurrentAngle());
    Serial.println("°");
}

/*
 * CONFIGURATION NOTES:
 * 
 * 1. IR Sensor Pin Selection:
 *    - Digital IR: Use digitalRead() - returns HIGH/LOW
 *    - Analog IR: Use analogRead() - returns 0-1023
 *    - Currently configured for DIGITAL mode
 *    - To use analog: modify IRSensor.cpp readSensorValue()
 * 
 * 2. IR Sensor Logic:
 *    - Most IR sensors: LOW = object detected, HIGH = clear
 *    - Adjust logic in IRSensor::update() if needed
 * 
 * 3. Servo Angles:
 *    - Test and find angles that work with your setup
 *    - Adjust in exitGate initialization (90°, 0°)
 * 
 * 4. Auto-close Timing:
 *    - Default: 5 seconds
 *    - Change via exitGate.setAutoCloseDelay(milliseconds)
 * 
 * 5. Debounce Time:
 *    - IR sensor debounce: 100ms (prevents false triggers)
 *    - Modify DEBOUNCE_DELAY in IRSensor.h if needed
 */
