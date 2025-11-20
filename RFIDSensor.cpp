#include "RFIDSensor.h"

RFIDSensor::RFIDSensor(int ssPin, int rstPin, String uid)
    : mfrc522(ssPin, rstPin), authorizedUID(uid),
      lastReadTime(0), readInterval(500),
      cardDetected(false), currentUID(""),
      gateOpenTime(0) {
}

void RFIDSensor::begin() {
    SPI.begin();           
    mfrc522.PCD_Init();    

    Serial.println("RFID Sensor initialized");
    Serial.print("Authorized UID: ");
    Serial.println(authorizedUID);
}

bool RFIDSensor::readCard() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        if (cardDetected) {
            cardDetected = false;
            currentUID = "";
        }
        return false;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return false;
    }

    currentUID = byteArrayToString(mfrc522.uid.uidByte, mfrc522.uid.size);
    cardDetected = true;
    lastReadTime = millis();
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    
    return true;
}

String RFIDSensor::getCurrentUID() {
    return currentUID;
}

bool RFIDSensor::isAuthorized() {
    return (currentUID == authorizedUID);
}

bool RFIDSensor::isCardDetected() {
    return cardDetected;
}

bool RFIDSensor::checkAndAuthorize() {
    if (readCard()) {
        if (isAuthorized()) {
            gateOpenTime = millis();
            return true;
        }
    }
    return false;
}

void RFIDSensor::resetGateTimer() {
    gateOpenTime = millis();
}

bool RFIDSensor::shouldCloseGate() {
    if (gateOpenTime == 0) {
        return true; 
    }
    if (millis() - gateOpenTime >= GATE_OPEN_DURATION) {
        gateOpenTime = 0;
        return true;
    }
    
    return false;
}

String RFIDSensor::getAuthorizedUID() {
    return authorizedUID;
}

void RFIDSensor::setAuthorizedUID(String uid) {
    authorizedUID = uid;
    Serial.print("Authorized UID updated to: ");
    Serial.println(authorizedUID);
}

void RFIDSensor::printCardInfo() {
    Serial.println("Card detected:");
    Serial.print("  UID: ");
    Serial.println(currentUID);
    Serial.print("  Status: ");
    if (isAuthorized()) {
        Serial.println("AUTHORIZED");
    } else {
        Serial.println("NOT AUTHORIZED");
    }
}

String RFIDSensor::byteArrayToString(byte *buffer, byte bufferSize) {
    String result = "";
    for (byte i = 0; i < bufferSize; i++) {
        if (buffer[i] < 0x10) {
            result += "0";
        }
        result += String(buffer[i], HEX);
        if (i < bufferSize - 1) {
            result += " ";
        }
    }
    result.toUpperCase();
    return result;
}
