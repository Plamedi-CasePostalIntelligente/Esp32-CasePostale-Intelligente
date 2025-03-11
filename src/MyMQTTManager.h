/*
 * Fichier: MyMQTTManager.h
 * Créé le: 2024-03-01
 * Mis à jour le: 2024-05-01
 * Auteurs: Plamedi Ilunga
 * Contact: 2038993@cegeprdl.ca 
 * Version: 2.0
 * Description: Ce fichier definit la classe MyMQTTManager
 * Licence: Arduino
 */
#ifndef MYMQTTMANAGER_H
#define MYMQTTMANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "MyAPI.h"
#include <Preferences.h>

class MyMQTTManager
{
public:
    MyMQTTManager();
    bool init(); // Nouvelle méthode pour initialiser avec les infos de l'API
    void connect();
    void publishtopic1(const char *message);
    void publishtopic2(const char *message);
    void subscribetopic1();
    void subscribetopic2();
    void clientLoop();
    void setCallback();
    static void mqttCallbackHandler(char *topic, byte *payload, unsigned int length);
    static MyMQTTManager* instance;
    String floatToString(float value, int precision = 1);

private:
    // Ces variables stockeront les informations de l'API
    String server;
    String port;
    String user;
    String password;

    const char *topic1 = "test";
    const char *topic2 = "Skibidy/Desiree";
    WiFiClient espClient;
    PubSubClient client;
    MyAPI api;
    Preferences preferences;
    String messageFirstTopic;
    String messageSecondTopic;
};

#endif