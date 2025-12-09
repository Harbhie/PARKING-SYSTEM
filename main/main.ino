// ----------------------------------------------------
// main.ino  (DUAL MODE + EVENT LOGGING + STATUS LOGGING)
// ----------------------------------------------------

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <HTTPClient.h>

#include "index_html.h"
#include "syscontroller.h"

// ---------------------------------------------------
// WiFi Settings
// ---------------------------------------------------
const char* ap_ssid     = "Parking-System";    
const char* ap_password = "12345678";

// STA → Connects to your hotspot
const char* sta_ssid     = "ConnectKa";
const char* sta_password = "magdangal";

// ---------------------------------------------------
// Flask + SQLite Server IP (Laptop IP)
// ---------------------------------------------------
const char* loggingServer = "192.168.116.18";
const int   loggingPort   = 5000;

// ---------------------------------------------------
// Web Servers
// ---------------------------------------------------
WebServer server(80);
WebSocketsServer webSocket(81);

// ---------------------------------------------------
// System Status Logging Timer
// ---------------------------------------------------
unsigned long lastStatusLog = 0;
const unsigned long STATUS_INTERVAL = 600000;   // 1 minute (change to 600000 for 10 minutes)

// ---------------------------------------------------
// Event flags from syscontroller
// ---------------------------------------------------
extern volatile bool evtGateOpen;
extern volatile bool evtGateClose;
extern volatile bool evtSlotOccupied;
extern volatile bool evtSlotAvailable;
extern volatile bool evtBuzzerOff;

extern float lastDistance;

// ---------------------------------------------------
// Collect system status snapshot
// ---------------------------------------------------
String collectSystemStatus() {
  String status = "";

  status += "IR1=" + String(digitalRead(irPin1) == LOW ? "active" : "idle");
  status += ",IR2=" + String(digitalRead(irPin2) == LOW ? "active" : "idle");

  status += ",UltrasonicDist=" + String(lastDistance, 2);

  status += ",Buzzer=" + String(isBuzzing ? "active" : "off");

  status += ",AlertServo=" + String(alertServo.read());

  status += ",Gate=" + String(count == 1 ? "open" : "closed");

  return status;
}

// ---------------------------------------------------
// Send log to Flask server using STA WiFi
// ---------------------------------------------------
void sendLogToServer(String event, String value) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Log] STA not connected.");
    return;
  }

  HTTPClient http;

  String url = "http://" + String(loggingServer) + ":" + String(loggingPort) +
               "/log?event=" + event + "&value=" + value;

  Serial.println("[HTTP] Sending → " + url);

  http.begin(url);
  int code = http.GET();

  if (code > 0) {
    Serial.println("[HTTP] Response: " + http.getString());
  } else {
    Serial.println("[HTTP] Failed to send log.");
  }

  http.end();
}

// ---------------------------------------------------
// Dashboard handler
// ---------------------------------------------------
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// ---------------------------------------------------
// WebSocket handler
// ---------------------------------------------------
void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {

  if (type == WStype_CONNECTED) {
    Serial.println("[WS] Client connected");
  }

  if (type == WStype_TEXT) {
    String msg = (char*)payload;

    if (msg == "open_request")  triggerGateOpen();
    if (msg == "close_request") triggerGateClose();
  }
}

// ---------------------------------------------------
// Dual WiFi Mode Setup (AP + STA)
// ---------------------------------------------------
void setupWiFiDual() {
  Serial.println("\n--- Starting WiFi Dual Mode ---");

  WiFi.mode(WIFI_AP_STA);

  // Start AP (Dashboard)
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("[AP] Dashboard WiFi → ");
  Serial.println(WiFi.softAPIP());

  // Connect STA (Hotspot for logging)
  Serial.print("[STA] Connecting to → ");
  Serial.println(sta_ssid);

  WiFi.begin(sta_ssid, sta_password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[STA] Connected!");
    Serial.print("[STA] ESP32 IP → ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[STA] Connection FAILED — Logs will NOT send.");
  }
}

// ---------------------------------------------------
// Setup
// ---------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  setupWiFiDual();

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWsEvent);

  systemSetup();

  Serial.println("\nSystem Ready.\n");
}

// ---------------------------------------------------
// Loop
// ---------------------------------------------------
void loop() {

  server.handleClient();
  webSocket.loop();
  systemLoop();

  // ---------------------- EVENT LOGGING ----------------------

  if (evtGateOpen) {
    evtGateOpen = false;
    webSocket.broadcastTXT("open");
    Serial.println("[WS] Gate Open");
    sendLogToServer("gate_open", "");
  }

  if (evtGateClose) {
    evtGateClose = false;
    webSocket.broadcastTXT("close");
    Serial.println("[WS] Gate Close");
    sendLogToServer("gate_close", "");
  }

  if (evtSlotOccupied) {
    evtSlotOccupied = false;

    char wsmsg[32];
    sprintf(wsmsg, "occupied:%.2f", lastDistance);

    webSocket.broadcastTXT(wsmsg);
    Serial.println(wsmsg);

    sendLogToServer("slot_occupied", String(lastDistance));
  }

  if (evtSlotAvailable) {
    evtSlotAvailable = false;
    webSocket.broadcastTXT("available");
    Serial.println("[WS] Available");
    sendLogToServer("slot_available", "");
  }

  if (evtBuzzerOff) {
    evtBuzzerOff = false;
    webSocket.broadcastTXT("buzzer_off");
    Serial.println("[WS] Buzzer Off");
    sendLogToServer("buzzer_off", "");
  }

  // ---------------------- PERIODIC STATUS LOG ----------------------

  if (millis() - lastStatusLog >= STATUS_INTERVAL) {
    lastStatusLog = millis();

    String sysStatus = collectSystemStatus();
    Serial.println("[STATUS] " + sysStatus);

    sendLogToServer("system_status", sysStatus);
  }
}
