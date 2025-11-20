#ifndef EXIT_SERVO_GATE_H
#define EXIT_SERVO_GATE_H

#include <Arduino.h>
#include <Servo.h>

class ExitServoGate {
private:
    Servo servo;
    int servoPin;
    
    // Servo angle positions
    int gateOpenAngle;    // Angle when gate is up (open for exit)
    int gateClosedAngle;  // Angle when gate is down (blocking)
    
    // Current state
    bool isOpen;
    unsigned long lastMoveTime;
    static const int SERVO_MOVE_DELAY = 50;  // 50ms between angle increments
    
    // Movement tracking
    int currentAngle;
    int targetAngle;
    bool isMoving;
    
    // Auto-close timing
    unsigned long gateOpenTime;
    unsigned long autoCloseDelay;
    bool autoCloseEnabled;
    
public:
    // Constructor
    ExitServoGate(int pin, int openAngle = 90, int closedAngle = 0);
    
    // Initialize servo
    void begin();
    
    // Open gate (move lever up)
    void openGate();
    
    // Close gate (move lever down)
    void closeGate();
    
    // Update servo position (call in loop for smooth movement)
    void update();
    
    // Check if gate is open
    bool getIsOpen();
    
    // Check if servo is moving
    bool isServoMoving();
    
    // Set custom angles
    void setAngles(int openAngle, int closedAngle);
    
    // Get current servo angle
    int getCurrentAngle();
    
    // Immediate move to angle (no smooth transition)
    void moveToAngle(int angle);
    
    // Stop servo movement
    void stopServo();
    
    // Set auto-close delay (milliseconds)
    void setAutoCloseDelay(unsigned long delayMs);
    
    // Check if gate should auto-close
    bool shouldAutoClose();
    
    // Enable/disable auto-close
    void enableAutoClose(bool enable);
    
    // Detach servo (power saving)
    void detach();
    
    // Attach servo again
    void attach();
};

#endif
