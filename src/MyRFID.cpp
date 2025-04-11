#include "MyRFID.h"

MyRFID::MyRFID()
{
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
}

MyRFID::~MyRFID()
{
}

bool MyRFID::IsRFIDDetected()
{
    byte v = rfid.PCD_ReadRegister(MFRC522::VersionReg);
    Serial.print("Version du module RFID: 0x");
    Serial.println(v, HEX);

    if (v == 0x00 || v == 0xFF)
    {
        Serial.println("ERREUR: Impossible de détecter le module RFID!");
        Serial.println("Vérifiez vos connexions.");
        return false;
    }
    else
    {
        Serial.println("Module RFID détecté avec succès!");
        return true;
    }
}

String MyRFID::ReadCardSerial()
{
    String uidString = "";
    
    // Vérifie si une nouvelle carte est présente
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        Serial.println("Carte détectée!");
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.print("RFID/NFC Tag Type: ");
        Serial.println(rfid.PICC_GetTypeName(piccType));

        // Construit la chaîne UID
        for (int i = 0; i < rfid.uid.size; i++)
        {
            if (rfid.uid.uidByte[i] < 0x10)
            {
                uidString += "0";
            }
            uidString += String(rfid.uid.uidByte[i], HEX);
        }
        uidString.toUpperCase();
        
        // Arrête la communication avec la carte
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }
    
    return uidString;
}

void MyRFID::Reset()
{
    rfid.PCD_Init();       // Réinitialise le lecteur
    rfid.PCD_StopCrypto1(); // S'assure que la crypto est arrêtée
}