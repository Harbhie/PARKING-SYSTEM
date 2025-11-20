/*
 * RFID Parking Gate System
 * Authorizes cars via RFID cards and controls servo motor lever
 * 
 * Pin Configuration:
 * - SDA (RFID): 10  (Chip Select for MFRC522)
 * - SCK (RFID): 13  (SPI Clock)
 * - MOSI (RFID): 11 (SPI MOSI)
 * - MISO (RFID): 12 (SPI MISO)
 * - RST (RFID): 9   (Reset pin for MFRC522)
 * - Servo: 8        (PWM signal for servo motor)
 * 
 * Operation:
 * 1. Scan RFID card
 * 2. If authorized: servo moves UP (gate opens)
 * 3. After 5 seconds or car passes: servo moves DOWN (gate closes)
 */

#include "RFIDSensor.h"
#include "ServoGate.h"

const int RFID_SS_PIN = 10;     
const int RFID_RST_PIN = 9;      
const int SERVO_PIN = 8;         

const String AUTHORIZED_UID = "7B 80 6C 87";

RFIDSensor rfidSensor(RFID_SS_PIN, RFID_RST_PIN, AUTHORIZED_UID);
ServoGate gate(SERVO_PIN, 90, 0);  

enum GateState {
    GATE_CLOSED,
    GATE_OPENING,
    GATE_OPEN,
    GATE_CLOSING
};

GateState currentState = GATE_CLOSED;
unsigned long stateChangeTime = 0;

unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 500;

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("\n=== RFID Parking Gate System Started ===");
    Serial.println("Initializing RFID sensor...");
    
    rfidSensor.begin();
    
    Serial.println("Initializing servo gate...");

    gate.begin();
    
    Serial.println("\n--- System Ready ---");
    Serial.println("Please scan an RFID card");
    Serial.println();
}

void loop() {
    gate.update();
 
    if (rfidSensor.readCard()) {
        rfidSensor.printCardInfo();
        
        if (rfidSensor.isAuthorized()) {
            Serial.println("✓ Card authorized! Opening gate...");
            currentState = GATE_OPENING;
            gate.openGate();
            rfidSensor.resetGateTimer();
            stateChangeTime = millis();
        } else {
            Serial.println("✗ Card NOT authorized! Access denied!");
            Serial.println();
        }
    }

    switch (currentState) {
        case GATE_CLOSED:
            break;
            
        case GATE_OPENING:
            if (!gate.isServoMoving()) {
                currentState = GATE_OPEN;
                Serial.println("Gate is now open - Car can pass");
                stateChangeTime = millis();
            }
            break;
            
        case GATE_OPEN:
            if (rfidSensor.shouldCloseGate()) {
                Serial.println("Timeout reached or car has passed - Closing gate...");
                currentState = GATE_CLOSING;
                gate.closeGate();
                stateChangeTime = millis();
            }
            break;
            
        case GATE_CLOSING:
            if (!gate.isServoMoving()) {
                currentState = GATE_CLOSED;
                Serial.println("Gate is now closed");
                Serial.println("Ready for next vehicle\n");
            }
            break;
    }
    
    if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
        displayStatus();
        lastDisplayTime = millis();
    }
    
    delay(10);
}

void displayStatus() {
    Serial.print("Status: ");
    
    switch (currentState) {
        case GATE_CLOSED:
            Serial.print("CLOSED");
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
    }
    
    Serial.print(" | Servo angle: ");
    Serial.print(gate.getCurrentAngle());
    Serial.println("°");
}

/*
 * CONFIGURATION NOTES:
 * 
 * 1. To find your RFID card UID:
 *    - Uncomment the code in readCard() or add a debug print
 *    - Scan your card and note the UID shown in Serial Monitor
 *    - Update AUTHORIZED_UID with the correct value
 * 
 * 2. Servo angles may need adjustment:
 *    - Test and find the angles that work with your mechanical setup
 *    - Adjust gate.begin() parameters: ServoGate(pin, openAngle, closedAngle)
 * 
 * 3. Gate open duration (5 seconds):
 *    - Modify GATE_OPEN_DURATION in RFIDSensor.h if needed
 * 
 * 4. SPI Pins (often fixed by Arduino board):
 *    - Most Arduino boards: MOSI=11, MISO=12, SCK=13
 *    - Verify your specific board's pin layout
 */
