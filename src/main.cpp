#include <Arduino.h>
#include "MyWifi.h"
#include "MyAPI.h"
#include "MyMQTTManager.h"
#include "MyUltrasonique.h"
#include <WiFi.h>
#include "MyRFID.h"
#include "MySolenoide.h"
#include "MyOled.h"
#include "MyTemp.h"
#include "MyButton.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define BTN_RIGHT 4 // Bouton droit
#define BTN_LEFT 16
int RELAY_PIN_SOLENOIDE1 = 26; // GPIO26 pour Casier 1
int RELAY_PIN_SOLENOIDE2 = 27; // GPIO27 pour Casier 2

// Topics MQTT
const char *ULTRASONIC_LATENCY_TOPIC = "casier1/ultrasonic/test/latency";
const char *ULTRASONIC_REPEATABILITY_TOPIC = "casier1/ultrasonic/test/repeatability";

// Variables pour les tests
unsigned long lastTestTime = 0;
const unsigned long TEST_INTERVAL = 10000; // Test toutes les 10 secondes
float previousDistance1 = 0, previousDistance2 = 0;
unsigned long changeStartTime1 = 0, changeStartTime2 = 0;
bool measuringLatency1 = false, measuringLatency2 = false;

// Buffer circulaire pour la répétabilité
const int BUFFER_SIZE = 10;
float distanceBuffer1[BUFFER_SIZE];
float distanceBuffer2[BUFFER_SIZE];
int bufferIndex1 = 0, bufferIndex2 = 0;
bool bufferFull1 = false, bufferFull2 = false;

MyWifi *mywifi;
MyAPI *myapi;
MyMQTTManager *mymqttmanager;
MyUltrasonique *myUltrasonique1; // Casier 1
MyUltrasonique *myUltrasonique2; // Casier 2
MyRFID *myRFID;
MySolenoide *mySolenoide1 = NULL;
MySolenoide *mySolenoide2 = NULL;
MyTemp *mytemp = NULL;
MyOled *myOled = NULL;
MyButton *myButtonLEFT = NULL;
MyButton *myButtonRIGHT = NULL;

int buttonleftState;
int buttonrightState;
float temperature;
const char *etatCasier1 = "Inconnu";
const char *etatCasier2 = "Inconnu";
const char *etatWifi;
String currentUid = "";
unsigned long solenoidOpenTime = 0;
const unsigned long SOLENOID_TIMEOUT = 5000; // 5 secondes
unsigned long lastComponentCheck = 0;
const unsigned long CHECK_INTERVAL = 20000; // Vérifier toutes les 20 secondes
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000; // Reconnexion toutes les 5 secondes

// Variables pour suivre les états précédents
bool prevBoxEmpty1 = true;
bool prevBoxEmpty2 = true;

// void setupUltrasonicTests() {
//     for (int i = 0; i < BUFFER_SIZE; i++) {
//         distanceBuffer1[i] = 0;
//         distanceBuffer2[i] = 0;
//     }
// }

// void runUltrasonicTests() {
//     if (millis() - lastTestTime < TEST_INTERVAL) {
//         return;
//     }

//     float currentDistance1 = myUltrasonique1->GetDistance();
//     float currentDistance2 = myUltrasonique2->GetDistance();

//     // Test de latence pour Casier 1
//     if (abs(currentDistance1 - previousDistance1) > 1.0 && !measuringLatency1) {
//         changeStartTime1 = millis();
//         measuringLatency1 = true;
//     } else if (measuringLatency1 && abs(currentDistance1 - previousDistance1) < 0.5) {
//         unsigned long latency = millis() - changeStartTime1;
//         mymqttmanager->publish(ULTRASONIC_LATENCY_TOPIC, String(latency).c_str());
//         measuringLatency1 = false;
//     }

//     // Test de latence pour Casier 2
//     if (abs(currentDistance2 - previousDistance2) > 1.0 && !measuringLatency2) {
//         changeStartTime2 = millis();
//         measuringLatency2 = true;
//     } else if (measuringLatency2 && abs(currentDistance2 - previousDistance2) < 0.5) {
//         unsigned long latency = millis() - changeStartTime2;
//         mymqttmanager->publish(ULTRASONIC_LATENCY_TOPIC, String(latency).c_str());
//         measuringLatency2 = false;
//     }

//     // Test de répétabilité pour Casier 1
//     distanceBuffer1[bufferIndex1] = currentDistance1;
//     bufferIndex1 = (bufferIndex1 + 1) % BUFFER_SIZE;
//     if (bufferIndex1 == 0) bufferFull1 = true;

//     if (bufferFull1) {
//         float sum = 0;
//         for (int i = 0; i < BUFFER_SIZE; i++) {
//             sum += distanceBuffer1[i];
//         }
//         float mean = sum / BUFFER_SIZE;
//         float variance = 0;
//         for (int i = 0; i < BUFFER_SIZE; i++) {
//             variance += pow(distanceBuffer1[i] - mean, 2);
//         }
//         variance /= BUFFER_SIZE;
//         mymqttmanager->publish(ULTRASONIC_REPEATABILITY_TOPIC, String(variance).c_str());
//     }

//     // Test de répétabilité pour Casier 2
//     distanceBuffer2[bufferIndex2] = currentDistance2;
//     bufferIndex2 = (bufferIndex2 + 1) % BUFFER_SIZE;
//     if (bufferIndex2 == 0) bufferFull2 = true;

//     if (bufferFull2) {
//         float sum = 0;
//         for (int i = 0; i < BUFFER_SIZE; i++) {
//             sum += distanceBuffer2[i];
//         }
//         float mean = sum / BUFFER_SIZE;
//         float variance = 0;
//         for (int i = 0; i < BUFFER_SIZE; i++) {
//             variance += pow(distanceBuffer2[i] - mean, 2);
//         }
//         variance /= BUFFER_SIZE;
//         mymqttmanager->publish(ULTRASONIC_REPEATABILITY_TOPIC, String(variance).c_str());
//     }

//     previousDistance1 = currentDistance1;
//     previousDistance2 = currentDistance2;
//     lastTestTime = millis();
// }

void checkComponents() {
    if (mytemp && !mytemp->init()) {
        mymqttmanager->publishTempStatus("Erreur: Capteur de température non détecté");
    } else if (mytemp && mytemp->init()) {
        mymqttmanager->publishTempStatus("Capteur de température opérationnel");
    }

    if (myOled && !myOled->isOperational()) {
        mymqttmanager->publishOledStatus("Erreur: Écran OLED non détecté");
    } else if (myOled && myOled->isOperational()) {
        mymqttmanager->publishOledStatus("Écran OLED opérationnel");
    }

    if (myRFID && !myRFID->IsRFIDDetected()) {
        mymqttmanager->publishRfidStatus("Erreur: Module RFID non détecté");
    } else if (myRFID && myRFID->IsRFIDDetected()) {
        mymqttmanager->publishRfidStatus("Module RFID opérationnel");
    }

    if (myUltrasonique1 && !myUltrasonique1->IsSensorResponsive()) {
        mymqttmanager->publishUltrasonicStatus("Erreur: Capteur ultrasonique 1 non détecté");
    } else if (myUltrasonique1 && myUltrasonique1->IsSensorResponsive()) {
        mymqttmanager->publishUltrasonicStatus("Capteur ultrasonique 1 opérationnel");
    }

    if (myUltrasonique2 && !myUltrasonique2->IsSensorResponsive()) {
        mymqttmanager->publishUltrasonicStatus("Erreur: Capteur ultrasonique 2 non détecté");
    } else if (myUltrasonique2 && myUltrasonique2->IsSensorResponsive()) {
        mymqttmanager->publishUltrasonicStatus("Capteur ultrasonique 2 opérationnel");
    }
}

void handleRFIDLogic(String uid) {
    if (!myapi->checkUserExists(uid)) {
        Serial.println("Utilisateur non existant, enregistrement de la tentative...");
        myapi->insertAccessTries(uid, false);
        return;
    } else {
        myapi->insertAccessTries(uid, true);
    }

    if (!myapi->verifyIfFactor(uid)) {
        Serial.println("Erreur lors de la vérification du type d'utilisateur");
        return;
    }
    bool isFacteur = myapi->getIsFacteur();
    Serial.println("Utilisateur est " + String(isFacteur ? "facteur" : "client"));

    if (!myapi->verifyIfHasDelivery(uid)) {
        Serial.println("Aucune commande associée, arrêt");
        return;
    }

    if (!myapi->isDelivered(uid)) {
        Serial.println("Erreur lors de la vérification de la livraison");
        return;
    }
    bool isDelivered = myapi->getIsDelivered();
    if (isDelivered) {
        Serial.println("Commande déjà livrée, arrêt");
        return;
    }

    bool boxEmpty1 = myUltrasonique1->IsBoxEmpty();
    bool boxEmpty2 = myUltrasonique2->IsBoxEmpty();
    etatCasier1 = boxEmpty1 ? "Vide" : "Pleine";
    etatCasier2 = boxEmpty2 ? "Vide" : "Pleine";

    if (boxEmpty1 != prevBoxEmpty1) {
        myapi->updateCaseState(uid);
        prevBoxEmpty1 = boxEmpty1;
    }
    if (boxEmpty2 != prevBoxEmpty2) {
        myapi->updateCaseState(uid);
        prevBoxEmpty2 = boxEmpty2;
    }

    if (myapi->openCase(uid)) {
        String caseNumbers = myapi->getCaseNumbers();
        Serial.println("Numéros de casier retournés : " + caseNumbers);

        int caseCount = 1;
        for (size_t i = 0; i < caseNumbers.length(); i++) {
            if (caseNumbers[i] == ',') caseCount++;
        }

        if (isFacteur && caseCount > 1) {
            if (boxEmpty1) {
                Serial.println("Facteur : Ouvre CAS-001 car boxEmpty1 est vrai");
                if (mySolenoide1 && boxEmpty1) {
                    mySolenoide1->openCase();
                    solenoidOpenTime = millis();
                }
            } else {
                Serial.println("Facteur : Ouvre CAS-002 car boxEmpty1 est faux");
                if (mySolenoide2 && boxEmpty2) {
                    mySolenoide2->openCase();
                    solenoidOpenTime = millis();
                }
            }
        } else {
            String caseList = caseNumbers + ",";
            int start = 0;
            for (int i = 0; i < caseCount; i++) {
                int comma = caseList.indexOf(',', start);
                String caseNumber = caseList.substring(start, comma);
                start = comma + 1;

                if (caseNumber == "CAS-001") {
                    if (!boxEmpty1) {
                        Serial.println("Client : Ouvre CAS-001");
                        if (mySolenoide1) {
                            mySolenoide1->openCase();
                            solenoidOpenTime = millis();
                           // myapi->updateDeliveryState(uid);
                        }
                        break;
                    }
                } else if (caseNumber == "CAS-002") {
                    if (!boxEmpty2) {
                        Serial.println("Client : Ouvre CAS-002");
                        if (mySolenoide2) {
                            mySolenoide2->openCase();
                            solenoidOpenTime = millis();
                           // myapi->updateDeliveryState(uid);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void setup() {
    Serial.begin(9600);

    mywifi = new MyWifi();
    if (!mywifi->connect()) {
        Serial.println("Erreur de connexion WiFi");
        etatWifi = "Erreur WiFi";
    } else {
        etatWifi = "WiFi OK";
        Serial.println("Connexion WiFi établie!");
        Serial.print("Adresse IP: ");
        Serial.println(mywifi->getLocalIP());
    }

    myapi = new MyAPI();
    mymqttmanager = new MyMQTTManager();
    if (!mymqttmanager->init()) {
        Serial.println("Erreur d'initialisation MQTT");
    } else {
        mymqttmanager->connect();
    }

    myUltrasonique1 = new MyUltrasonique(32, 33); // Casier 1 : trig_Pin=32, echo_Pin=33
    myUltrasonique2 = new MyUltrasonique(2, 15); // Casier 2 : trig_Pin=25, echo_Pin=26
    if (!myUltrasonique1->FindEmptyBoxDistance()) {
        Serial.println("Erreur de calibration du capteur ultrasonique 1");
    } else {
        Serial.println("Calibration du capteur ultrasonique 1 terminée avec succès");
    }
    if (!myUltrasonique2->FindEmptyBoxDistance()) {
        Serial.println("Erreur de calibration du capteur ultrasonique 2 (peut-être non branché)");
    } else {
        Serial.println("Calibration du capteur ultrasonique 2 terminée avec succès");
    }

    myRFID = new MyRFID();
    if (!myRFID->IsRFIDDetected()) {
        Serial.println("Erreur de détection du module RFID");
    }

    myButtonLEFT = new MyButton(BTN_LEFT);
    myButtonRIGHT = new MyButton(BTN_RIGHT);

    mytemp = new MyTemp();
    if (!mytemp->init()) {
        Serial.println("Erreur d'initialisation de MyTemp");
    }
    mytemp->setUniteUsed(MyTemp::UNITY_CELSIUS);

    myOled = new MyOled(&Wire, OLED_RESET, SCREEN_HEIGHT, SCREEN_WIDTH);
    if (myOled->init(2) != 0) {
        Serial.println("Erreur d'initialisation de MyOled");
        while (1);
    }

    mySolenoide1 = new MySolenoide(RELAY_PIN_SOLENOIDE1);
    mySolenoide2 = new MySolenoide(RELAY_PIN_SOLENOIDE2);
    if (!mySolenoide1->init() || !mySolenoide2->init()) {
        Serial.println("Erreur d'initialisation des solénoïdes");
        while (1);
    }
    mySolenoide1->closeCase();
    mySolenoide2->closeCase();

    // setupUltrasonicTests();
}

void loop() {
    mywifi->checkResetButton();

    if (millis() - lastComponentCheck >= CHECK_INTERVAL) {
        checkComponents();
        lastComponentCheck = millis();
    }

    String newCardRead = myRFID ? myRFID->ReadCardSerial() : "";
    if (newCardRead != "" && newCardRead != currentUid) {
        currentUid = newCardRead;
        Serial.println("UID détecté: " + currentUid);
        handleRFIDLogic(currentUid);
        myRFID->Reset();
    }

    buttonleftState = myButtonLEFT ? myButtonLEFT->ButtonPressed() : 0;
    buttonrightState = myButtonRIGHT ? myButtonRIGHT->ButtonPressed() : 0;
    temperature = mytemp ? mytemp->getTemperature() : NAN;
    Serial.println("Température: " + String(temperature) + " °C");

    if (buttonleftState == 1) {
        if (myOled) myOled->moveLeftButton();
        delay(100);
    }

    if (buttonrightState == 1) {
        if (myOled) myOled->moveRightButton(temperature, etatCasier1, etatCasier2, etatWifi);
        delay(100);
    }

    if (myOled) {
        if (myOled->getCurrentSubMenu() == -1) {
            myOled->displayMainMenu();
        } else {
            myOled->displaySubMenu(myOled->getCurrentSubMenu(), temperature, etatCasier1, etatCasier2, etatWifi);
        }
    }

    etatCasier1 = myUltrasonique1->IsBoxEmpty() ? "Vide" : "Pleine";
    etatCasier2 = myUltrasonique2->IsBoxEmpty() ? "Vide" : "Pleine";

    // runUltrasonicTests();

    if (solenoidOpenTime > 0 && (millis() - solenoidOpenTime >= SOLENOID_TIMEOUT)) {
        Serial.println("Délai de 5 secondes écoulé : fermeture des solénoïdes");
        if (mySolenoide1) mySolenoide1->closeCase();
        if (mySolenoide2) mySolenoide2->closeCase();
        solenoidOpenTime = 0;
        if (myRFID) myRFID->Reset();
    }

    if (mywifi->isConnected() && !mymqttmanager->client.connected() && millis() - lastReconnectAttempt >= RECONNECT_INTERVAL) {
        mymqttmanager->tryConnect();
        lastReconnectAttempt = millis();
    }

    if (mymqttmanager) mymqttmanager->clientLoop();
    delay(500);
}