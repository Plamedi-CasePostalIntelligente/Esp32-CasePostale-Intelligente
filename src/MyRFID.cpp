#include "MyRFID.h"

MyRFID::MyRFID() {
    SPI.begin();      // Init SPI bus
    rfid.PCD_Init();  // Init MFRC522
}

MyRFID::~MyRFID() {
}

bool MyRFID::IsRFIDDetected() {
    byte v = rfid.PCD_ReadRegister(MFRC522::VersionReg);
    Serial.print("Version du module RFID: 0x");
    Serial.println(v, HEX);
    
    if (v == 0x00 || v == 0xFF) {
        Serial.println("ERREUR: Impossible de détecter le module RFID!");
        Serial.println("Vérifiez vos connexions.");
        return false;
    } else {
        Serial.println("Module RFID détecté avec succès!");
        return true;
    }
}

String MyRFID::ReadCardSerial() {
    if (rfid.PICC_IsNewCardPresent()) {
        if (rfid.PICC_ReadCardSerial()) {
            Serial.println("Carte détectée!");
            MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
            Serial.print("RFID/NFC Tag Type: ");
            Serial.println(rfid.PICC_GetTypeName(piccType));

            // Construit la chaîne hexadécimale
            String uidString = "";
            for (int i = 0; i < rfid.uid.size; i++) {
                if (rfid.uid.uidByte[i] < 0x10) {
                    uidString += "0"; // Ajoute un 0 pour les valeurs < 16
                }
                uidString += String(rfid.uid.uidByte[i], HEX); // Ajoute la valeur en hex
            }
            uidString.toUpperCase(); // Met en majuscules pour uniformité 

            // Affiche l'UID pour vérification
            Serial.print("UID: ");
            Serial.println(uidString);

            rfid.PICC_HaltA();      // Arrête la carte
            rfid.PCD_StopCrypto1(); // Arrête le chiffrement
            return uidString;       // Retourne la chaîne
        }
    }
    delay(50); // Petit délai pour stabiliser
    return ""; // Retourne une chaîne vide si rien n’est lu
}