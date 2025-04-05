#include <SPI.h>
#include <MFRC522.h>
#ifndef MYRFID_H
#define MYRFID_H

class MyRFID
{
private:
    #define SS_PIN  5  // ESP32 pin GPIO5 
    #define RST_PIN 17//27 // ESP32 pin GPIO27
    MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
    
public:
    MyRFID();
    ~MyRFID();
    bool IsRFIDDetected();
    String ReadCardSerial();
    
    
};
#endif