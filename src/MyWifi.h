/*
 * Fichier: MyWifi.h
 * Créé le: 2024-03-01
 * Mis à jour le: 2025-02-20
 * Auteurs: Plamedi Ilunga 
 * Contact: 2038993@cegeprdl.ca 
 * Version: 1.0
 * Description: Ce fichier definit la classe MyWifi
 * Licence: Arduino
 */
#ifndef MYWIFI_H
#define MYWIFI_H

#include <WiFiManager.h>
#include <WiFi.h>

class MyWifi
{
public:
    MyWifi();
    bool connect();
    String getLocalIP();
    bool isConnected();
    void checkResetButton();

private:
    const char *AP_SSID = "ESP32-Config";
    const char *AP_PASSWORD = "12345678";
    const int TIMEOUT = 180;
    const int RESET_BUTTON = 32;  // GPIO27 pour le bouton de reset
    const long RESET_TIME = 5000; // 5 secondes pour le reset
    bool buttonPressed;
    bool lastButtonState; // Pour détecter les transitions
    unsigned long buttonPressTime;
};

#endif