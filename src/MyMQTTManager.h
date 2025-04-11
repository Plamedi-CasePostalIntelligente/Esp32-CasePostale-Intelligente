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

    // Méthodes pour publier des messages sur les topics
    void publishtopic1(const char *message);
    void publishtopic2(const char *message);
    void publishTempStatus(const char *message);
    void publishOledStatus(const char *message);
    void publishRfidStatus(const char *message);
    void publishUltrasonicStatus(const char *message);
    void publishTempError(float number);
    void publishOledError(float number);
    void publishRfidError(float number);
    void publishUltrasonicError(float number);

    // Méthodes pour subscrire aux topics
    void subscribetopic1();
    void subscribetopic2();
    void subscribetempStatusTopic();
    void subscribeoledStatusTopic();
    void subscriberfidStatusTopic();
    void subscribeultrasonicStatusTopic();
    void subscribetempErrorTopic();
    void subscribeoledErrorTopic();
    void subscriberfidErrorTopic();
    void subscribeultrasonicErrorTopic();


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
    const char *tempStatusTopic = "casier1/temperature/status";
    const char *oledStatusTopic = "casier1/oled/status";
    const char *rfidStatusTopic = "casier1/rfid/status";
    const char *ultrasonicStatusTopic = "casier1/ultrasonic/status";

    const char *tempErrorTopic = "casier1/temperature/error";
    const char *oledErrorTopic = "casier1/oled/error";
    const char *rfidErrorTopic = "casier1/rfid/error";
    const char *ultrasonicErrorTopic = "casier1/ultrasonic/error";


    WiFiClient espClient;
    PubSubClient client;
    MyAPI api;
    Preferences preferences;
    String messageFirstTopic;
    String messageSecondTopic;
    String messagetempStatusTopic;
    String messageoledStatusTopic;
    String messagerfidStatusTopic;
    String messageultrasonicStatusTopic;
    String messagetempErrorTopic;
    String messageoledErrorTopic;
    String messagerfidErrorTopic;
    String messageultrasonicErrorTopic;
};

#endif