#include "MyMQTTManager.h"

MyMQTTManager* MyMQTTManager::instance = nullptr;

MyMQTTManager::MyMQTTManager() : client(espClient) {
    instance = this; // On stocke l'instance pour le callback statique
}

bool MyMQTTManager::init()
{
    // Tenter de charger les informations du broker depuis la mémoire
    if (loadBrokerInfo()) {
        client.setServer(server.c_str(), port.toInt());
        Serial.println("Broker MQTT chargé depuis la mémoire: " + server + ":" + port);
        return true;
    }

    // Si aucune information valide n'est trouvée, appeler getBrokerInfo
    if (api.getBrokerInfo())
    {
        server = api.getMqttAddress();
        port = api.getMqttPort();
        user = api.getMqttUser();
        password = api.getMqttPassword();

        if (server.isEmpty() || port.isEmpty()) {
            Serial.println("Erreur: Informations du broker MQTT invalides");
            return false;
        }

        client.setServer(server.c_str(), port.toInt());
        saveBrokerInfo(); // Sauvegarder les nouvelles informations
        Serial.println("Broker MQTT configuré: " + server + ":" + port);
        return true;
    }

    Serial.println("Échec de la récupération des informations du broker");
    return false;
}

bool MyMQTTManager::tryConnect()
{
    if (!myWifi.isConnected()) {
        Serial.println("WiFi non connecté. Impossible de se connecter au broker MQTT.");
        return false;
    }

    if (client.connected()) {
        return true;
    }

    Serial.println("Connexion au broker MQTT...");
    if (client.connect("ESP32Client", user.c_str(), password.c_str()))
    {
        Serial.println("Connecté au broker MQTT");
        subscribetopic1();
        subscribetopic2();
        subscribetempStatusTopic();
        subscribeoledStatusTopic();
        subscriberfidStatusTopic();
        subscribeultrasonicStatusTopic();
        subscribetempErrorTopic();
        subscribeoledErrorTopic();
        return true;
    }
    else
    {
        Serial.print("Échec, rc=");
        Serial.print(client.state());
        Serial.println(" Nouvel essai ultérieur");
        return false;
    }
}

void MyMQTTManager::connect()
{
    setCallback();
    tryConnect();
}

void MyMQTTManager::reset()
{
    client.disconnect();
    server = "";
    port = "";
    user = "";
    password = "";
    preferences.begin("mqtt", false);
    preferences.clear();
    preferences.end();
    Serial.println("Connexion MQTT et préférences réinitialisées.");
}

void MyMQTTManager::saveBrokerInfo()
{
    preferences.begin("mqtt", false);
    preferences.putString("server", server);
    preferences.putString("port", port);
    preferences.putString("user", user);
    preferences.putString("password", password);
    preferences.end();
    Serial.println("Informations du broker sauvegardées dans la mémoire non volatile.");
}

bool MyMQTTManager::loadBrokerInfo()
{
    preferences.begin("mqtt", true);
    server = preferences.getString("server", "");
    port = preferences.getString("port", "");
    user = preferences.getString("user", "");
    password = preferences.getString("password", "");
    preferences.end();
    bool valid = !server.isEmpty() && !port.isEmpty();
    if (valid) {
        Serial.println("Informations du broker chargées depuis la mémoire non volatile.");
    }
    return valid;
}

void MyMQTTManager::publishtopic1(const char *message)
{
    if (client.connected()) {
        client.publish(topic1, message);
    }
}

void MyMQTTManager::publishtopic2(const char *message)
{
    if (client.connected()) {
        client.publish(topic2, message);
    }
}

void MyMQTTManager::publishTempStatus(const char *message)
{
    if (client.connected()) {
        client.publish(tempStatusTopic, message);
    }
}

void MyMQTTManager::publishOledStatus(const char *message)
{
    if (client.connected()) {
        client.publish(oledStatusTopic, message);
    }
}

void MyMQTTManager::publishRfidStatus(const char *message)
{
    if (client.connected()) {
        client.publish(rfidStatusTopic, message);
    }
}

void MyMQTTManager::publishUltrasonicStatus(const char *message)
{
    if (client.connected()) {
        client.publish(ultrasonicStatusTopic, message);
    }
}

void MyMQTTManager::publishTempError(float number)
{
    if (client.connected()) {
        String message = floatToString(number, 1);
        client.publish(tempErrorTopic, message.c_str());
    }
}

void MyMQTTManager::publishOledError(float number)
{
    if (client.connected()) {
        String message = floatToString(number, 1);
        client.publish(oledErrorTopic, message.c_str());
    }
}

void MyMQTTManager::publishRfidError(float number)
{
    if (client.connected()) {
        String message = floatToString(number, 1);
        client.publish(rfidErrorTopic, message.c_str());
    }
}

void MyMQTTManager::publishUltrasonicError(float number)
{
    if (client.connected()) {
        String message = floatToString(number, 1);
        client.publish(ultrasonicErrorTopic, message.c_str());
    }
}

void MyMQTTManager::subscribetempStatusTopic()
{
    if (client.connected()) {
        client.subscribe(tempStatusTopic);
    }
}

void MyMQTTManager::subscribeoledStatusTopic()
{
    if (client.connected()) {
        client.subscribe(oledStatusTopic);
    }
}

void MyMQTTManager::subscribetempErrorTopic()
{
    if (client.connected()) {
        client.subscribe(tempErrorTopic);
    }
}

void MyMQTTManager::subscriberfidStatusTopic()
{
    if (client.connected()) {
        client.subscribe(rfidStatusTopic);
    }
}

void MyMQTTManager::subscribeultrasonicStatusTopic()
{
    if (client.connected()) {
        client.subscribe(ultrasonicStatusTopic);
    }
}

void MyMQTTManager::subscribeoledErrorTopic()
{
    if (client.connected()) {
        client.subscribe(oledErrorTopic);
    }
}

void MyMQTTManager::subscribetopic1()
{
    if (client.connected()) {
        client.subscribe(topic1);
    }
}

void MyMQTTManager::subscribetopic2()
{
    if (client.connected()) {
        client.subscribe(topic2);
    }
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
    }
    else if (strcmp(topic, "Skibidy/Desiree") == 0)
    {
        instance->messageSecondTopic = message;
    }
    else if (strcmp(topic, "casier1/temperature/status") == 0)
    {
        instance->messagetempStatusTopic = message;
    }
    else if (strcmp(topic, "casier1/oled/status") == 0)
    {
        instance->messageoledStatusTopic = message;
    }
    else if (strcmp(topic, "casier1/rfid/status") == 0)
    {
        instance->messagerfidStatusTopic = message;
    }
    else if (strcmp(topic, "casier1/ultrasonic/status") == 0)
    {
        instance->messageultrasonicStatusTopic = message;
    }
    else if (strcmp(topic, "casier1/temperature/error") == 0)
    {
        instance->messagetempErrorTopic = message;
    }
    else if (strcmp(topic, "casier1/oled/error") == 0)
    {
        instance->messageoledErrorTopic = message;
    }
    else if (strcmp(topic, "casier1/rfid/error") == 0)
    {
        instance->messagerfidErrorTopic = message;
    }
    else if (strcmp(topic, "casier1/ultrasonic/error") == 0)
    {
        instance->messageultrasonicErrorTopic = message;
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
    if (client.connected()) {
        client.loop();
    }
}