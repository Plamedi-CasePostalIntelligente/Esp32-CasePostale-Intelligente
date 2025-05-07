#include "MyWifi.h"

MyWifi::MyWifi() : buttonPressed(false), buttonPressTime(0), lastButtonState(LOW)
{
    pinMode(RESET_BUTTON, INPUT_PULLDOWN); // GPIO 4 avec pull-up interne
}

bool MyWifi::connect()
{
    WiFiManager wm;
    wm.setConfigPortalTimeout(TIMEOUT);
    wm.setAPCallback([](WiFiManager *myWiFiManager) {
        Serial.println("Point d'accès démarré : " + String(myWiFiManager->getConfigPortalSSID()));
    });

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

    // Forcer la fermeture du point d'accès
    wm.stopConfigPortal();
    WiFi.softAPdisconnect(true);

    return true;
}

void MyWifi::checkResetButton()
{
    // Lecture de l'état actuel du bouton (LOW quand pressé avec INPUT_PULLUP)
    bool currentButtonState = digitalRead(RESET_BUTTON);

    // Détection d'un appui sur le bouton (transition de HIGH à LOW)
    if (currentButtonState == LOW && lastButtonState == HIGH)
    {
        buttonPressed = true;
        buttonPressTime = millis();
        Serial.println("Bouton pressé - début du compteur");
    }
    // Si le bouton est maintenu appuyé
    else if (currentButtonState == LOW && buttonPressed)
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
    else if (currentButtonState == HIGH)
    {
        buttonPressed = false;
    }

    // Sauvegarde de l'état du bouton pour le prochain cycle
    lastButtonState = currentButtonState;
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