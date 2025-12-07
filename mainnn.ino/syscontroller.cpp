#include "SystemController.h"
#include <SPI.h>
#include "index_html.h"   // uses index_html[] from your file

const byte SystemController::allowedTag1[4] = {0xF2, 0xCB, 0x12, 0x01};
const byte SystemController::allowedTag2[4] = {0x22, 0x73, 0xB4, 0xAB};

// Constructor
SystemController::SystemController()
    : rfid(5, 22)
{}

// Begin — ESP32 Access Point + WebServer + WebSocket
void SystemController::begin() {
    Serial.begin(115200);

    // ===== FORCE ESP32 INTO AP MODE =====
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    delay(500);

    Serial.println("Access Point Started!");
    Serial.print("SSID: "); Serial.println(ap_ssid);
    Serial.print("Password: "); Serial.println(ap_password);
    Serial.print("AP IP Address: "); Serial.println(WiFi.softAPIP());

    // ===== HTTP SERVER (SERVE DASHBOARD UI) =====
    webServer.on("/", [this]() {
        this->webServer.send_P(200, "text/html", index_html);   // FIXED HERE
    });

    webServer.begin();
    Serial.println("HTTP Server Started");

    // ===== WEBSOCKET SERVER =====
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        if (type == WStype_TEXT) {
            String cmd = (char*)payload;
            this->handleWebsocketMessage(cmd);
        }
    });

    Serial.println("WebSocket Started");

    // ===== HARDWARE SETUP =====
    pinMode(irPin1, INPUT);
    pinMode(irPin2, INPUT);

    SPI.begin(18, 19, 23);
    rfid.PCD_Init();

    myServo.attach(servoPin);
    myServo.detach();

    Serial.println("System Ready...");
}

// Handle incoming browser commands
void SystemController::handleWebsocketMessage(String cmd) {
    if (cmd == "open") {
        Serial.println("Browser requested OPEN");

        myServo.attach(servoPin);
        myServo.write(0);
        delay(400);
        myServo.detach();

        count = 1;
        hasTriggeredForward = true;

        webSocket.broadcastTXT("open");
    }

    if (cmd == "close") {
        Serial.println("Browser requested CLOSE");

        myServo.attach(servoPin);
        myServo.write(180);
        delay(400);
        myServo.detach();

        count = 0;
        hasTriggeredBackward = true;

        webSocket.broadcastTXT("close");
    }
}

// RFID check
bool SystemController::checkRFID() {
    if (!rfid.PICC_IsNewCardPresent()) return false;
    if (!rfid.PICC_ReadCardSerial()) return false;

    bool match1 = true;
    bool match2 = true;

    for (byte i = 0; i < 4; i++) {
        if (rfid.uid.uidByte[i] != allowedTag1[i]) match1 = false;
        if (rfid.uid.uidByte[i] != allowedTag2[i]) match2 = false;
    }

    return (match1 || match2);
}

// Main loop
void SystemController::run() {
    this->webServer.handleClient(); 
    this->webSocket.loop();         

    bool ir1Detected = (digitalRead(irPin1) == LOW);
    bool ir2Detected = (digitalRead(irPin2) == LOW);
    bool rfidDetected = checkRFID();

    // Forward Trigger (OPEN GATE)
    if (count == 0 && ir1Detected && rfidDetected && !hasTriggeredForward) {
        hasTriggeredForward = true;
        count = 1;

        myServo.attach(servoPin);
        myServo.write(0);
        delay(400);
        myServo.detach();

        Serial.println("Forward Triggered");
        webSocket.broadcastTXT("open");
    }

    if (!ir1Detected && !rfidDetected) {
        hasTriggeredForward = false;
    }

    // Backward Trigger (CLOSE GATE)
    if (count == 1 && ir2Detected && !hasTriggeredBackward) {
        hasTriggeredBackward = true;
        count = 0;

        myServo.attach(servoPin);
        myServo.write(180);
        delay(400);
        myServo.detach();

        Serial.println("Backward Triggered");
        webSocket.broadcastTXT("close");
    }

    if (!ir2Detected) {
        hasTriggeredBackward = false;
    }
}
