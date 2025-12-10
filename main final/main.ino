// ------------------- ver final: Software Sleep (No Reboot) --------------------
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <HTTPClient.h>

#include "index_html.h"
#include "syscontroller.h"

// MODE
// 0 = AUTO
// 1 = MAINTENANCE
// 2 = SLEEP
int systemMode = 0;

// WiFi Settings
const char* ap_ssid     = "Parking-System";
const char* ap_password = "12345678";

const char* sta_ssid     = "ConnectKa";
const char* sta_password = "magdangal";

// Logging server
const char* loggingServer = "192.168.116.18";
const int loggingPort     = 5000;

WebServer server(80);
WebSocketsServer webSocket(81);

SystemController controller;

unsigned long lastStatusLog = 0;
const unsigned long STATUS_INTERVAL = 600000;

// ---------------------------------------------------
void sendLogToServer(String event, String value) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Log] STA not connected.");
        return;
    }

    HTTPClient http;
    String url = "http://" + String(loggingServer) + ":" +
                 String(loggingPort) + "/log?event=" +
                 event + "&value=" + value;

    http.begin(url);
    http.GET();
    http.end();
}

// ---------------------------------------------------
void handleRoot() {
    server.send_P(200, "text/html", index_html);
}

// ---------------------------------------------------
void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {

    if (type == WStype_TEXT) {

        String msg = String((char*)payload);

        if (msg == "open_request") {
            controller.manualOpen();
        }
        else if (msg == "close_request") {
            controller.manualClose();
        }
        else if (msg == "mode_auto") {
            systemMode = 0;
            controller.setMode(0);
            Serial.println("[MODE] AUTO");

            sendLogToServer("mode", "AUTO");
        }
        else if (msg == "mode_maint") {
            systemMode = 1;
            controller.setMode(1);
            Serial.println("[MODE] MAINTENANCE");

            sendLogToServer("mode", "MAINTENANCE");
        }
        else if (msg == "mode_sleep") {
            systemMode = 2;
            controller.setMode(2);
            Serial.println("[MODE] SLEEP");

            sendLogToServer("mode", "SLEEP");
        }
    }
}

// ---------------------------------------------------
void setupWiFiDual() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);

    WiFi.begin(sta_ssid, sta_password);
    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 10000) {
        delay(200);
    }
}

// ---------------------------------------------------
void setup() {
    Serial.begin(115200);

    setupWiFiDual();

    server.on("/", handleRoot);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(onWsEvent);

    controller.begin();

    Serial.println("System Ready.");
}

// ---------------------------------------------------
void loop() {

    server.handleClient();
    webSocket.loop();

    if (systemMode == 2) return;     // SLEEP
    if (systemMode == 1) return;     // MAINTENANCE

    // AUTO
    SystemController::EventType evt = controller.update();

    switch (evt) {

        case SystemController::EVT_GATE_OPEN:
            webSocket.broadcastTXT("open");
            sendLogToServer("gate_open", "");
            break;

        case SystemController::EVT_GATE_CLOSE:
            webSocket.broadcastTXT("close");
            sendLogToServer("gate_close", "");
            break;

        case SystemController::EVT_SLOT_OCCUPIED:
            webSocket.broadcastTXT("occupied:" + String(controller.getDistance(), 2));
            sendLogToServer("slot_occupied", String(controller.getDistance(), 2));
            break;

        case SystemController::EVT_SLOT_AVAILABLE:
            webSocket.broadcastTXT("available");
            sendLogToServer("slot_available", "");
            break;

        case SystemController::EVT_BUZZER_OFF:
            webSocket.broadcastTXT("buzzer_off");
            sendLogToServer("buzzer_off", "");
            break;

        default:
            break;
    }

    if (millis() - lastStatusLog >= STATUS_INTERVAL) {
        lastStatusLog = millis();
        String sysStatus = controller.getSystemStatus();
        sendLogToServer("system_status", sysStatus);
    }
}
