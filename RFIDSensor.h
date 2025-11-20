#ifndef RFID_SENSOR_H
#define RFID_SENSOR_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RFIDSensor {
private:
    MFRC522 mfrc522;
    String authorizedUID;
    unsigned long lastReadTime;
    unsigned long readInterval;
    bool cardDetected;
    String currentUID;
    
    unsigned long gateOpenTime;
    static const unsigned long GATE_OPEN_DURATION = 5000;  
    
public:
    RFIDSensor(int ssPin, int rstPin, String uid);

    void begin();

    bool readCard();

    String getCurrentUID();

    bool isAuthorized();

    bool isCardDetected();

    bool checkAndAuthorize();

    void resetGateTimer();

    bool shouldCloseGate();

    String getAuthorizedUID();

    void setAuthorizedUID(String uid);

    void printCardInfo();

    static String byteArrayToString(byte *buffer, byte bufferSize);
};

#endif
