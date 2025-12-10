// ---------------------------- syscontroller.cpp (fixed final) ---------------------------
#include "syscontroller.h"

static const int GATE_US_STOP  = 1500;
static const int GATE_US_CW    = 2000;
static const int GATE_US_CCW   = 1000;
static const int GATE_MOVE_MS  = 400;
static const int GATE_STOP_HOLD_MS = 80;

SystemController::SystemController()
: _rfid(5, 22),
  _rfidActive(true),
  _forwardTriggered(false),
  _backwardTriggered(false),
  _isBuzzing(false),
  _buzzStartTime(0),
  _occupiedState(false),
  _availableState(false),
  _gateState(0),
  _lastDistance(0.0f)
{
    _allowedTag1[0] = 0xF2; _allowedTag1[1] = 0xCB; _allowedTag1[2] = 0x12; _allowedTag1[3] = 0x01;
    _allowedTag2[0] = 0x22; _allowedTag2[1] = 0x73; _allowedTag2[2] = 0xB4; _allowedTag2[3] = 0xAB;
}

static void safeDetachServo(Servo &s, int pin) {
    s.detach();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void SystemController::disablePinsExceptGate() {
    pinMode(_irPin1, INPUT);
    pinMode(_irPin2, INPUT);
    pinMode(_irPin3, INPUT);

    pinMode(_trigPin, INPUT);
    pinMode(_echoPin, INPUT);

    digitalWrite(_buzzerPin, LOW);
    pinMode(_buzzerPin, INPUT);

    if (_rfidActive) {
        SPI.end();
        _rfidActive = false;
    }

    safeDetachServo(_alertServo, _alertServoPin);
    safeDetachServo(_gateServo, _gateServoPin);
}

void SystemController::enableGateOnly() {
    pinMode(_irPin1, INPUT);
    pinMode(_irPin2, INPUT);
    pinMode(_irPin3, INPUT);

    pinMode(_trigPin, INPUT);
    pinMode(_echoPin, INPUT);

    digitalWrite(_buzzerPin, LOW);
    pinMode(_buzzerPin, INPUT);

    if (_rfidActive) {
        SPI.end();
        _rfidActive = false;
    }

    safeDetachServo(_alertServo, _alertServoPin);
    safeDetachServo(_gateServo, _gateServoPin);
}

void SystemController::enableAllPins() {

    pinMode(_irPin1, INPUT);
    pinMode(_irPin2, INPUT);
    pinMode(_irPin3, INPUT);

    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);

    pinMode(_buzzerPin, OUTPUT);
    digitalWrite(_buzzerPin, LOW);

    if (!_rfidActive) {
        SPI.begin(18, 19, 23);
        _rfid.PCD_Init();
        _rfidActive = true;
    } else {
        SPI.begin(18, 19, 23);
        _rfid.PCD_Init();
    }

    _alertServo.attach(_alertServoPin);
    _alertServo.write(0);

    safeDetachServo(_gateServo, _gateServoPin);
}

void SystemController::setMode(int m) {
    _mode = m;

    if (m == 2) disablePinsExceptGate();
    else if (m == 1) enableGateOnly();
    else enableAllPins();
}

void SystemController::begin() {
    enableAllPins();
}

void SystemController::manualOpen() {
    if (_mode == 1) gateOpen();
}

void SystemController::manualClose() {
    if (_mode == 1) gateClose();
}

bool SystemController::checkRFID() {
    if (!_rfidActive) return false;
    if (!_rfid.PICC_IsNewCardPresent()) return false;
    if (!_rfid.PICC_ReadCardSerial()) return false;

    bool match1 = true;
    bool match2 = true;

    for (int i = 0; i < 4; i++) {
        if (_rfid.uid.uidByte[i] != _allowedTag1[i]) match1 = false;
        if (_rfid.uid.uidByte[i] != _allowedTag2[i]) match2 = false;
    }

    return (match1 || match2);
}

void SystemController::gateOpen() {
    if (_gateState == 0) {
        _gateState = 1;

        _gateServo.attach(_gateServoPin);
        _gateServo.writeMicroseconds(GATE_US_STOP);
        delay(20);

        _gateServo.writeMicroseconds(GATE_US_CW);
        delay(GATE_MOVE_MS);

        _gateServo.writeMicroseconds(GATE_US_STOP);
        delay(GATE_STOP_HOLD_MS);

        safeDetachServo(_gateServo, _gateServoPin);
    }
}

void SystemController::gateClose() {
    if (_gateState == 1) {
        _gateState = 0;

        _gateServo.attach(_gateServoPin);
        _gateServo.writeMicroseconds(GATE_US_STOP);
        delay(20);

        _gateServo.writeMicroseconds(GATE_US_CCW);
        delay(GATE_MOVE_MS);

        _gateServo.writeMicroseconds(GATE_US_STOP);
        delay(GATE_STOP_HOLD_MS);

        safeDetachServo(_gateServo, _gateServoPin);
    }
}

float SystemController::readDistance() {

    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    long duration = pulseIn(_echoPin, HIGH, 30000);

    if (duration > 0) {
        _lastDistance = (duration * 0.0343) / 2.0;
    }

    return _lastDistance;
}

SystemController::EventType SystemController::update() {

    if (_mode == 1) return EVT_NONE;
    if (_mode == 2) return EVT_NONE;

    float dist = readDistance();
    bool ir1 = (digitalRead(_irPin1) == LOW);
    bool ir2 = (digitalRead(_irPin2) == LOW);
    bool ir3 = (digitalRead(_irPin3) == LOW);
    bool rfid = checkRFID();

    if (_gateState == 0 && ir1 && rfid && !_forwardTriggered) {
        _forwardTriggered = true;
        gateOpen();
        return EVT_GATE_OPEN;
    }
    if (!ir1 && !rfid) _forwardTriggered = false;

    if (_gateState == 1 && ir2 && !_backwardTriggered) {
        _backwardTriggered = true;
        gateClose();
        return EVT_GATE_CLOSE;
    }
    if (!ir2) _backwardTriggered = false;

    if (dist < 10) {
        if (!_isBuzzing) {
            _isBuzzing = true;
            _buzzStartTime = millis();
            digitalWrite(_buzzerPin, HIGH);

            _occupiedState = true;
            _availableState = false;

            return EVT_SLOT_OCCUPIED;
        }
    }

    if (_isBuzzing && millis() - _buzzStartTime >= 5000) {
        _isBuzzing = false;
        digitalWrite(_buzzerPin, LOW);
        return EVT_BUZZER_OFF;
    }

    if (dist > 10 && !_isBuzzing) {
        if (ir3) {
            if (!_alertServo.attached()) _alertServo.attach(_alertServoPin);
            _alertServo.write(180);

            if (!_availableState) {
                _availableState = true;
                _occupiedState = false;
                return EVT_SLOT_AVAILABLE;
            }
        } else {
            if (!_alertServo.attached()) _alertServo.attach(_alertServoPin);
            _alertServo.write(0);
            _availableState = false;
        }
    }

    return EVT_NONE;
}

float SystemController::getDistance() const {
    return _lastDistance;
}

String SystemController::getSystemStatus() {
    String s = "";

    s += "Mode=";
    if (_mode == 0) s += "AUTO";
    else if (_mode == 1) s += "MAINTENANCE";
    else s += "SLEEP";

    s += ",IR1=";
    s += (digitalRead(_irPin1) == LOW ? "active" : "idle");

    s += ",IR2=";
    s += (digitalRead(_irPin2) == LOW ? "active" : "idle");

    s += ",IR3=";
    s += (digitalRead(_irPin3) == LOW ? "active" : "idle");

    s += ",UltrasonicDist=";
    s += String(_lastDistance, 2);

    s += ",Buzzer=";
    s += (_isBuzzing ? "active" : "off");

    s += ",AlertServo=";
    if (_alertServo.attached())
        s += String(_alertServo.read());
    else
        s += "detached";

    s += ",Gate=";
    s += (_gateState == 1 ? "open" : "closed");

    return s;
}
