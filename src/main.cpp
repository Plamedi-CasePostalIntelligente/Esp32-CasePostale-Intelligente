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
int RELAY_PIN_SOLENOIDE1 = 26; // GPIO26
int RELAY_PIN_SOLENOIDE2 = 27; // GPIO27

// === AJOUTS POUR LES TESTS ULTRASONIQUES ===
// Topics MQTT
const char *ULTRASONIC_LATENCY_TOPIC = "casier1/ultrasonic/test/latency";
const char *ULTRASONIC_REPEATABILITY_TOPIC = "casier1/ultrasonic/test/repeatability";

// Variables pour les tests
unsigned long lastTestTime = 0;
const unsigned long TEST_INTERVAL = 10000; // Test toutes les 10 secondes
float previousDistance = 0;
unsigned long changeStartTime = 0;
bool measuringLatency = false;

// Buffer circulaire pour la répétabilité
const int BUFFER_SIZE = 10;
float distanceBuffer[BUFFER_SIZE];
int bufferIndex = 0;
bool bufferFull = false;

MyWifi *mywifi;
MyAPI *myapi;
MyMQTTManager *mymqttmanager;
MyUltrasonique *myUltrasonique;
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
const char *etatCasier1;
const char *etatCasier2;
const char *etatWifi;
String nombreRfid = "";
unsigned long solenoidOpenTime = 0;          // Temps d'ouverture des solénoïdes
const unsigned long SOLENOID_TIMEOUT = 5000; // 5 secondes avant fermeture
unsigned long lastComponentCheck = 0;        // Dernière vérification des composants
const unsigned long CHECK_INTERVAL = 20000;  // Vérifier toutes les 10 secondes

// Fonction pour vérifier périodiquement l'état des composants
void checkComponents() {
    // Vérifier MyTemp
    if (mytemp && !mytemp->init()) {
        mymqttmanager->publishTempStatus("Erreur: Capteur de température non détecté");
    } else if (mytemp && mytemp->init()) {
        mymqttmanager->publishTempStatus("Capteur de température opérationnel");
    }

    // Vérifier MyOled
    if (myOled && !myOled->isOperational()) {
        mymqttmanager->publishOledStatus("Erreur: Écran OLED non détecté");
    } else if (myOled && myOled->isOperational()) {
        mymqttmanager->publishOledStatus("Écran OLED opérationnel");
    }

    // Vérifier MyRFID
    if (myRFID && !myRFID->IsRFIDDetected()) {
        mymqttmanager->publishRfidStatus("Erreur: Module RFID non détecté");
    } else if (myRFID && myRFID->IsRFIDDetected()) {
        mymqttmanager->publishRfidStatus("Module RFID opérationnel");
    }

    // Vérifier MyUltrasonique
    if (myUltrasonique && !myUltrasonique->FindEmptyBoxDistance()) {
        mymqttmanager->publishUltrasonicStatus("Erreur: Capteur ultrasonique non détecté");
    } else if (myUltrasonique && myUltrasonique->FindEmptyBoxDistance()) {
        mymqttmanager->publishUltrasonicStatus("Capteur ultrasonique opérationnel");
    }
}

// Fonction pour configurer les tests ultrasoniques
void setupUltrasonicTests() {
    if (myUltrasonique) {
        previousDistance = myUltrasonique->GetDistance();
    }
}

// Fonction pour exécuter les tests ultrasoniques
void runUltrasonicTests() {
    if (!myUltrasonique || !mymqttmanager || millis() - lastTestTime < TEST_INTERVAL) {
        return;
    }

    lastTestTime = millis();
    float currentDistance = myUltrasonique->GetDistance();

    // Stocker la mesure dans le buffer
    distanceBuffer[bufferIndex] = currentDistance;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
    if (bufferIndex == 0)
        bufferFull = true;

    // Test de répétabilité
    if (bufferFull) {
        float sum = 0, sumDiff = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            sum += distanceBuffer[i];
        }
        float avg = sum / BUFFER_SIZE;

        for (int i = 0; i < BUFFER_SIZE; i++) {
            sumDiff += pow(distanceBuffer[i] - avg, 2);
        }
        float stdDev = sqrt(sumDiff / BUFFER_SIZE);

        String message = "{\"avg\":" + String(avg, 2) +
                         ",\"std_dev\":" + String(stdDev, 2) +
                         ",\"cv\":" + String((stdDev / avg) * 100, 2) + "}";
        mymqttmanager->client.publish(ULTRASONIC_REPEATABILITY_TOPIC, message.c_str());
        Serial.println("Test répétabilité: " + message);
    }

    // Test de latence
    if (!measuringLatency && abs(currentDistance - previousDistance) > 5.0) {
        // Changement significatif détecté, démarrer mesure de latence
        measuringLatency = true;
        changeStartTime = millis();
        Serial.println("Changement détecté, début mesure latence");
    } else if (measuringLatency && abs(currentDistance - myUltrasonique->GetDistance()) < 0.5) {
        // Mesure stabilisée
        unsigned long latency = millis() - changeStartTime;
        String message = "{\"latency_ms\":" + String(latency) +
                         ",\"distance_change\":" + String(abs(currentDistance - previousDistance), 2) + "}";
        mymqttmanager->client.publish(ULTRASONIC_LATENCY_TOPIC, message.c_str());
        Serial.println("Test latence: " + message);
        measuringLatency = false;
    }

    previousDistance = currentDistance;
}

void setup() {
    Serial.begin(9600);

    // Instanciation des objets
    mywifi = new MyWifi();
    if (!mywifi) {
        Serial.println("Erreur d'instanciation de la classe MyWifi");
    }

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
    if (!myapi) {
        Serial.println("Erreur d'instanciation de la classe MyAPI");
    }

    if (!myapi->getBrokerInfo()) {
        Serial.println("Erreur de récupération des informations du broker");
    }

    mymqttmanager = new MyMQTTManager();
    if (!mymqttmanager) {
        Serial.println("Erreur d'instanciation de la classe MyMQTTManager");
    }

    mymqttmanager->init();
    if (!mymqttmanager->init()) {
        Serial.println("Erreur d'initialisation du client MQTT");
    } else {
        mymqttmanager->connect();
    }

    myUltrasonique = new MyUltrasonique();
    if (!myUltrasonique) {
        Serial.println("Erreur d'instanciation de la classe MyUltrasonique");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishUltrasonicStatus("Erreur d'instanciation de MyUltrasonique");
        }
    }

    myRFID = new MyRFID();
    if (!myRFID) {
        Serial.println("Erreur d'instanciation de la classe MyRFID");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishRfidStatus("Erreur d'instanciation de MyRFID");
        }
    }

    myButtonLEFT = new MyButton(BTN_LEFT);
    myButtonRIGHT = new MyButton(BTN_RIGHT);
    if (!myButtonLEFT || !myButtonRIGHT) {
        Serial.println("Erreur d'instanciation de la classe MyButton");
    }

    mytemp = new MyTemp();
    if (!mytemp) {
        Serial.println("Erreur d'instanciation de la classe MyTemp");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishTempStatus("Erreur d'instanciation de MyTemp");
        }
    }

    // Initialisation des composants
    if (mytemp && !mytemp->init()) {
        Serial.println("Erreur d'initialisation de la classe MyTemp");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishTempStatus("Erreur d'initialisation de MyTemp");
        }
    }
    if (mytemp) {
        mytemp->setUniteUsed(MyTemp::UNITY_CELSIUS);
    }

    myOled = new MyOled(&Wire, OLED_RESET, SCREEN_HEIGHT, SCREEN_WIDTH);
    if (!myOled) {
        Serial.println("Erreur d'instanciation de la classe MyOled");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishOledStatus("Erreur d'instanciation de MyOled");
        }
    }

    mySolenoide1 = new MySolenoide(RELAY_PIN_SOLENOIDE1);
    mySolenoide2 = new MySolenoide(RELAY_PIN_SOLENOIDE2);
    if (!mySolenoide1 || !mySolenoide2) {
        Serial.println("Erreur d'instanciation de la classe MySolenoide");
        while (1);
    }

    if (myOled && myOled->init(2) != 0) {
        Serial.println("Erreur d'initialisation de la classe MyOled");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishOledStatus("Erreur d'initialisation de MyOled");
        }
        while (1);
    }

    if (myUltrasonique && !myUltrasonique->FindEmptyBoxDistance()) {
        Serial.println("Erreur de calibration de la boîte vide");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishUltrasonicStatus("Erreur de calibration de la boîte vide");
        }
        while (1);
    }

    if (myRFID && !myRFID->IsRFIDDetected()) {
        Serial.println("Erreur de détection du module RFID");
        if (mymqttmanager && mymqttmanager->init()) {
            mymqttmanager->publishRfidStatus("Erreur de détection du module RFID");
        }
    }

    if (mySolenoide1 && !mySolenoide1->init()) {
        Serial.println("Erreur d'initialisation du solénoïde 1");
    }

    if (mySolenoide2 && !mySolenoide2->init()) {
        Serial.println("Erreur d'initialisation du solénoïde 2");
    }

    // Fermer les solénoïdes au démarrage
    if (mySolenoide1)
        mySolenoide1->closeCase();
    if (mySolenoide2)
        mySolenoide2->closeCase();

    // Initialiser les tests ultrasoniques
    setupUltrasonicTests();
}

void loop() {
    mywifi->checkResetButton();
    // Vérification périodique des composants
    if (millis() - lastComponentCheck >= CHECK_INTERVAL) {
        checkComponents();
        lastComponentCheck = millis(); // Mettre à jour le temps de la dernière vérification
    }

    // Lecture de la carte RFID
    String newCardRead = myRFID ? myRFID->ReadCardSerial() : "";
    if (newCardRead != "") {
        nombreRfid = newCardRead;
        Serial.println("UID détecté: " + nombreRfid);
    }

    buttonleftState = myButtonLEFT ? myButtonLEFT->ButtonPressed() : 0;
    buttonrightState = myButtonRIGHT ? myButtonRIGHT->ButtonPressed() : 0;
    temperature = mytemp ? mytemp->getTemperature() : NAN;

    if (buttonleftState == 1) {
        Serial.println("Bouton gauche pressé");
        if (myOled)
            myOled->moveLeftButton();
        delay(100);
    }

    if (buttonrightState == 1) {
        Serial.println("Bouton droit pressé");
        if (myOled)
            myOled->moveRightButton(temperature, etatCasier1, etatCasier2, etatWifi);
        delay(100);
    }

    if (myOled) {
        if (myOled->getCurrentSubMenu() == -1) {
            myOled->displayMainMenu();
        } else {
            myOled->displaySubMenu(myOled->getCurrentSubMenu(), temperature, etatCasier1, etatCasier2, etatWifi);
        }
    }

    // Gestion Ultrasonique (pour affichage uniquement)
    bool boxEmpty = myUltrasonique ? myUltrasonique->IsBoxEmpty() : false;
    if (boxEmpty) {
        etatCasier1 = "Vide";
        etatCasier2 = "Vide";
    } else {
        etatCasier1 = "Pleine";
        etatCasier2 = "Pleine";
    }

    // Gestion RFID et Solénoïdes
    if (nombreRfid == "5A79FC03" && boxEmpty && solenoidOpenTime == 0) {
        Serial.println("Puce RFID valide détectée et boîte vide : ouverture des solénoïdes");
        if (mySolenoide1)
            mySolenoide1->openCase();
        if (mySolenoide2)
            mySolenoide2->openCase();
        solenoidOpenTime = millis(); // Enregistrer le moment de l'ouverture
        nombreRfid = "";             // Réinitialise immédiatement
        if (myRFID)
            myRFID->Reset(); // Réinitialise le module RFID
    }

    // Fermeture automatique après 5 secondes
    if (solenoidOpenTime > 0 && (millis() - solenoidOpenTime >= SOLENOID_TIMEOUT)) {
        Serial.println("Délai de 5 secondes écoulé : fermeture des solénoïdes");
        if (mySolenoide1)
            mySolenoide1->closeCase();
        if (mySolenoide2)
            mySolenoide2->closeCase();
        solenoidOpenTime = 0; // Réinitialiser
        if (myRFID)
            myRFID->Reset(); // Réinitialise à nouveau
    }

    // Exécuter les tests ultrasoniques
    runUltrasonicTests();

    if (mymqttmanager)
        mymqttmanager->clientLoop();
    delay(500); // Réactivité accrue
}