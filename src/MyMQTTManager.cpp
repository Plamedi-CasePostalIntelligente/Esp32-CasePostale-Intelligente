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

void MyMQTTManager::publishTempStatus(const char *message)
{
    client.publish(tempStatusTopic, message);
}

void MyMQTTManager::publishOledStatus(const char *message)
{
    client.publish(oledStatusTopic, message);
}

void MyMQTTManager::publishRfidStatus(const char *message)
{
    client.publish(rfidStatusTopic, message);
}

void MyMQTTManager::publishUltrasonicStatus(const char *message)
{
    client.publish(ultrasonicStatusTopic, message);
}

void MyMQTTManager::publishTempError(float number)
{
    String message = floatToString(number, 1);
    client.publish(tempErrorTopic, message.c_str());
}

void MyMQTTManager::publishOledError(float number)
{
    String message = floatToString(number, 1);
    client.publish(oledErrorTopic, message.c_str());
}

void MyMQTTManager::publishRfidError(float number)
{
    String message = floatToString(number, 1);
    client.publish(rfidErrorTopic, message.c_str());
}

void MyMQTTManager::publishUltrasonicError(float number)
{
    String message = floatToString(number, 1);
    client.publish(ultrasonicErrorTopic, message.c_str());
}

void MyMQTTManager::subscribetempStatusTopic()
{
    client.subscribe(tempStatusTopic);
}

void MyMQTTManager::subscribeoledStatusTopic()
{
    client.subscribe(oledStatusTopic);
}

void MyMQTTManager::subscribetempErrorTopic()
{
    client.subscribe(tempErrorTopic);
}

void MyMQTTManager::subscriberfidStatusTopic()
{
    client.subscribe(rfidStatusTopic);
}

void MyMQTTManager::subscribeultrasonicStatusTopic()
{
    client.subscribe(ultrasonicStatusTopic);
}

void MyMQTTManager::subscribeoledErrorTopic()
{
    client.subscribe(oledErrorTopic);
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
        Serial.println("Message reçu sur test: " + String(message));
        Serial.println("Message reçu sur test: " + String(instance->messageFirstTopic));
    }
    else if (strcmp(topic, "Skibidy/Desiree") == 0)
    {
        instance->messageSecondTopic = message;
       // Serial.println("Message reçu sur test2: " + String(message));
       // Serial.println("Message reçu sur test2: " + String(instance->messageSecondTopic));  
    }else if (strcmp(topic, "casier1/temperature/status") == 0)
    {
        instance->messagetempStatusTopic = message;
        // Serial.println("Message reçu sur casier1/temperature/status: " + String(message));
    }
    else if (strcmp(topic, "casier1/oled/status") == 0)
    {
        instance->messageoledStatusTopic = message;
        // Serial.println("Message reçu sur casier1/oled/status: " + String(message));
    }
    else if (strcmp(topic, "casier1/rfid/status") == 0)
    {
        instance->messagerfidStatusTopic = message;
        // Serial.println("Message reçu sur casier1/rfid/status: " + String(message));
    }
    else if (strcmp(topic, "casier1/ultrasonic/status") == 0)
    {
        instance->messageultrasonicStatusTopic = message;
        // Serial.println("Message reçu sur casier1/ultrasonic/status: " + String(message));
    }
    else if (strcmp(topic, "casier1/temperature/error") == 0)
    {
        instance->messagetempErrorTopic = message;
        // Serial.println("Message reçu sur casier1/temperature/error: " + String(message));
    }
    else if (strcmp(topic, "casier1/oled/error") == 0)
    {
        instance->messageoledErrorTopic = message;
        // Serial.println("Message reçu sur casier1/oled/error: " + String(message));
    }
    else if (strcmp(topic, "casier1/rfid/error") == 0)
    {
        instance->messagerfidErrorTopic = message;
        // Serial.println("Message reçu sur casier1/rfid/error: " + String(message));
    }
    else if (strcmp(topic, "casier1/ultrasonic/error") == 0)
    {
        instance->messageultrasonicErrorTopic = message;
        // Serial.println("Message reçu sur casier1/ultrasonic/error: " + String(message));
    }
    else
    {
        Serial.println("Message reçu sur un topic inconnu: " + String(topic));
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