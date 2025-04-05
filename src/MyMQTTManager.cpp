/*
 * Fichier: MyMQTTManager.cpp
 * Créé le: 2024-03-01
 * Mis à jour le: 2024-05-01
 * Auteurs: Plamedi Ilunga
 * Contact: 2038993@cegeprdl.ca
 * Version: 2.0
 * Description: Ce fichier definit la classe MyMQTTManager
 * Licence: Arduino
 */
#include "MyMQTTManager.h"

MyMQTTManager* MyMQTTManager::instance = nullptr;

MyMQTTManager::MyMQTTManager() : client(espClient) {
    instance = this; // On stocke l'instance pour le callback statique
}

bool MyMQTTManager::init()
{
    if (api.getBrokerInfo())
    {
        server = api.getMqttAddress();
        port = api.getMqttPort();
        user = api.getMqttUser();
        password = api.getMqttPassword();

        client.setServer(server.c_str(), port.toInt());
        return true;
    }
    return false;
}

void MyMQTTManager::connect()
{
    setCallback();
    while (!client.connected())
    {
        Serial.println("Connexion au broker MQTT...");
        if (client.connect("ESP32Client", user.c_str(), password.c_str()))
        {
            Serial.println("Connecté au broker MQTT");
            subscribetopic1();
            subscribetopic2();
        }
        else
        {
            Serial.print("Échec, rc=");
            Serial.print(client.state());
            Serial.println(" nouvel essai dans 5 secondes");
            delay(5000);
        }
    }
}

void MyMQTTManager::publishtopic1(const char *message)
{
    client.publish(topic1, message);
}

void MyMQTTManager::publishtopic2(const char *message)
{
    client.publish(topic2, message);
}

void MyMQTTManager::subscribetopic1()
{
    client.subscribe(topic1);
}

void MyMQTTManager::subscribetopic2()
{
    client.subscribe(topic2);
}


void MyMQTTManager::setCallback()
{
    client.setCallback(mqttCallbackHandler);
}


void MyMQTTManager::mqttCallbackHandler(char *topic, byte *payload, unsigned int length)
{
    char message[length + 1];
    for (int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';

    if (strcmp(topic, "test") == 0)
    {
        instance->messageFirstTopic = message;
       // Serial.println("Message reçu sur test: " + String(message));
        Serial.println("Message reçu sur test: " + String(instance->messageFirstTopic));
    }
    else if (strcmp(topic, "Skibidy/Desiree") == 0)
    {
        instance->messageSecondTopic = message;
        Serial.println("Message reçu sur test2: " + String(message));
        Serial.println("Message reçu sur test2: " + String(instance->messageSecondTopic));  
    }
}

String MyMQTTManager::floatToString(float value, int precision)
{
    char temp[10];
    dtostrf(value, 4, precision, temp);
    return String(temp);
}

void MyMQTTManager::clientLoop()
{
    client.loop();
}