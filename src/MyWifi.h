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
    const int RESET_BUTTON = 4; // GPIO 4 pour le bouton de reset
    const long RESET_TIME = 5000; // 5 secondes pour le reset
    bool buttonPressed;
    bool lastButtonState;
    unsigned long buttonPressTime;
};

#endif