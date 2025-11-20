/*
 * Ultrasonic Parking System
 * Detects approaching cars and provides audio feedback via buzzer
 * Buzzer frequency increases as car gets closer
 * 
 * Pin Configuration:
 * - Trigger Pin: 9  (Ultrasonic Sensor)
 * - Echo Pin: 10    (Ultrasonic Sensor)
 * - Buzzer Pin: 11  (Active buzzer or piezo speaker)
 * 
 * Distance Zones:
 * - 0-15cm:   DANGER   - Constant high frequency (2500 Hz)
 * - 15-30cm:  CRITICAL - Fast increasing tone (1500-2500 Hz)
 * - 30-100cm: WARNING  - Moderate tone (500-1500 Hz)
 * - >100cm:   SAFE     - No sound
 */

#include "UltrasonicSensor.h"

const int TRIGGER_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 11;

UltrasonicSensor parkingSensor(TRIGGER_PIN, ECHO_PIN, BUZZER_PIN);

unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 500;  

void setup() {
    Serial.begin(9600);
    
    parkingSensor.begin();
    
    Serial.println("=== Ultrasonic Parking System Started ===");
    Serial.println("Distance zones:");
    Serial.println("  0-15cm:   DANGER   - Constant high frequency");
    Serial.println("  15-30cm:  CRITICAL - Fast increasing tone");
    Serial.println("  30-100cm: WARNING  - Moderate tone");
    Serial.println("  >100cm:   SAFE     - No sound");
    Serial.println();
}

void loop() {
    parkingSensor.updateBuzzer();
    
    if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
        long distance = parkingSensor.getCurrentDistance();
        
        if (distance > 0) {  
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.print(" cm | Status: ");
            
            if (distance <= 15) {
                Serial.println("[DANGER!] STOP!");
            } else if (distance <= 30) {
                Serial.println("[CRITICAL] Very Close!");
            } else if (distance <= 100) {
                Serial.println("[WARNING] Approaching!");
            } else {
                Serial.println("[SAFE] Too far");
            }
        } else {
            Serial.println("Waiting for first measurement...");
        }
        
        lastDisplayTime = millis();
    }

    delay(10);
}

/*
 * Alternative loop with LED indicator (optional)
 * Uncomment to use with LEDs for visual feedback
 * 
 * const int GREEN_LED = 6;
 * const int YELLOW_LED = 7;
 * const int RED_LED = 8;
 * 
 * void setup() {
 *   pinMode(GREEN_LED, OUTPUT);
 *   pinMode(YELLOW_LED, OUTPUT);
 *   pinMode(RED_LED, OUTPUT);
 *   parkingSensor.begin();
 *   Serial.begin(9600);
 * }
 * 
 * void loop() {
 *   parkingSensor.updateBuzzer();
 *   long distance = parkingSensor.getCurrentDistance();
 *   
 *   // Update LEDs
 *   digitalWrite(GREEN_LED, LOW);
 *   digitalWrite(YELLOW_LED, LOW);
 *   digitalWrite(RED_LED, LOW);
 *   
 *   if (distance > 100) {
 *     digitalWrite(GREEN_LED, HIGH);
 *   } else if (distance > 30) {
 *     digitalWrite(YELLOW_LED, HIGH);
 *   } else {
 *     digitalWrite(RED_LED, HIGH);
 *   }
 *   
 *   delay(10);
 * }
 */
