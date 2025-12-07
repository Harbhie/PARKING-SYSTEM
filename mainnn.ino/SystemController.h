#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>
#include <MFRC522.h>

class SystemController {

public:
    SystemController();
    void begin();
    void run();

private:
    // === Hardware Pins ===
    const int irPin1 = 15;
    const int irPin2 = 14;
    const int servoPin = 13;

    // === WiFi Access Point Credentials ===
    const char* ap_ssid = "ParkingGate_AP";
    const char* ap_password = "12345678";

    // === Web Server & WebSocket ===
    WebServer webServer = WebServer(80);     // <--- DECLARED HERE
    WebSocketsServer webSocket = WebSocketsServer(81);

    // === RFID ===
    MFRC522 rfid;
    static const byte allowedTag1[4];
    static const byte allowedTag2[4];

    // === Helpers ===
    Servo myServo;

    bool hasTriggeredForward = false;
    bool hasTriggeredBackward = false;

    int count = 0;

    bool checkRFID();
    void handleWebsocketMessage(String cmd);
};

#endif
