/*
 * Fichier: MyWifi.cpp
 * Créé le: 2024-03-01
 * Mis à jour le: 2025-02-20
 * Auteurs: Plamedi Ilunga 
 * Contact: 2038993@cegeprdl.ca 
 * Version: 1.0
 * Description: Ce fichier implemente la classe MyWifi
 * Licence: Arduino
 */
#include "MyWifi.h"

MyWifi::MyWifi() : buttonPressed(false), buttonPressTime(0), lastButtonState(LOW)
{
    pinMode(RESET_BUTTON, INPUT_PULLDOWN); // Configuration en INPUT_PULLDOWN comme dans votre exemple
}

bool MyWifi::connect()
{
    WiFiManager wm;
    wm.setConfigPortalTimeout(TIMEOUT);

    Serial.println("Démarrage de la configuration WiFi...");

    bool res = wm.autoConnect(AP_SSID, AP_PASSWORD);

    if (!res)
    {
        Serial.println("Échec de la connexion WiFi");
        return false;
    }

    Serial.println("Connexion WiFi établie!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());

    return true;
}

void MyWifi::checkResetButton()
{
    // Lecture de l'état actuel du bouton
    bool currentButtonState = digitalRead(RESET_BUTTON);

    // Détection d'un appui sur le bouton (transition de LOW à HIGH)
    if (currentButtonState == HIGH && lastButtonState == LOW)
    {
        buttonPressed = true;
        buttonPressTime = millis();
        Serial.println("Bouton pressé - début du compteur");
    }
    // Si le bouton est maintenu appuyé
    else if (currentButtonState == HIGH && buttonPressed)
    {
        if (millis() - buttonPressTime >= RESET_TIME)
        {
            Serial.println("Reset WiFi demandé...");
            WiFiManager wm;
            wm.resetSettings();
            Serial.println("Paramètres WiFi effacés");
            ESP.restart();
        }
    }
    // Si le bouton est relâché
    else if (currentButtonState == LOW)
    {
        buttonPressed = false;
    }

    // Sauvegarde de l'état du bouton pour le prochain cycle
    lastButtonState = currentButtonState;
    delay(50); // Anti-rebond comme dans votre exemple
}

String MyWifi::getLocalIP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }
    return "Non connecté";
}

bool MyWifi::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}